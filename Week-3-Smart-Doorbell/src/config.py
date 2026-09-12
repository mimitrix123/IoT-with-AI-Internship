import os
from pathlib import Path
from dotenv import load_dotenv

ROOT = Path(__file__).resolve().parents[1]
load_dotenv(ROOT / ".env")

CAMERA_INDEX = int(os.getenv("CAMERA_INDEX", "0"))
DOORBELL_GPIO = int(os.getenv("DOORBELL_GPIO", "17"))
UNKNOWN_THRESHOLD = float(os.getenv("UNKNOWN_THRESHOLD", "0.55"))
SNAPSHOT_DIR = ROOT / os.getenv("SNAPSHOT_DIR", "data/snapshots")
DATABASE_PATH = ROOT / os.getenv("DATABASE_PATH", "data/events.db")
MODEL_PATH = ROOT / "models" / "visitor_classifier.joblib"

SNAPSHOT_DIR.mkdir(parents=True, exist_ok=True)
DATABASE_PATH.parent.mkdir(parents=True, exist_ok=True)
