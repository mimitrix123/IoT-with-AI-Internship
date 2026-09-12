# Mini Project — Week 3: TinyML Smart Doorbell

## Objective
Build a Raspberry Pi smart doorbell that captures visitor images, detects faces, classifies visitors as **KNOWN** or **UNKNOWN** with a lightweight ML model, saves snapshots, sends notifications, and logs events to a dashboard.

## Architecture

```text
Doorbell button / motion
          ↓
   Raspberry Pi Camera
          ↓
   Face detection (OpenCV)
          ↓
  Crop + resize face
          ↓
 Lightweight classifier
   (TinyML-style model)
       ↙         ↘
    KNOWN       UNKNOWN
      ↓            ↓
  log event    snapshot + alert
       \          /
        ↓        ↓
       SQLite event log
              ↓
       Streamlit dashboard
```

> **Note:** On a Raspberry Pi, this project uses a small CPU-friendly classifier rather than claiming that a full TensorFlow Lite model is mandatory. The supplied model uses HOG features + a linear classifier and can be replaced by a `.tflite` model later. This is a learning prototype, not a security/access-control system.

## Hardware
- Raspberry Pi 4/5 (Pi Zero 2 W can be used with reduced settings)
- Raspberry Pi Camera Module or compatible CSI/USB camera
- Push button for the doorbell
- 10 kΩ resistor if using an external pull-down (the example uses the Pi's internal pull-up)
- Optional LED/buzzer

## Software
- Raspberry Pi OS
- Python 3.10+
- OpenCV
- NumPy
- scikit-learn
- joblib
- Streamlit
- SQLite3
- Optional `RPi.GPIO` for physical button input

## Project structure

```text
Week-3-Smart-Doorbell/
├── README.md
├── requirements.txt
├── .env.example
├── data/
│   └── README.md
├── models/
│   └── README.md
├── src/
│   ├── config.py
│   ├── database.py
│   ├── face_detector.py
│   ├── classifier.py
│   ├── notifier.py
│   ├── smart_doorbell.py
│   └── train_classifier.py
└── dashboard/
    └── app.py
```

## 1. Install

```bash
cd Week-3-Smart-Doorbell
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
cp .env.example .env
```

Edit `.env` with your notification settings. Never commit `.env`.

## 2. Train the visitor classifier

Create training folders such as:

```text
data/training/
├── known/
│   ├── alice_01.jpg
│   └── alice_02.jpg
└── unknown/
    ├── visitor_01.jpg
    └── visitor_02.jpg
```

Then run:

```bash
python src/train_classifier.py
```

The script extracts a compact HOG representation from detected face crops and trains a linear SVM. For a real deployment, collect multiple images per authorized person under different lighting and poses and validate the false-accept/false-reject rate.

## 3. Run the smart doorbell

```bash
python src/smart_doorbell.py
```

The application:
1. Opens the Raspberry Pi camera.
2. Waits for a doorbell press or motion-style camera trigger.
3. Detects faces.
4. Runs the lightweight classifier.
5. Saves an event to SQLite.
6. Saves a snapshot for an unknown visitor.
7. Sends a notification when configured.

For headless testing, use the camera trigger mode configured in `src/config.py`.

## 4. Run the dashboard

```bash
streamlit run dashboard/app.py
```

The dashboard shows total visits, known/unknown counts, recent events, timestamps, confidence scores, and links to stored snapshots.

## Notification setup

The included notifier supports Telegram using a bot token and chat ID. Set:

```text
TELEGRAM_BOT_TOKEN=your_bot_token
TELEGRAM_CHAT_ID=your_chat_id
```

If notification credentials are not configured, the system continues logging locally and does not attempt an external request.

## TinyML model explanation

The classifier is deliberately small:

- Face crop is resized to a fixed grayscale image.
- HOG converts the image into a compact feature vector.
- A linear SVM performs binary `KNOWN` / `UNKNOWN` classification.
- `joblib` stores the trained pipeline locally.

This keeps inference lightweight enough for a Raspberry Pi. A production-grade implementation can replace this pipeline with a quantized TensorFlow Lite face-embedding/classification model.

## Privacy and security

This project processes biometric imagery. Use it only with appropriate consent and applicable laws/policies. Store snapshots securely, restrict dashboard access, minimize retention, and do not use the prototype as the sole mechanism for physical access control.

Never commit:
- `.env`
- camera credentials
- bot tokens
- private visitor images
- generated SQLite databases
- trained models containing sensitive personal data

## Troubleshooting

### Camera does not open
Check the camera with Raspberry Pi OS camera tools and verify permissions. For a USB webcam, change the camera index in configuration.

### Classifier says UNKNOWN for everyone
Collect more representative training images and rerun training. Check that faces are detected and that the confidence threshold is appropriate.

### Telegram notifications fail
Verify the bot token, chat ID, internet connection, and that the bot has been started from the destination account.

## Demo flow

```text
Press doorbell
    ↓
Capture image
    ↓
Detect face
    ↓
Classify visitor
   ↙       ↘
KNOWN    UNKNOWN
  ↓          ↓
Log       Log + snapshot + notification
   \        /
    Dashboard
```

## Future upgrades
- TensorFlow Lite / Edge Impulse quantized model
- Multiple authorized identities instead of binary known/unknown classification
- PIR sensor for lower-power triggering
- OLED status display
- MQTT event publishing
- encrypted remote dashboard
- automatic snapshot retention policy
