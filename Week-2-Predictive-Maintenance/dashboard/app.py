from pathlib import Path
import time
import pandas as pd
import streamlit as st
from notifier import send_alert

ROOT = Path(__file__).resolve().parents[1]
DATA_DIR = ROOT / "data"
PREDICTIONS = DATA_DIR / "predictions.csv"

st.set_page_config(page_title="Predictive Maintenance", page_icon="⚙️", layout="wide")
st.title("⚙️ Predictive Maintenance Dashboard")
st.caption("ESP32 vibration + temperature telemetry with Isolation Forest anomaly detection")

source = PREDICTIONS if PREDICTIONS.exists() else DATA_DIR / "sample_machine_data.csv"
df = pd.read_csv(source)
required = {"timestamp_ms", "temperature_c", "vibration_rms_g", "vibration_peak_g"}
missing = required - set(df.columns)
if missing:
    st.error(f"Missing columns: {', '.join(sorted(missing))}")
    st.stop()

if "prediction" in df.columns:
    anomalies = int((df["prediction"] == "ANOMALY").sum())
    latest_status = str(df.iloc[-1]["prediction"])
else:
    anomalies = 0
    latest_status = "MODEL NOT RUN"

TEMP_LIMIT_C = 70.0
latest = df.iloc[-1]
latest_temp = float(latest["temperature_c"])
maintenance_alert = latest_status == "ANOMALY" or latest_temp >= TEMP_LIMIT_C

c1, c2, c3, c4 = st.columns(4)
c1.metric("Latest temperature", f"{latest_temp:.1f} °C")
c2.metric("Latest vibration RMS", f"{float(latest['vibration_rms_g']):.3f} g")
c3.metric("Detected anomalies", anomalies)
c4.metric("Latest status", latest_status)

if maintenance_alert:
    st.error("⚠️ Maintenance alert: abnormal machine behavior detected or temperature limit exceeded.")
    if st.button("Send email alert"):
        body = (
            f"Predictive maintenance alert\n\n"
            f"Temperature: {latest_temp:.1f} °C\n"
            f"Vibration RMS: {float(latest['vibration_rms_g']):.3f} g\n"
            f"Vibration peak: {float(latest['vibration_peak_g']):.3f} g\n"
            f"Status: {latest_status}\n"
        )
        try:
            sent = send_alert("Predictive Maintenance Alert", body)
            if sent:
                st.success("Email alert sent.")
            else:
                st.warning("Email is not configured. Set SMTP_* and ALERT_EMAIL environment variables.")
        except Exception as exc:
            st.error(f"Email delivery failed: {exc}")
else:
    st.success("✅ Machine status appears normal.")

st.subheader("Temperature")
st.line_chart(df.set_index("timestamp_ms")["temperature_c"])

st.subheader("Vibration")
st.line_chart(df.set_index("timestamp_ms")[["vibration_rms_g", "vibration_peak_g"]])

if "prediction" in df.columns:
    st.subheader("Recent anomalies")
    recent = df[df["prediction"] == "ANOMALY"].tail(20)
    if recent.empty:
        st.info("No anomalies detected in the current dataset.")
    else:
        st.dataframe(recent, use_container_width=True)

st.subheader("Data")
st.dataframe(df.tail(100), use_container_width=True)

if st.button("Refresh data"):
    time.sleep(0.2)
    st.rerun()
