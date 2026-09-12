# System Architecture

## Layers

1. **Edge sensing:** ESP32 reads temperature, motion, light, and sound.
2. **Secure transport:** telemetry is published with MQTT over TLS.
3. **Gateway:** Raspberry Pi runs Mosquitto and the Python MQTT gateway.
4. **Data layer:** SQLite stores raw telemetry and AI outputs.
5. **AI layer:** occupancy classification estimates whether a room is occupied; energy regression estimates relative demand; a policy creates energy-saving recommendations.
6. **Experience layer:** Streamlit provides real-time trends and recommendations; a local control API and optional speech-recognition client provide voice control.
7. **Notifications:** Telegram sends selected high-demand alerts to a mobile device.
8. **Actuation:** MQTT control commands reach ESP32 relay outputs.

## Data flow

```text
Sensors -> ESP32 -> MQTT/TLS -> Raspberry Pi -> SQLite
                                      |
                                      +-> Occupancy model
                                      +-> Energy model
                                      +-> Recommendation
                                      +-> Telegram
                                      +-> Dashboard
                                      +-> Control API -> MQTT -> Relays
```

## AI limitations

The included dataset is intentionally small and illustrative. Model metrics from it are not evidence of production accuracy. A real deployment needs weeks of representative telemetry, occupancy labels, train/test separation by time, calibration, monitoring for drift, and safe fallback rules.
