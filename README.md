# 🚀 IoT with C — Internship Projects

A progressive 4-week embedded systems and IoT internship portfolio. The projects move from basic GPIO programming to multi-sensor monitoring, Raspberry Pi computer vision, and an ESP32 smart-agriculture system with connectivity, automation, multitasking, low-power operation, and OTA support.

## 📚 Project Roadmap

| Week | Project | Main Platform | Key Technologies |
|---|---|---|---|
| 1 | 🎲 Digital Dice | Arduino Uno / ATmega328P | GPIO, LEDs, button, buzzer, random numbers |
| 2 | 📊 Multi-Sensor Data Logger | Arduino Uno / ATmega328P | ADC, LM35, LDR, HC-SR04, UART, alerts |
| 3 | 🔔 TinyML Smart Doorbell | Raspberry Pi | Camera, OpenCV, HOG, SVM, SQLite, Streamlit, Telegram |
| 4 | 🌱 Smart Agriculture | ESP32 / ESP-IDF | FreeRTOS, sensors, relay pump, OLED, Wi-Fi, MQTT, OTA, deep sleep |

---

# 🗓️ Week 1 — Digital Dice

**Project:** [`Week-1-Digital-Dice`](Week-1-Digital-Dice/)

A seven-LED electronic dice controlled by a push button with audible buzzer feedback.

### Features

- Seven LEDs arranged as a dice display
- Push-button input with debounce
- Pseudo-random dice values from 1–6
- Rolling animation
- Piezo buzzer feedback
- Arduino Uno / ATmega328P compatible

### Hardware

- Arduino Uno
- 7 × LEDs
- 7 × 220Ω resistors
- Push button
- Piezo buzzer
- Breadboard and jumper wires

### Skills Learned

- Digital GPIO input/output
- LED pattern control
- Button debouncing
- Random number generation
- Buzzer/tone generation
- Basic embedded C/C++ programming

The implementation uses a seven-element LED mapping and deterministic dice-face patterns for values 1–6. fileciteturn13file0L2-L2

---

# 🗓️ Week 2 — Multi-Sensor Data Logger

**Project:** [`Week-2-Multi-Sensor-Data-Logger`](Week-2-Multi-Sensor-Data-Logger/)

A sensor-monitoring system that reads temperature, distance, and light and outputs timestamped CSV-style measurements over Serial/UART.

### Features

- LM35 analog temperature monitoring
- LDR light measurement
- HC-SR04 ultrasonic distance measurement
- Millisecond timestamps using `millis()`
- CSV-style Serial output
- Configurable threshold alerts
- Buzzer warning when thresholds are exceeded

### Hardware

- Arduino Uno
- LM35
- LDR + resistor divider
- HC-SR04
- Piezo buzzer
- Breadboard and jumper wires

### Example Output

```text
Timestamp_ms,Temperature_C,Distance_cm,Light_ADC,Alert
1000,27.40,48.32,712,NORMAL
2000,35.10,17.45,190,ALERT
```

### Skills Learned

- ADC sensor interfacing
- Analog-to-engineering-unit conversion
- Ultrasonic pulse timing
- UART/Serial communication
- Timestamped logging
- Threshold-based event detection

The current Week 2 firmware samples every second and reports temperature, distance, light ADC, and alert state. fileciteturn18file0L2-L2

---

# 🗓️ Week 3 — TinyML Smart Doorbell

**Project:** [`Week-3-Smart-Doorbell`](Week-3-Smart-Doorbell/)

A Raspberry Pi computer-vision doorbell prototype that captures visitors, detects faces, classifies them as KNOWN or UNKNOWN, stores events, and can send notifications.

### Features

- Raspberry Pi camera integration
- Doorbell GPIO input
- OpenCV face detection
- HOG-based lightweight face representation
- Linear SVM visitor classifier
- UNKNOWN visitor snapshots
- SQLite event logging
- Streamlit dashboard
- Optional Telegram notifications

### Architecture

