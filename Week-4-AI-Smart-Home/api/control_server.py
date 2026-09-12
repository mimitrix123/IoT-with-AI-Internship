import os
import ssl
from flask import Flask, jsonify, request
import paho.mqtt.publish as publish
from dotenv import load_dotenv

load_dotenv()
app = Flask(__name__)
TOKEN = os.getenv("CONTROL_API_TOKEN", "")
HOST = os.getenv("MQTT_HOST", "127.0.0.1")
PORT = int(os.getenv("MQTT_PORT", "8883"))
USER = os.getenv("MQTT_USERNAME", "smart_home")
PASSWORD = os.getenv("MQTT_PASSWORD", "")
CA = os.getenv("MQTT_CA_CERT", "/etc/mosquitto/certs/ca.crt")
TOPIC = os.getenv("MQTT_CONTROL_TOPIC", "home/livingroom/control")

ALLOWED = {"light_on", "light_off", "fan_on", "fan_off"}

def authorized():
    return not TOKEN or request.headers.get("Authorization") == f"Bearer {TOKEN}"

def send(command):
    tls = {"ca_certs": CA}
    publish.single(TOPIC, command, hostname=HOST, port=PORT,
                  auth={"username": USER, "password": PASSWORD}, tls=tls)

@app.post("/command")
def command():
    if not authorized(): return jsonify(error="unauthorized"), 401
    data = request.get_json(silent=True) or {}
    cmd = data.get("command", "")
    if cmd not in ALLOWED: return jsonify(error="unsupported command"), 400
    send(cmd)
    return jsonify(ok=True, command=cmd)

@app.get("/health")
def health():
    return jsonify(status="ok")

if __name__ == "__main__":
    app.run(host="127.0.0.1", port=5000)
