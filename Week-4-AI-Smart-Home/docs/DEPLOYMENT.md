# Deployment Guide

## Raspberry Pi

Install the MQTT broker and Python tooling:

```bash
sudo apt update
sudo apt install -y mosquitto mosquitto-clients python3-venv
```

Create a virtual environment in the Week 4 directory and install `requirements.txt`.

## Mosquitto TLS

Configure Mosquitto with a TLS listener on port 8883. Use a private CA to issue a server certificate for the Raspberry Pi and, where required by the security model, client certificates for devices. Keep private keys outside the repository.

A production broker should also use:

- password authentication
- ACLs limiting each device to its own topics
- TLS certificate validation
- firewall rules allowing only trusted networks
- no anonymous access

The gateway deliberately refuses to start if its configured CA certificate is missing.

## Environment

Copy `.env.example` to `.env` and set MQTT credentials, CA path, API token, database path, and optional Telegram credentials.

## Start services

```bash
source .venv/bin/activate
python ml/train_models.py
python gateway/mqtt_gateway.py
```

In another terminal:

```bash
streamlit run dashboard/app.py
```

For control and voice features:

```bash
python api/control_server.py
python voice/voice_control.py
```

The control API binds to localhost by default. If you expose it beyond localhost, use HTTPS/reverse proxy, strong authentication, firewalling, and rate limiting.

## Validation checklist

1. Confirm ESP32 joins Wi-Fi.
2. Confirm MQTT TLS connection succeeds.
3. Use `mosquitto_sub` with TLS credentials to verify telemetry.
4. Confirm rows appear in SQLite.
5. Train and load both ML artifacts.
6. Confirm the dashboard refreshes every five seconds.
7. Test Telegram with a non-sensitive alert.
8. Test relay commands using low-voltage loads before any mains-connected hardware.
9. Check broker logs and rotate credentials/certificates according to an operational policy.
