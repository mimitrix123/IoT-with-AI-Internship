#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_https_ota.h"

#define WIFI_SSID "CHANGE_ME"
#define WIFI_PASS "CHANGE_ME"
#define MQTT_URI "mqtts://broker.example.com:8883"
#define MQTT_USER "CHANGE_ME"
#define MQTT_PASS "CHANGE_ME"
#define DEVICE_ID "farm-node-01"
#define OTA_URL "https://example.com/firmware.bin"

#define SOIL_ADC ADC1_CHANNEL_6
#define LIGHT_ADC ADC1_CHANNEL_7
#define DHT_PIN GPIO_NUM_4
#define RELAY_PIN GPIO_NUM_26
#define I2C_SDA GPIO_NUM_21
#define I2C_SCL GPIO_NUM_22
#define OLED_ADDR 0x3C
#define MOISTURE_START_PCT 35.0f
#define MOISTURE_STOP_PCT 55.0f
#define MAX_PUMP_RUNTIME_MS 20000
#define SAMPLE_PERIOD_MS 5000
#define DEEP_SLEEP_SECONDS 300
#define WIFI_CONNECTED_BIT BIT0

static const char *TAG = "SMART_AGRI";
static EventGroupHandle_t wifi_events;
static esp_mqtt_client_handle_t mqtt;
static volatile bool pump_on = false;
static volatile bool auto_mode = true;
static TickType_t pump_started = 0;

typedef struct { float soil_pct; float temp_c; float light_pct; int rssi; } telemetry_t;
static telemetry_t latest = {0};

static void pump_set(bool on) {
    if (on && !pump_on) pump_started = xTaskGetTickCount();
    pump_on = on;
    gpio_set_level(RELAY_PIN, on ? 1 : 0);
}

static float adc_to_percent(int raw, int dry, int wet) {
    if (wet == dry) return 0;
    float p = 100.0f * ((float)dry - raw) / ((float)dry - wet);
    return fminf(100.0f, fmaxf(0.0f, p));
}

static void read_sensors(void) {
    const int soil_dry = 3000, soil_wet = 1300;
    latest.soil_pct = adc_to_percent(adc1_get_raw(SOIL_ADC), soil_dry, soil_wet);
    latest.light_pct = 100.0f * adc1_get_raw(LIGHT_ADC) / 4095.0f;
    latest.temp_c = NAN; // Add a validated DHT22 component/driver for production.
    wifi_ap_record_t ap;
    if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) latest.rssi = ap.rssi;
}

static void oled_init(void) {
    i2c_config_t cfg = {.mode=I2C_MODE_MASTER,.sda_io_num=I2C_SDA,.scl_io_num=I2C_SCL,
        .sda_pullup_en=GPIO_PULLUP_ENABLE,.scl_pullup_en=GPIO_PULLUP_ENABLE,.master.clk_speed=400000};
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &cfg));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, cfg.mode, 0, 0, 0));
    // SSD1306 drawing commands can be supplied by the selected ESP-IDF OLED component.
}

