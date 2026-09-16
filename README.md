# IoT-with-AI-Internship

Build intelligent IoT systems powered by AI and machine learning.

## Internship Projects

### Week 1 — ESP32 Weather Monitoring Station

ESP32 + DHT11 weather station that reads temperature and humidity, sends telemetry to ThingSpeak, displays live cloud graphs, and supports threshold-based email alerts through ThingSpeak automation.

**Folder:** `Week-1-Weather-Monitoring-Station/`

**Key skills:** ESP32, DHT11, Arduino C++, Wi-Fi, REST/cloud telemetry, ThingSpeak, real-time visualization, threshold alerts.

### Week 2 — Predictive Maintenance

`Week-2-Predictive-Maintenance/`

### Week 3 — Smart Doorbell

`Week-3-Smart-Doorbell/`

### Week 4 — AI Smart Home

`Week-4-AI-Smart-Home/`

## Week 1 Quick Start

1. Open `Week-1-Weather-Monitoring-Station/src/weather_station.ino` in Arduino IDE.
2. Copy `src/secrets.example.h` to `src/secrets.h` locally.
3. Add your Wi-Fi credentials and ThingSpeak Write API Key/channel ID.
4. Install the ESP32 board package plus DHT sensor, Adafruit Unified Sensor, and ThingSpeak libraries.
5. Wire DHT11 DATA to ESP32 GPIO 4, VCC to 3.3V, and GND to GND.
6. Upload the firmware and open Serial Monitor at 115200 baud.
7. Configure ThingSpeak Field 1 (temperature), Field 2 (humidity), live charts, and React/email alert rules.

> **Security:** Real credentials must remain local and must never be committed to GitHub.
