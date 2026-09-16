/*
 * Mini Project — Week 2: Multi-Sensor Data Logger
 * IoT-with-C-Internship
 *
 * Sensors:
 *   LM35  -> A0
 *   LDR   -> A1
 *   HC-SR04 TRIG -> D6
 *   HC-SR04 ECHO -> D7
 *   Buzzer -> D8
 *
 * Serial output: 9600 baud, CSV format.
 */

const byte LM35_PIN = A0;
const byte LDR_PIN = A1;
const byte TRIG_PIN = 6;
const byte ECHO_PIN = 7;
const byte BUZZER_PIN = 8;

const unsigned long LOG_INTERVAL_MS = 1000UL;
const unsigned long ECHO_TIMEOUT_US = 30000UL;

// Alert thresholds — adjust for the application.
const float TEMP_HIGH_C = 35.0;
const float DISTANCE_LOW_CM = 20.0;
const int LIGHT_LOW_ADC = 250;

unsigned long lastLogTime = 0;

float readTemperatureC() {
  int adc = analogRead(LM35_PIN);
  float voltage = (adc * 5.0) / 1023.0;
  return voltage * 100.0; // LM35: ~10 mV/°C
}

float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);

  if (duration == 0) {
    return -1.0; // No echo / timeout
  }

  // Speed of sound: approximately 0.0343 cm/us; divide by 2 for round trip.
  return (duration * 0.0343) / 2.0;
}

int readLight() {
  return analogRead(LDR_PIN);
}

bool thresholdExceeded(float temperatureC, float distanceCm, int lightAdc) {
  bool temperatureAlert = temperatureC >= TEMP_HIGH_C;
  bool distanceAlert = distanceCm >= 0.0 && distanceCm <= DISTANCE_LOW_CM;
  bool lightAlert = lightAdc <= LIGHT_LOW_ADC;

  return temperatureAlert || distanceAlert || lightAlert;
}

void buzzerAlert() {
  tone(BUZZER_PIN, 1500, 100);
  delay(130);
  tone(BUZZER_PIN, 2000, 100);
  delay(130);
  noTone(BUZZER_PIN);
}

void printCsv(unsigned long timestamp, float temperatureC,
             float distanceCm, int lightAdc, bool alert) {
  Serial.print(timestamp);
  Serial.print(',');

  Serial.print(temperatureC, 2);
  Serial.print(',');

  if (distanceCm < 0.0) {
    Serial.print("TIMEOUT");
  } else {
    Serial.print(distanceCm, 2);
  }
  Serial.print(',');

  Serial.print(lightAdc);
  Serial.print(',');
  Serial.println(alert ? "ALERT" : "NORMAL");
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(TRIG_PIN, LOW);
  noTone(BUZZER_PIN);

  Serial.begin(9600);
  delay(100);

  Serial.println("Timestamp_ms,Temperature_C,Distance_cm,Light_ADC,Alert");
}

void loop() {
  unsigned long now = millis();

  if (now - lastLogTime >= LOG_INTERVAL_MS) {
    lastLogTime = now;

    float temperatureC = readTemperatureC();
    float distanceCm = readDistanceCm();
    int lightAdc = readLight();

    bool alert = thresholdExceeded(temperatureC, distanceCm, lightAdc);

    printCsv(now, temperatureC, distanceCm, lightAdc, alert);

    if (alert) {
      buzzerAlert();
    }
  }
}
