#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// ===================== USER CONFIGURATION =====================
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define DHT_PIN 4
#define DHT_TYPE DHT11

// ThingSpeak channel configuration
#define THINGSPEAK_API_KEY "YOUR_THINGSPEAK_WRITE_API_KEY"
#define THINGSPEAK_URL "http://api.thingspeak.com/update"

// Alert thresholds. Adjust for your project/environment.
#define TEMP_HIGH_C 40.0
#define TEMP_LOW_C 5.0
#define HUMIDITY_HIGH 85.0
#define HUMIDITY_LOW 20.0

// ThingSpeak free channels should not be updated more often than every 15 seconds.
const unsigned long UPDATE_INTERVAL_MS = 20000;

DHT dht(DHT_PIN, DHT_TYPE);
unsigned long lastUpdate = 0;

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi connection failed.");
  }
}

bool sendToThingSpeak(float temperature, float humidity) {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;
  String url = String(THINGSPEAK_URL) +
               "?api_key=" + THINGSPEAK_API_KEY +
               "&field1=" + String(temperature, 2) +
               "&field2=" + String(humidity, 2);

  http.begin(url);
  int responseCode = http.GET();
  String response = http.getString();
  http.end();

  Serial.print("ThingSpeak HTTP response: ");
  Serial.println(responseCode);
  Serial.print("ThingSpeak entry: ");
  Serial.println(response);

  return responseCode == 200 && response != "0";
}

void printReadings(float temperature, float humidity) {
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");

  if (temperature >= TEMP_HIGH_C || temperature <= TEMP_LOW_C ||
      humidity >= HUMIDITY_HIGH || humidity <= HUMIDITY_LOW) {
    Serial.println("ALERT: Extreme temperature/humidity detected!");
    Serial.println("Configure ThingSpeak React + ThingHTTP/Email for email notifications.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  connectWiFi();
}

void loop() {
  if (millis() - lastUpdate < UPDATE_INTERVAL_MS) {
    delay(100);
    return;
  }
  lastUpdate = millis();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("ERROR: Failed to read DHT11 sensor.");
    return;
  }

  printReadings(temperature, humidity);

  if (sendToThingSpeak(temperature, humidity)) {
    Serial.println("Data uploaded successfully.\n");
  } else {
    Serial.println("Data upload failed.\n");
  }
}
