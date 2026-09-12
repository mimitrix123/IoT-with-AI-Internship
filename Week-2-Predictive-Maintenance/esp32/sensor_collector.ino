#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4
#define SAMPLE_COUNT 100
#define SAMPLE_DELAY_MS 10

Adafruit_MPU6050 mpu;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.begin()) {
    Serial.println("ERROR,MPU6050_NOT_FOUND");
    while (true) delay(1000);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  tempSensor.begin();

  Serial.println("timestamp_ms,temperature_c,vibration_rms_g,vibration_peak_g");
}

void loop() {
  double sumSquares = 0.0;
  double peak = 0.0;

  for (int i = 0; i < SAMPLE_COUNT; i++) {
    sensors_event_t acceleration, gyro, temp;
    mpu.getEvent(&acceleration, &gyro, &temp);

    // Convert acceleration magnitude to approximately g units.
    double magnitude = sqrt(
      acceleration.acceleration.x * acceleration.acceleration.x +
      acceleration.acceleration.y * acceleration.acceleration.y +
      acceleration.acceleration.z * acceleration.acceleration.z
    ) / 9.80665;

    // Remove the approximate static 1 g component for this simple demo.
    double vibration = fabs(magnitude - 1.0);
    sumSquares += vibration * vibration;
    if (vibration > peak) peak = vibration;
    delay(SAMPLE_DELAY_MS);
  }

  double rms = sqrt(sumSquares / SAMPLE_COUNT);

  tempSensor.requestTemperatures();
  float temperatureC = tempSensor.getTempCByIndex(0);

  Serial.print(millis());
  Serial.print(",");
  Serial.print(temperatureC, 2);
  Serial.print(",");
  Serial.print(rms, 4);
  Serial.print(",");
  Serial.println(peak, 4);

  delay(500);
}
