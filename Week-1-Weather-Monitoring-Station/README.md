# Mini Project — Week 1: ESP32 Weather Monitoring Station

A beginner-friendly IoT weather station built with an **ESP32 + DHT11**. The device reads temperature and relative humidity, publishes the readings to **ThingSpeak**, and provides live cloud charts. ThingSpeak React/MATLAB Analysis can be used to send email alerts when values cross configured thresholds.

## Objectives

- Read temperature and humidity from a DHT11 sensor.
- Connect an ESP32 to Wi-Fi.
- Upload measurements to ThingSpeak.
- View live temperature and humidity graphs.
- Configure email alerts for extreme values.
- Keep credentials out of Git history.

## Hardware

| Component | Quantity |
|---|---:|
| ESP32 development board | 1 |
| DHT11 sensor/module | 1 |
| Breadboard | 1 |
| Jumper wires | 3–4 |
| USB cable | 1 |

### Wiring

For a common 3-pin DHT11 module:

| DHT11 | ESP32 |
|---|---|
| VCC | 3.3V |
| DATA | GPIO 4 |
| GND | GND |

If using a bare 4-pin DHT11, add a **4.7k–10kΩ pull-up resistor** between DATA and 3.3V. Check your particular sensor module's pinout before powering it.

## ThingSpeak setup

1. Create a ThingSpeak channel.
2. Enable **Field 1 = Temperature (°C)**.
3. Enable **Field 2 = Humidity (%)**.
4. Copy the channel's **Write API Key**.
5. Create charts for Fields 1 and 2. ThingSpeak supports dynamic line charts for channel fields. 
6. Keep the channel private unless you intentionally want to publish the readings.

The ESP32 sends one update every 20 seconds. This is deliberately slower than the common 15-second minimum update interval for standard ThingSpeak channel writes.

## Arduino IDE setup

Install/select an ESP32 board package in Arduino IDE, then install these libraries from **Library Manager**:

- **DHT sensor library** by Adafruit
- **Adafruit Unified Sensor**
- **ThingSpeak** by MathWorks

Open `src/weather_station.ino` and set the Wi-Fi and ThingSpeak values. For local development, the recommended approach is to copy `src/secrets.example.h` to `src/secrets.h` and put credentials there. `secrets.h` is ignored by Git.

### Configuration

```cpp
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define THINGSPEAK_API_KEY "YOUR_WRITE_API_KEY"
#define THINGSPEAK_CHANNEL_ID 1234567
```

Do **not** commit real Wi-Fi passwords or API keys.

## How it works

```text
DHT11
  │
  │ temperature + humidity
  ▼
ESP32
  │
  │ Wi-Fi / HTTPS
  ▼
ThingSpeak Channel
  ├── Field 1: Temperature °C ──► Live chart
  └── Field 2: Humidity % ──────► Live chart
                 │
                 ▼
             React rule
                 │
                 ▼
           Email alert
```

## Email alerts

ThingSpeak's **React** app can monitor channel data and trigger an action when a condition is met. A practical configuration is:

### High-temperature alert

- Apps → React → New React
- Condition Type: **Numeric**
- Test Frequency: **On Data Insertion**
- Channel: your weather channel
- Field: **1 — Temperature**
- Condition: **is greater than or equal to**
- Value: **35 °C** (change this to suit your project)
- Action: use a ThingHTTP or MATLAB Analysis workflow that sends the alert email.

### High-humidity alert

Create a second React rule:

- Field: **2 — Humidity**
- Condition: **is greater than or equal to**
- Value: **80 %**

ThingSpeak's current documentation describes React as the condition-monitoring mechanism and documents email alerts through MATLAB Analysis using the ThingSpeak Alerts API. See the official references below.

## Testing checklist

- [ ] ESP32 connects to Wi-Fi.
- [ ] Serial Monitor shows valid DHT11 readings.
- [ ] ThingSpeak receives Field 1 temperature values.
- [ ] ThingSpeak receives Field 2 humidity values.
- [ ] Both charts update.
- [ ] Extreme-value React rule triggers an email.
- [ ] No secrets are committed to GitHub.

## Expected Serial Monitor output

```text
Connecting to Wi-Fi....
Wi-Fi connected.
IP address: 192.168.x.x
Temperature: 28.40 °C | Humidity: 67.00 %
ThingSpeak update successful. HTTP status: 200
```

## Project structure

```text
Week-1-Weather-Monitoring-Station/
├── README.md
└── src/
    ├── secrets.example.h
    └── weather_station.ino
```

## Official references

- ThingSpeak: https://thingspeak.mathworks.com/
- ThingSpeak React: https://www.mathworks.com/help/thingspeak/react-app.html
- ThingSpeak email alert example: https://in.mathworks.com/help/thingspeak/act-on-your-data.html

## Learning outcomes

By completing this project, you practice embedded C++/Arduino programming, GPIO sensor interfacing, Wi-Fi networking, REST/cloud telemetry, time-based data logging, cloud visualization, and event-driven alerting — the core workflow of a small IoT system.