```text
Doorbell Button
      ↓
Raspberry Pi Camera
      ↓
Face Detection
      ↓
Face Crop + HOG Features
      ↓
Linear Classifier
   ↙          ↘
KNOWN       UNKNOWN
  ↓             ↓
Log       Snapshot + Alert
   \           /
    ↓         ↓
      SQLite
        ↓
 Streamlit Dashboard
```

### Skills Learned

- Raspberry Pi GPIO
- Camera programming
- OpenCV
- Computer vision preprocessing
- Lightweight machine learning
- Python + scikit-learn
- SQLite databases
- Streamlit dashboards
- Notification APIs

The project intentionally uses a lightweight HOG + linear classifier approach rather than requiring a full TensorFlow Lite model, making it suitable as an educational Raspberry Pi prototype. fileciteturn15file0L2-L2

> **Privacy note:** This project processes visitor imagery. Use appropriate consent, access controls, retention policies, and applicable privacy requirements. It is not intended to be a sole physical-access-control mechanism.

---

# 🗓️ Week 4 — ESP32 Smart Agriculture

**Project:** [`Week-4-Smart-Agriculture`](Week-4-Smart-Agriculture/)

A complete ESP32-based smart-agriculture prototype for monitoring environmental conditions and automatically controlling irrigation.

### Features

- ESP32 + ESP-IDF firmware in C
- Soil-moisture ADC monitoring
- Temperature sensor interface point for DHT22
- LDR/light monitoring
- Relay-controlled DC irrigation pump
- Automatic irrigation with moisture hysteresis
- Maximum pump runtime safety timeout
- SSD1306 OLED/I²C interface
- Wi-Fi connectivity
- MQTT telemetry and pump commands
- FreeRTOS multitasking
- HTTPS OTA update hook
- Deep-sleep battery optimization
- PCB design documentation
- 3D-printed enclosure plan
- Hardware test plan

### Architecture

```text
 Soil Moisture ──┐
 Light / LDR ────┼──> ESP32
 Temperature ────┘      │
                        ├── FreeRTOS Tasks
                        ├── OLED Status
                        ├── Relay → Water Pump
                        ├── Wi-Fi → MQTT Cloud
                        ├── HTTPS OTA
                        └── Deep Sleep
```

### Irrigation Logic

```text
Soil moisture ≤ 35%
        ↓
    Pump ON
        ↓
Moisture ≥ 55% ──→ Pump OFF
        │
        └── Maximum runtime exceeded → Safety OFF
```

The 35%/55% hysteresis band reduces relay chatter, while the maximum pump runtime provides a basic protection against an indefinitely running pump. fileciteturn16file0L2-L2

### MQTT Topics

```text
agriculture/<device_id>/telemetry
agriculture/<device_id>/command
```

Example telemetry:

```json
{
  "soil_pct": 42.5,
  "temperature_c": 28.1,
  "light_pct": 67.0,
  "pump": 0,
  "rssi": -61,
  "uptime_s": 1234
}
```

### FreeRTOS Tasks

- `sensor_task` — periodic sensor sampling and telemetry
- `irrigation_task` — automatic pump control and timeout safety
- `oled_status_task` — local status updates
- Wi-Fi/MQTT callbacks — connectivity and remote commands

These runtime responsibilities are documented in the project's architecture specification. fileciteturn16file0L2-L2

### Hardware

- ESP32-WROOM development board
- Capacitive soil-moisture sensor
- DHT22/AM2302
- LDR + resistor divider
- SSD1306 I²C OLED
- 3.3V-compatible relay module
- Small DC water pump
- Separate pump power supply
- Battery + suitable regulator

### Skills Learned

- ESP32 embedded C
- ESP-IDF
- FreeRTOS task design
- ADC and GPIO interfaces
- I²C communication
- Wi-Fi networking
- MQTT IoT communication
- Relay/actuator control
- Power management
- Deep sleep
- OTA firmware updates
- PCB planning
- Enclosure engineering
- Embedded-system testing

The Week 4 firmware includes bounded pump control, Wi-Fi reconnect handling, MQTT command processing, OTA integration points, and deep-sleep preparation. fileciteturn23file0L2-L2

---

# 🧩 Overall Learning Progression

