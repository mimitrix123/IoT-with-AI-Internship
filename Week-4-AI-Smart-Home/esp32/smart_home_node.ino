#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHT_PIN 4
#define DHT_TYPE DHT22
#define PIR_PIN 27
#define LDR_PIN 34
#define SOUND_PIN 35
#define LIGHT_RELAY 26
#define FAN_RELAY 25

const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
const char* MQTT_HOST = "YOUR_RASPBERRY_PI_HOST";
const int MQTT_PORT = 8883;
const char* MQTT_USER = "smart_home";
const char* MQTT_PASSWORD = "CHANGE_ME";
const char* SENSOR_TOPIC = "home/livingroom/sensors";
const char* CONTROL_TOPIC = "home/livingroom/control";

// Paste your broker CA certificate here for TLS. Never commit a private key.
static const char* ROOT_CA = R"EOF(
-----BEGIN CERTIFICATE-----
YOUR_CA_CERTIFICATE
-----END CERTIFICATE-----
)EOF";

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClientSecure tlsClient;
PubSubClient mqtt(tlsClient);

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void controlCallback(char* topic, byte* payload, unsigned int length) {
  String command;
  for (unsigned int i = 0; i < length; i++) command += (char)payload[i];
  command.trim();
  if (command == "light_on") digitalWrite(LIGHT_RELAY, HIGH);
  else if (command == "light_off") digitalWrite(LIGHT_RELAY, LOW);
  else if (command == "fan_on") digitalWrite(FAN_RELAY, HIGH);
  else if (command == "fan_off") digitalWrite(FAN_RELAY, LOW);
}

void connectMQTT() {
  while (!mqtt.connected()) {
    String clientId = "esp32-smart-home-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    if (mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) mqtt.subscribe(CONTROL_TOPIC);
    else delay(2000);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LIGHT_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);
  digitalWrite(LIGHT_RELAY, LOW);
  digitalWrite(FAN_RELAY, LOW);
  dht.begin();
  connectWiFi();
  tlsClient.setCACert(ROOT_CA);
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(controlCallback);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();

  float temperature = dht.readTemperature();
  if (isnan(temperature)) temperature = -127.0;
  int motion = digitalRead(PIR_PIN);
  int rawLight = analogRead(LDR_PIN);
  int rawSound = analogRead(SOUND_PIN);
  float lightLux = (4095.0 - rawLight) * 1000.0 / 4095.0;
  float soundLevel = rawSound / 4095.0;

  String payload = "{\"device_id\":\"esp32-living-room\",\"timestamp\":" + String((unsigned long)time(nullptr)) +
                   ",\"temperature_c\":" + String(temperature, 2) +
                   ",\"motion\":" + String(motion) +
                   ",\"light_lux\":" + String(lightLux, 1) +
                   ",\"sound\":" + String(soundLevel, 3) + "}";
  mqtt.publish(SENSOR_TOPIC, payload.c_str());
  delay(5000);
}
