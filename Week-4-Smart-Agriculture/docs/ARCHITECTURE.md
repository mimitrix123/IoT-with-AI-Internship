# Architecture

## Runtime tasks

- `sensor_task`: samples ADC channels, reads Wi-Fi RSSI, and publishes telemetry every 5 seconds.
- `irrigation_task`: applies moisture hysteresis and the 20-second pump safety timeout every 500 ms.
- `oled_status_task`: refreshes local status every second.
- ESP-IDF Wi-Fi and MQTT event callbacks handle connectivity and commands.

## State machine

```text
BOOT -> SENSOR INIT -> WIFI CONNECT -> MQTT CONNECT
                         |                |
                         v                v
                    SAMPLE/REPORT <--> AUTO IRRIGATION
                         |
                         v
                    OTA CHECK
                         |
                         v
                    DEEP SLEEP
                         |
                         v
                       WAKE
```

## Fault handling

- Pump is forced OFF during boot and before deep sleep.
- Pump runtime is bounded to prevent a stuck irrigation command from running indefinitely.
- Wi-Fi disconnect clears the connected event and retries.
- OTA is disabled when the placeholder URL is unchanged.
- Sensor failures must be represented as invalid/unknown values; do not invent readings.

## Production hardening

Use a real DHT22 component/driver and SSD1306 display component, configure a trusted CA for MQTT/HTTPS, authenticate OTA images, store credentials securely, add a flow sensor and reservoir-level switch, and use a hardware watchdog/fail-safe relay design appropriate to the pump.
