# Week 4 — ESP32 Smart Agriculture (C / ESP-IDF)

A complete educational smart-agriculture node built around ESP32 and ESP-IDF. It monitors soil moisture, temperature, and light; drives a low-voltage irrigation pump through a relay; shows status on an SSD1306 OLED; publishes telemetry over MQTT; uses FreeRTOS tasks; enters deep sleep between measurement cycles; and supports OTA firmware updates.

## Features

- ESP32 firmware in C using ESP-IDF/FreeRTOS
- Capacitive soil-moisture ADC input
- DHT22 temperature input
- LDR/light ADC input
- Relay-controlled DC water pump
- Automatic irrigation with hysteresis and maximum runtime
- SSD1306 I2C OLED status display
- Wi-Fi station connection with reconnect handling
- MQTT telemetry and irrigation state reporting
- MQTT command topic for manual irrigation control
- HTTPS OTA update hook
- Deep-sleep cycle for battery-oriented operation
- Watchdog-safe task design and bounded sensor/pump timing
- PCB, wiring, enclosure, and test documentation

## Architecture

```text
 Soil probe ---- ADC1 ----+
 LDR ---------- ADC1 ----+|
 DHT22 -------- GPIO ------| ESP32
 OLED <-------- I2C -------|  FreeRTOS tasks
 Relay <------- GPIO ------|      |
                           |      +--> Wi-Fi --> MQTT broker/cloud
 Battery ------------------+      +--> HTTPS OTA
                                  +--> Deep Sleep
```

## Suggested hardware

| Part | Purpose |
|---|---|
| ESP32-WROOM development board | MCU + Wi-Fi + FreeRTOS |
| Capacitive soil-moisture sensor | Soil moisture |
| DHT22/AM2302 | Temperature |
| LDR + resistor divider | Relative light |
| 0.96-inch SSD1306 I2C OLED | Local status |
| 1-channel 3.3-V logic relay module | Pump switching |
| Small DC water pump + separate supply | Irrigation |
| Li-ion/LiPo + suitable regulator | Battery operation |

**Safety:** prototype the pump with an isolated low-voltage DC supply. Do not connect mains voltage to the relay unless the enclosure, isolation, fusing, clearances, and installation are designed and certified for mains work.

## Pin map

| Signal | ESP32 | Notes |
|---|---:|---|
| Soil moisture | GPIO34 / ADC1_CH6 | ADC-only input |
| LDR | GPIO35 / ADC1_CH7 | ADC-only input |
| DHT22 DATA | GPIO4 | Pull-up per sensor/module requirements |
| OLED SDA | GPIO21 | I2C |
| OLED SCL | GPIO22 | I2C |
| Pump relay | GPIO26 | Active-high by default; change in `main.c` if needed |

Keep the sensor ground, ESP32 ground, and relay-control ground common where the module design requires it. Keep the pump power path separate from the ESP32 regulator path.

## Build and flash

Install ESP-IDF, then:

```bash
cd Week-4-Smart-Agriculture
idf.py set-target esp32
idf.py menuconfig
idf.py build
idf.py -p PORT flash monitor
```

Configure Wi-Fi, MQTT broker URI, MQTT credentials, moisture calibration, and OTA URL using `idf.py menuconfig` or project configuration before deployment. Never commit production credentials.

## MQTT contract

Telemetry topic:

```text
agriculture/<device_id>/telemetry
```

Example payload:

```json
{"soil_pct":42.5,"temperature_c":28.1,"light_pct":67.0,"pump":0,"rssi":-61,"uptime_s":1234}
```

Command topic:

```text
agriculture/<device_id>/command
```

Supported demonstration commands:

```text
PUMP_ON
PUMP_OFF
AUTO
```

For production, add MQTT TLS, broker ACLs, authenticated OTA, signed firmware, anti-replay controls, and rate limits.

## Irrigation logic

Automatic irrigation starts when soil moisture is at or below `MOISTURE_START_PCT` and stops when it reaches `MOISTURE_STOP_PCT` or when `MAX_PUMP_RUNTIME_MS` is exceeded. The hysteresis band prevents relay chatter. A real farm deployment should add tank-empty, flow, leak, rain, and pump-fault interlocks.

## Power strategy

The node samples, displays, publishes, optionally checks OTA, then enters deep sleep. `DEEP_SLEEP_SECONDS` controls the interval. Wi-Fi/MQTT and the OLED are powered only during the active window. For a real battery design, measure sleep current on the final PCB and switch sensor/relay power rails where appropriate.

## OTA

The firmware contains an HTTPS OTA entry point. Set an authenticated HTTPS firmware URL and CA validation for deployment. The example intentionally does not contain a real certificate, password, or endpoint.

## PCB plan

See `docs/PCB_DESIGN.md` for schematic blocks, placement, power-domain guidance, and fabrication checklist.

## Enclosure plan

See `docs/ENCLOSURE.md` for a printable two-piece IP-rated-style concept, mounting dimensions, ventilation, cable glands, and print recommendations.

## Test plan

1. Validate ADC readings against dry/wet soil reference samples.
2. Verify DHT22 temperature stability.
3. Verify OLED boot/status screen.
4. Confirm Wi-Fi reconnect after AP interruption.
5. Confirm MQTT telemetry and command handling.
6. Test relay using an LED/low-voltage load before connecting the pump.
7. Confirm automatic irrigation hysteresis and timeout.
8. Measure active and deep-sleep current.
9. Perform OTA with a signed/test firmware image.
10. Run a 24-hour soak test with telemetry logging.

## Folder layout

```text
Week-4-Smart-Agriculture/
├── CMakeLists.txt
├── README.md
├── main/
│   ├── CMakeLists.txt
│   └── main.c
└── docs/
    ├── ARCHITECTURE.md
    ├── PCB_DESIGN.md
    ├── ENCLOSURE.md
    └── TEST_PLAN.md
```

This is an educational reference design, not a certified agricultural, electrical, or safety controller.
