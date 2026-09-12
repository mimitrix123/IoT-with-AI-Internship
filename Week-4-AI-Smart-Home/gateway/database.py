import os
import sqlite3
from pathlib import Path
from dotenv import load_dotenv

ROOT = Path(__file__).resolve().parents[1]
load_dotenv(ROOT / ".env")
DB_PATH = Path(os.getenv("DB_PATH", str(ROOT / "data" / "smart_home.db")))
DB_PATH.parent.mkdir(parents=True, exist_ok=True)

def init_db():
    with sqlite3.connect(DB_PATH) as con:
        con.execute("""CREATE TABLE IF NOT EXISTS telemetry (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp REAL, device_id TEXT, temperature_c REAL,
            motion INTEGER, light_lux REAL, sound REAL,
            occupied_probability REAL, energy_kwh REAL, recommendation TEXT
        )""")

def insert_telemetry(row, occupied_probability=None, energy_kwh=None, recommendation=None):
    with sqlite3.connect(DB_PATH) as con:
        con.execute("""INSERT INTO telemetry
            (timestamp, device_id, temperature_c, motion, light_lux, sound,
             occupied_probability, energy_kwh, recommendation)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)""",
            (row.get("timestamp"), row.get("device_id"), row.get("temperature_c"),
             row.get("motion"), row.get("light_lux"), row.get("sound"),
             occupied_probability, energy_kwh, recommendation))

def recent(limit=100):
    with sqlite3.connect(DB_PATH) as con:
        con.row_factory = sqlite3.Row
        return [dict(r) for r in con.execute(
            "SELECT * FROM telemetry ORDER BY timestamp DESC LIMIT ?", (limit,)
        ).fetchall()]
