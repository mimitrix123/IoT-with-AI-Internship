#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

// Copy src/secrets.example.h to src/secrets.h and replace the placeholders.
#include "secrets.h"

#define DHT_PIN 4
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient client;

unsigned long lastUpload = 0;
const unsigned long uploadInterval = 20000UL; // 20 seconds

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000UL) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi connection timed out.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  connectWiFi();
  ThingSpeak.begin(client);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
    delay(1000);
    return;
  }

  if (millis() - lastUpload < uploadInterval) {
    delay(100);
    return;
  }

  lastUpload = millis();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT11 read failed. Check wiring and power.");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature, 2);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity, 2);
  Serial.println(" %");

  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);

  int statusCode = ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);

  if (statusCode == 200) {
    Serial.println("ThingSpeak update successful. HTTP status: 200");
  } else {
    Serial.print("ThingSpeak update failed. HTTP status: ");
    Serial.println(statusCode);
  }
}