static void oled_status_task(void *arg) {
    while (1) {
        ESP_LOGI(TAG, "OLED | soil=%.1f%% temp=%s light=%.1f%% pump=%d auto=%d",
                 latest.soil_pct, isnan(latest.temp_c) ? "N/A" : "OK", latest.light_pct, pump_on, auto_mode);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void irrigation_task(void *arg) {
    while (1) {
        if (auto_mode) {
            if (!pump_on && latest.soil_pct <= MOISTURE_START_PCT) pump_set(true);
            if (pump_on && latest.soil_pct >= MOISTURE_STOP_PCT) pump_set(false);
        }
        if (pump_on && (xTaskGetTickCount() - pump_started) > pdMS_TO_TICKS(MAX_PUMP_RUNTIME_MS)) {
            ESP_LOGW(TAG, "Pump timeout safety stop"); pump_set(false);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void mqtt_publish(void) {
    if (!(xEventGroupGetBits(wifi_events) & WIFI_CONNECTED_BIT) || !mqtt) return;
    char topic[96], payload[256], temp[24];
    snprintf(topic, sizeof(topic), "agriculture/%s/telemetry", DEVICE_ID);
    if (isnan(latest.temp_c)) strcpy(temp, "null"); else snprintf(temp, sizeof(temp), "%.2f", latest.temp_c);
    snprintf(payload, sizeof(payload),
        "{\"soil_pct\":%.1f,\"temperature_c\":%s,\"light_pct\":%.1f,\"pump\":%d,\"rssi\":%d,\"uptime_s\":%lu}",
        latest.soil_pct, temp, latest.light_pct, pump_on, latest.rssi, (unsigned long)(esp_log_timestamp()/1000));
    esp_mqtt_client_publish(mqtt, topic, payload, 0, 1, 0);
}

static void sensor_task(void *arg) {
    while (1) { read_sensors(); mqtt_publish(); vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_MS)); }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t e = event_data;
    if (event_id == MQTT_EVENT_CONNECTED) {
        char topic[96]; snprintf(topic, sizeof(topic), "agriculture/%s/command", DEVICE_ID);
        esp_mqtt_client_subscribe(e->client, topic, 1);
    } else if (event_id == MQTT_EVENT_DATA) {
        if (e->data_len == 7 && strncmp(e->data, "PUMP_ON", 7) == 0) { auto_mode=false; pump_set(true); }
        else if (e->data_len == 8 && strncmp(e->data, "PUMP_OFF", 8) == 0) { auto_mode=false; pump_set(false); }
        else if (e->data_len == 4 && strncmp(e->data, "AUTO", 4) == 0) auto_mode=true;
    }
}

static void mqtt_start(void) {
    esp_mqtt_client_config_t cfg = {
        .broker.address.uri = MQTT_URI,
        .credentials.username = MQTT_USER,
        .credentials.authentication.password = MQTT_PASS,
        .broker.verification.skip_cert_common_name_check = true // demo only; use CA validation in production
    };
    mqtt = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_register_event(mqtt, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt);
}

static void wifi_event(void *arg, esp_event_base_t base, int32_t id, void *data) {
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) esp_wifi_connect();
    else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) { xEventGroupClearBits(wifi_events, WIFI_CONNECTED_BIT); esp_wifi_connect(); }
    else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) xEventGroupSetBits(wifi_events, WIFI_CONNECTED_BIT);
}

static void wifi_start(void) {
    wifi_events = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init()); ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta(); wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT(); ESP_ERROR_CHECK(esp_wifi_init(&init));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event, NULL));
    wifi_config_t cfg = {0}; strncpy((char*)cfg.sta.ssid, WIFI_SSID, sizeof(cfg.sta.ssid)); strncpy((char*)cfg.sta.password, WIFI_PASS, sizeof(cfg.sta.password));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg)); ESP_ERROR_CHECK(esp_wifi_start());
}

static void ota_check(void) {
    if (strncmp(OTA_URL, "https://example.com", 19) == 0) return;
    esp_http_client_config_t http = {.url=OTA_URL, .timeout_ms=10000};
    esp_https_ota_config_t ota = {.http_config=&http};
    esp_err_t err = esp_https_ota(&ota);
    if (err == ESP_OK) esp_restart(); else ESP_LOGW(TAG, "OTA skipped/failed: %s", esp_err_to_name(err));
}

static void prepare_deep_sleep(void) {
    pump_set(false); if (mqtt) esp_mqtt_client_stop(mqtt); esp_wifi_stop();
    esp_sleep_enable_timer_wakeup((uint64_t)DEEP_SLEEP_SECONDS * 1000000ULL); esp_deep_sleep_start();
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    gpio_config_t relay = {.pin_bit_mask=1ULL<<RELAY_PIN,.mode=GPIO_MODE_OUTPUT}; ESP_ERROR_CHECK(gpio_config(&relay)); pump_set(false);
    adc1_config_width(ADC_WIDTH_BIT_12); adc1_config_channel_atten(SOIL_ADC, ADC_ATTEN_DB_11); adc1_config_channel_atten(LIGHT_ADC, ADC_ATTEN_DB_11);
    oled_init(); wifi_start();
    xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
    xTaskCreate(irrigation_task, "irrigation", 3072, NULL, 6, NULL);
    xTaskCreate(oled_status_task, "oled", 3072, NULL, 3, NULL);
    xEventGroupWaitBits(wifi_events, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, pdMS_TO_TICKS(15000));
    mqtt_start(); vTaskDelay(pdMS_TO_TICKS(3000)); ota_check();
    prepare_deep_sleep();
}
