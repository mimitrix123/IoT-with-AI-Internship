import json
import os
import sys
from pathlib import Path
import paho.mqtt.client as mqtt
from dotenv import load_dotenv

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
load_dotenv(ROOT / ".env")
from gateway.database import init_db, insert_telemetry
from gateway.ml_inference import load_models, infer
from gateway.notifier import notify

HOST = os.getenv("MQTT_HOST", "127.0.0.1")
PORT = int(os.getenv("MQTT_PORT", "8883"))
USER = os.getenv("MQTT_USERNAME", "smart_home")
PASSWORD = os.getenv("MQTT_PASSWORD", "")
CA_CERT = os.getenv("MQTT_CA_CERT", "/etc/mosquitto/certs/ca.crt")
TOPIC = os.getenv("MQTT_TOPIC", "home/livingroom/sensors")

occupancy_model, energy_model = load_models()

def on_connect(client, userdata, flags, reason_code, properties=None):
    print("MQTT connected:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        row = json.loads(msg.payload.decode())
        probability, energy, recommendation = infer(row, occupancy_model, energy_model)
        insert_telemetry(row, probability, energy, recommendation)
        if probability > 0.8 and energy > 0.8:
            notify(f"Smart Home alert: high occupancy and energy demand. Recommendation: {recommendation}")
        print(f"occupancy={probability:.2f} energy={energy:.2f} -> {recommendation}")
    except (ValueError, TypeError, KeyError) as exc:
        print("Invalid telemetry:", exc)

init_db()
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="raspberrypi-smart-home")
client.username_pw_set(USER, PASSWORD)
if Path(CA_CERT).exists():
    client.tls_set(ca_certs=CA_CERT)
else:
    raise FileNotFoundError(f"MQTT CA certificate not found: {CA_CERT}")
client.on_connect = on_connect
client.on_message = on_message
client.connect(HOST, PORT, 60)
client.loop_forever()
