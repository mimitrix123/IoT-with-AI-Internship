import sys
from pathlib import Path
import pandas as pd
import streamlit as st
from streamlit_autorefresh import st_autorefresh

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from gateway.database import recent, init_db

st.set_page_config(page_title="AI Smart Home", layout="wide")
st.title("🏠 AI-Powered Smart Home")
st.caption("ESP32 → MQTT/TLS → Raspberry Pi → AI → Dashboard")
st_autorefresh(interval=5000, key="refresh")
init_db()
rows = recent(200)
if not rows:
    st.info("Waiting for ESP32 telemetry. Start the MQTT gateway and sensor node.")
    st.stop()

df = pd.DataFrame(rows).sort_values("timestamp")
latest = df.iloc[-1]
occupied = float(latest.get("occupied_probability") or 0)
energy = float(latest.get("energy_kwh") or 0)

c1, c2, c3, c4 = st.columns(4)
c1.metric("Temperature", f"{latest.temperature_c:.1f} °C")
c2.metric("Occupancy", f"{occupied * 100:.0f}%")
c3.metric("Energy demand", f"{energy:.2f} kWh")
c4.metric("Motion", "Detected" if latest.motion else "Clear")

st.subheader("Sensor trends")
chart_df = df.set_index(pd.to_datetime(df.timestamp, unit="s"))[["temperature_c", "light_lux", "sound"]]
st.line_chart(chart_df)

st.subheader("AI recommendation")
st.success(latest.recommendation or "Maintain current settings")

st.subheader("Recent telemetry")
st.dataframe(df.tail(30), use_container_width=True)
