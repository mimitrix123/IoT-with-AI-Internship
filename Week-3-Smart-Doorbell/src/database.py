import sqlite3
from datetime import datetime, timezone
from config import DATABASE_PATH


def init_db():
    with sqlite3.connect(DATABASE_PATH) as con:
        con.execute("""
            CREATE TABLE IF NOT EXISTS events (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp TEXT NOT NULL,
                visitor_class TEXT NOT NULL,
                confidence REAL NOT NULL,
                snapshot_path TEXT
            )
        """)
        con.commit()


def log_event(visitor_class, confidence, snapshot_path=None):
    timestamp = datetime.now(timezone.utc).isoformat()
    with sqlite3.connect(DATABASE_PATH) as con:
        con.execute(
            "INSERT INTO events(timestamp, visitor_class, confidence, snapshot_path) VALUES (?, ?, ?, ?)",
            (timestamp, visitor_class, float(confidence), snapshot_path),
        )
        con.commit()


def get_events(limit=100):
    with sqlite3.connect(DATABASE_PATH) as con:
        con.row_factory = sqlite3.Row
        rows = con.execute(
            "SELECT * FROM events ORDER BY id DESC LIMIT ?", (int(limit),)
        ).fetchall()
    return [dict(row) for row in rows]
