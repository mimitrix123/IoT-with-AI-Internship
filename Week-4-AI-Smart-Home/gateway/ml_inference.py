import os
from pathlib import Path
import joblib
import pandas as pd
from dotenv import load_dotenv

ROOT = Path(__file__).resolve().parents[1]
load_dotenv(ROOT / ".env")
OCC_PATH = ROOT / os.getenv("OCCUPANCY_MODEL", "ml/artifacts/occupancy_model.joblib")
ENERGY_PATH = ROOT / os.getenv("ENERGY_MODEL", "ml/artifacts/energy_model.joblib")
FEATURES = ["temperature_c", "motion", "light_lux", "sound", "hour", "weekday"]


def load_models():
    return joblib.load(OCC_PATH), joblib.load(ENERGY_PATH)


def infer(row, occupancy_model, energy_model):
    ts = pd.to_datetime(row.get("timestamp"), unit="s", errors="coerce")
    if pd.isna(ts):
        ts = pd.Timestamp.now()
    features = pd.DataFrame([{
        "temperature_c": float(row.get("temperature_c", 0)),
        "motion": int(row.get("motion", 0)),
        "light_lux": float(row.get("light_lux", 0)),
        "sound": float(row.get("sound", 0)),
        "hour": int(ts.hour),
        "weekday": int(ts.weekday()),
    }])
    probability = float(occupancy_model.predict_proba(features)[0][1])
    energy = float(energy_model.predict(features)[0])
    actions = []
    if probability < 0.35:
        actions.append("reduce HVAC setpoint activity")
        actions.append("turn lights off when safe")
    elif float(row.get("light_lux", 0)) > 500:
        actions.append("use daylight and reduce artificial lighting")
    if energy > 0.75:
        actions.append("apply high-demand energy saving mode")
    recommendation = "; ".join(actions) if actions else "maintain current settings"
    return probability, energy, recommendation
