"""Central configuration helpers for the Week 4 gateway."""
import os
from pathlib import Path
from dotenv import load_dotenv

ROOT = Path(__file__).resolve().parents[1]
load_dotenv(ROOT / ".env")

MQTT_HOST = os.getenv("MQTT_HOST", "127.0.0.1")
MQTT_PORT = int(os.getenv("MQTT_PORT", "8883"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC", "home/livingroom/sensors")
MQTT_CONTROL_TOPIC = os.getenv("MQTT_CONTROL_TOPIC", "home/livingroom/control")
