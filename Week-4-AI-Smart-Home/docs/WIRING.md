# Wiring Guide

## ESP32 sensor node

| Component | ESP32 pin | Notes |
|---|---:|---|
| DHT22 DATA | GPIO 4 | 10 kΩ pull-up to 3.3 V if required |
| PIR OUT | GPIO 27 | Power from the sensor's supported supply |
| LDR analog output | GPIO 34 | ADC input; calibrate lux conversion |
| Sound sensor analog | GPIO 35 | ADC input; normalize/calibrate level |
| Light relay input | GPIO 26 | Use a properly rated isolated relay module |
| Fan relay input | GPIO 25 | Use a properly rated isolated relay module |
| GND | GND | Common low-voltage ground |

### Safety

Do not connect mains voltage directly to an ESP32. Use certified isolated relay hardware and appropriate electrical protection. For a student prototype, test with low-voltage loads first.

The LDR conversion in firmware is an approximate demonstration mapping, not a calibrated lux measurement. Calibrate against a real light meter or use a digital lux sensor such as BH1750 for accurate readings.
