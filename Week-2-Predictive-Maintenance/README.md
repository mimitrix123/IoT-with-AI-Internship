# Mini Project — Week 2: Predictive Maintenance System

## Objective
Build an IoT + Machine Learning prototype that collects machine vibration and temperature data, learns normal operating behavior, detects anomalies, and displays maintenance alerts in a Python dashboard.

## Architecture

```text
Vibration Sensor + Temperature Sensor
                ↓
              ESP32
                ↓ CSV / MQTT-ready telemetry
                ↓
       Python preprocessing
                ↓
        Isolation Forest ML
                ↓
   Normal / Anomaly classification
                ↓
       Streamlit dashboard
                ↓
     Alert + maintenance status
```

## Hardware
- ESP32
- MPU6050 accelerometer/gyroscope (vibration proxy)
- DS18B20 temperature sensor (or replace with another temperature sensor)
- Breadboard and jumper wires

## Software
- Python 3.10+
- pandas, numpy, scikit-learn, streamlit
- Arduino IDE / PlatformIO for ESP32

## Project structure

```text
Week-2-Predictive-Maintenance/
├── README.md
├── data/
│   └── sample_machine_data.csv
├── esp32/
│   └── sensor_collector.ino
├── ml/
│   └── train_model.py
├── dashboard/
│   └── app.py
└── requirements.txt
```

## Workflow
1. Flash `esp32/sensor_collector.ino` to collect timestamp, temperature, vibration RMS and vibration peak.
2. Save telemetry to `data/machine_data.csv` (the example firmware prints CSV to Serial; it can be captured with a serial logger).
3. Run `python ml/train_model.py` to train an Isolation Forest model using normal-operation data.
4. Run `streamlit run dashboard/app.py`.
5. Load the generated/predicted CSV in the dashboard and inspect anomalies.

## ML approach
Isolation Forest is used because predictive-maintenance datasets often begin with mostly normal observations and relatively few labeled failures. Features are standardized before training. The model produces an anomaly score and labels observations as `NORMAL` or `ANOMALY`.

This is an educational prototype, not a certified failure-prediction system. A real deployment should use validated failure labels, machine-specific thresholds, sensor calibration, drift monitoring and domain review.

## Dashboard alerts
The dashboard displays:
- Temperature trend
- Vibration RMS trend
- Vibration peak trend
- Number and percentage of detected anomalies
- Latest machine status
- Recent anomaly records

The alert banner is raised when an anomaly is detected or when temperature exceeds the configured threshold.

## Example result
A normal operating period should remain mostly `NORMAL`. Synthetic abnormal rows in the sample dataset contain elevated temperature/vibration and are intended to demonstrate the anomaly pipeline.

## Installation

```bash
pip install -r requirements.txt
streamlit run dashboard/app.py
```

## Safety and security
Do not commit credentials, private MQTT endpoints, API keys or production device secrets. This project intentionally uses local CSV telemetry so it can be demonstrated without exposing credentials.
