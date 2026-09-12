import sys
from pathlib import Path
import streamlit as st
import pandas as pd

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "src"))
from database import init_db, get_events  # noqa: E402

st.set_page_config(page_title="Smart Doorbell", page_icon="🔔", layout="wide")
st.title("🔔 TinyML Smart Doorbell")
st.caption("Local visitor event monitoring dashboard")

init_db()
events = pd.DataFrame(get_events(500))

if events.empty:
    st.info("No visitor events logged yet. Start the Raspberry Pi doorbell and press the button.")
    st.stop()

events["timestamp"] = pd.to_datetime(events["timestamp"], errors="coerce")
known = int((events["visitor_class"] == "KNOWN").sum())
unknown = int((events["visitor_class"] == "UNKNOWN").sum())

c1, c2, c3 = st.columns(3)
c1.metric("Total events", len(events))
c2.metric("Known visitors", known)
c3.metric("Unknown visitors", unknown)

st.subheader("Visitor distribution")
st.bar_chart(events["visitor_class"].value_counts())

st.subheader("Recent events")
show = events[["timestamp", "visitor_class", "confidence", "snapshot_path"]].copy()
show["confidence"] = show["confidence"].round(3)
st.dataframe(show, use_container_width=True, hide_index=True)

unknown_rows = events[events["visitor_class"] == "UNKNOWN"].head(6)
if not unknown_rows.empty:
    st.subheader("Recent unknown visitor snapshots")
    cols = st.columns(min(3, len(unknown_rows)))
    for col, (_, row) in zip(cols, unknown_rows.iterrows()):
        path = row.get("snapshot_path")
        if path and Path(path).exists():
            col.image(str(path), caption=str(row["timestamp"]), use_container_width=True)
        else:
            col.warning("Snapshot unavailable")

st.caption("For privacy, runtime snapshots and the SQLite database are excluded from Git.")
