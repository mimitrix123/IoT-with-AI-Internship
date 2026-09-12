# Week 4 — AI-Powered Smart Home

A complete ESP32 + Raspberry Pi smart-home prototype combining IoT sensing, machine learning, energy optimization, real-time monitoring, voice commands, mobile notifications, and encrypted MQTT communication.

## Features

- ESP32 sensor node for temperature, motion, light, and sound
- MQTT telemetry from ESP32 to Raspberry Pi
- MQTT over TLS architecture for encrypted communication
- Raspberry Pi gateway with SQLite event/time-series storage
- AI occupancy prediction using Random Forest
- AI energy-demand prediction and recommendation engine
- Real-time Streamlit dashboard
- Local voice-command interface with optional speech recognition
- Telegram mobile notifications
- Optional relay control for lights/fan
- Environment-variable based secrets; no credentials committed
- Deployment examples for Raspberry Pi and Mosquitto

## Architecture

```text
ESP32 sensors
  |  DHT22 / PIR / LDR / sound
  |  MQTT + TLS
  v
Raspberry Pi + Mosquitto
  |
  +--> MQTT gateway --> SQLite
  |         |
  |         +--> Occupancy ML
  |         +--> Energy ML
  |         +--> Recommendation publisher
  |         +--> Telegram notifier
  |
  +--> Streamlit real-time dashboard
  |
  +--> Voice/control API --> MQTT control --> ESP32 relays
```

## Repository layout

```text
Week-4-AI-Smart-Home/
├── README.md
├── requirements.txt
├── .env.example
├── data/
│   └── sample_sensor_data.csv
├── esp32/
│   └── smart_home_node.ino
├── gateway/
│   ├── config.py
│   ├── database.py
│   ├── ml_inference.py
│   ├── notifier.py
│   └── mqtt_gateway.py
├── ml/
│   └── train_models.py
├── dashboard/
│   └── app.py
├── voice/
│   └── voice_control.py
├── api/
│   └── control_server.py
└── docs/
    ├── ARCHITECTURE.md
    ├── DEPLOYMENT.md
    └── WIRING.md
```

## Quick start

### 1. Raspberry Pi Python environment

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
cp .env.example .env
```

### 2. Train the demonstration models

```bash
python ml/train_models.py
```

The script trains an occupancy classifier and an energy-demand regressor from the included sample dataset. For a production system, replace the demonstration data with real labeled household telemetry.

### 3. Configure MQTT/TLS

Install Mosquitto on the Raspberry Pi and configure a TLS listener. See `docs/DEPLOYMENT.md`. Put the CA certificate path and MQTT credentials in `.env` and configure the same CA certificate in the ESP32 firmware.

### 4. Flash the ESP32

Install these Arduino libraries:

- DHT sensor library
- Adafruit Unified Sensor
- PubSubClient

Set Wi-Fi and MQTT credentials locally before flashing. Do not commit real credentials or private certificates.

### 5. Start the gateway

```bash
python gateway/mqtt_gateway.py
```

### 6. Start the dashboard

```bash
streamlit run dashboard/app.py
```

### 7. Start optional control/voice services

```bash
python api/control_server.py
python voice/voice_control.py
```

Voice recognition is optional; typed commands can be used when microphone dependencies are unavailable.

## Example MQTT payload

```json
{
  "device_id": "esp32-living-room",
  "timestamp": 1720000000,
  "temperature_c": 27.4,
  "motion": 1,
  "light_lux": 320,
  "sound": 0.18
}
```

## AI design

**Occupancy model:** predicts occupied/unoccupied from temperature, motion, light, sound, hour, and weekday. The model is intended as a learning/demo system; real deployment should use household-specific labeled data and evaluate false positives/negatives.

**Energy model:** predicts relative energy demand from occupancy and environmental conditions. A policy layer converts the prediction into recommendations such as reducing lighting when daylight is sufficient or relaxing HVAC operation when the room is unoccupied.

## Security

- MQTT traffic should use TLS.
- Use unique MQTT credentials and a restricted broker account.
- Keep private CA/client keys outside Git.
- Protect the control API with an access token on a real deployment.
- Store Telegram credentials only in `.env` or another secret manager.
- Do not expose Mosquitto or the control API directly to the public Internet.
- This is an educational smart-home prototype, not a certified security or safety system.

## Deployment

See `docs/DEPLOYMENT.md` for Raspberry Pi services, MQTT TLS setup, environment variables, and operational checks.

## Future improvements

- Add multiple ESP32 nodes and room-level models
- Replace synthetic data with real labeled occupancy/energy data
- Add TensorFlow Lite/Edge Impulse models for edge inference
- Add Home Assistant integration
- Add secure OTA firmware updates
- Add role-based dashboard authentication
- Add long-term energy forecasting and appliance-level disaggregation