```text
WEEK 1
GPIO + LEDs + Button + Buzzer
        ↓
WEEK 2
Sensors + ADC + Timing + UART
        ↓
WEEK 3
Raspberry Pi + Camera + Computer Vision + ML
        ↓
WEEK 4
ESP32 + FreeRTOS + Wi-Fi + MQTT + Automation
        ↓
      IoT SYSTEM DESIGN
```

Across the four projects, the internship progresses from fundamental embedded programming to complete connected IoT prototypes involving sensing, processing, communication, data storage, automation, user interfaces, and hardware design.

---

# 📁 Repository Structure

```text
IoT-with-C-Internship/
│
├── README.md
├── .gitignore
│
├── Week-1-Digital-Dice/
│   ├── README.md
│   └── src/
│       └── digital_dice.ino
│
├── Week-2-Multi-Sensor-Data-Logger/
│   ├── README.md
│   └── src/
│       └── multi_sensor_logger.ino
│
├── Week-3-Smart-Doorbell/
│   ├── README.md
│   ├── requirements.txt
│   ├── .env.example
│   ├── data/
│   ├── models/
│   ├── src/
│   └── dashboard/
│
└── Week-4-Smart-Agriculture/
    ├── README.md
    ├── CMakeLists.txt
    ├── main/
    │   ├── CMakeLists.txt
    │   └── main.c
    └── docs/
        ├── ARCHITECTURE.md
        ├── PCB_DESIGN.md
        ├── ENCLOSURE.md
        └── TEST_PLAN.md
```

The repository currently contains four week directories: Digital Dice, Multi-Sensor Data Logger, TinyML Smart Doorbell, and Smart Agriculture. fileciteturn11file0L2-L2

---

# 🛠️ Technologies Used

### Embedded

- C / Embedded C
- Arduino
- ESP32
- ESP-IDF
- FreeRTOS
- GPIO
- ADC
- I²C
- UART
- PWM/tone generation

### IoT & Networking

- Wi-Fi
- MQTT
- HTTPS
- OTA firmware updates

### Raspberry Pi / Software

- Python
- OpenCV
- NumPy
- scikit-learn
- SQLite
- Streamlit
- Telegram notifications

### Hardware Engineering

- Breadboard prototyping
- Sensor interfacing
- Relay/actuator control
- PCB planning
- Power-domain design
- 3D-printed enclosure planning

---

# 🔐 Security & Safety

These projects are educational reference implementations. Before real-world deployment:

- Never commit passwords, API tokens, private keys, or production certificates.
- Use authenticated MQTT with TLS and broker ACLs.
- Validate HTTPS certificates for OTA.
- Use signed/authenticated firmware updates.
- Test pump control with a low-voltage load first.
- Keep pump power isolated from sensitive MCU power where appropriate.
- Add agricultural safety sensors such as reservoir-level, flow, leak, and pump-fault detection for a production irrigation system.
- Apply suitable privacy and retention controls to camera/visitor data.

---

# 🎯 Internship Learning Outcomes

By completing Weeks 1–4, the learner practices:

- Embedded C/C++ fundamentals
- Microcontroller GPIO and peripheral programming
- Sensor integration and calibration concepts
- Real-time task scheduling
- Serial and network communication
- IoT messaging with MQTT
- Computer vision and lightweight ML
- Database-backed event logging
- Dashboard development
- Actuator and automation control
- Low-power embedded design
- OTA firmware deployment concepts
- PCB and enclosure planning
- Testing, documentation, and system architecture

---

# 📌 Project Status

| Week | Status | Deliverable |
|---|---|---|
| Week 1 | ✅ Complete | Digital Dice prototype |
| Week 2 | ✅ Complete | Multi-Sensor Data Logger |
| Week 3 | ✅ Complete | TinyML Smart Doorbell prototype |
| Week 4 | ✅ Complete | ESP32 Smart Agriculture reference design |

---

## 👨‍💻 Internship Portfolio

This repository demonstrates the progression from **basic embedded programming → sensor systems → edge computer vision/ML → connected, automated IoT system design**.

For detailed hardware, software, build, wiring, architecture, and testing information, open the README inside each week's directory.
