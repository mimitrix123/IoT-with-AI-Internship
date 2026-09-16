# Mini Project — Week 2: Predictive Maintenance System

## Objective
Build an IoT + Machine Learning predictive-maintenance prototype that collects machine temperature and vibration telemetry from an ESP32, learns normal operating behavior, detects anomalies, and raises a maintenance alert through a Python dashboard.

## Architecture

```text
MPU6050 vibration + DS18B20 temperature
                 ↓
               ESP32
                 ↓ USB Serial CSV
        Python serial collector
                 ↓
          pandas preprocessing
                 ↓
       StandardScaler + Isolation Forest
                 ↓
       NORMAL / ANOMALY prediction
                 ↓
          Streamlit dashboard
                 ↓
       Maintenance + email alert
```

## Hardware
- ESP32
- MPU6050 accelerometer/gyroscope (vibration proxy)
- DS18B20 temperature sensor
- Breadboard and jumper wires
- 4.7k–10kΩ pull-up resistor for the DS18B20 data line

### Example wiring
| Sensor | ESP32 |
|---|---|
| MPU6050 VCC | 3.3V |
| MPU6050 GND | GND |
| MPU6050 SDA | GPIO 21 |
| MPU6050 SCL | GPIO 22 |
| DS18B20 VCC | 3.3V |
| DS18B20 GND | GND |
| DS18B20 DATA | GPIO 4 |

## Software
- Python 3.10+
- Arduino IDE / PlatformIO
- pandas, NumPy, scikit-learn, Streamlit, PySerial
- Arduino libraries: Adafruit MPU6050, Adafruit Unified Sensor, OneWire, DallasTemperature

## Project structure

```text
Week-2-Predictive-Maintenance/
├── README.md
├── .env.example
├── requirements.txt
├── data/
│   ├── sample_machine_data.csv
│   └── collect_serial.py
├── esp32/
│   └── sensor_collector.ino
├── ml/
│   └── train_model.py
└── dashboard/
    ├── app.py
    └── notifier.py
```

## Step 1 — Install Python dependencies

```bash
cd Week-2-Predictive-Maintenance
python -m venv .venv
# Windows: .venv\Scripts\activate
# Linux/macOS: source .venv/bin/activate
pip install -r requirements.txt
```

## Step 2 — Collect real sensor data

1. Install the ESP32 Arduino libraries listed above.
2. Flash `esp32/sensor_collector.ino`.
3. Identify the ESP32 serial port, for example `COM5` on Windows.
4. Capture telemetry:

```bash
python data/collect_serial.py COM5
```

On Linux/macOS, use a port such as `/dev/ttyUSB0` or `/dev/tty.usbserial-*`.

The collector writes `data/machine_data.csv` with:
- `timestamp_ms`
- `temperature_c`
- `vibration_rms_g`
- `vibration_peak_g`

## Step 3 — Train the anomaly detector

```bash
python ml/train_model.py
```

The training script uses the first 80% of the available records as baseline data, standardizes the features, trains an Isolation Forest, saves the scaler/model under `ml/artifacts/`, and writes `data/predictions.csv`.

The model uses:
- Temperature (°C)
- Vibration RMS (g)
- Vibration peak (g)

Isolation Forest is suitable for this educational prototype because it can learn mostly-normal behavior without requiring failure labels for every record.

## Step 4 — Run the dashboard

```bash
streamlit run dashboard/app.py
```

The dashboard shows:
- Latest temperature and vibration
- Detected anomaly count
- Current machine status
- Temperature and vibration graphs
- Recent anomaly records
- Configurable high-temperature threshold in code (`70 °C` by default)
- Manual email-alert button when an alert is active
- Refresh control for new CSV data

## Optional email alerts

Copy `.env.example` to your local environment configuration and set SMTP credentials. **Never commit real passwords or app passwords.**

For Gmail, use an App Password rather than your normal account password when SMTP authentication requires it.

Example environment variables:

```text
SMTP_HOST=smtp.gmail.com
SMTP_PORT=587
SMTP_USERNAME=your_email@gmail.com
SMTP_PASSWORD=your_app_password
ALERT_EMAIL=alert_recipient@example.com
```

The dashboard sends an email only when the user presses **Send email alert** while a maintenance alert is active. This avoids accidentally sending repeated emails on every Streamlit rerun.

## Demo mode

The repository includes `data/sample_machine_data.csv`, which contains synthetic telemetry with abnormal high-temperature/vibration periods. If `data/predictions.csv` does not exist, the dashboard automatically displays the sample data. Run the training script to generate predictions for the sample dataset.

## Important limitations

This is an educational predictive-maintenance prototype, not a certified failure-prediction system. Real deployment requires machine-specific sensor calibration, validated failure labels, appropriate sampling rates, feature engineering, threshold validation, model monitoring, drift handling, secure telemetry, and domain/maintenance review.

## Security

Do not commit Wi-Fi passwords, SMTP passwords, API keys, MQTT credentials, private endpoints, generated model artifacts, or production device secrets. Use environment variables or local secret files instead.
