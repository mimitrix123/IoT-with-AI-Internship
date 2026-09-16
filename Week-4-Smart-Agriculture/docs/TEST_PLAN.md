# Test Plan

| Test | Procedure | Pass condition |
|---|---|---|
| Boot | Power from regulated 3.3 V rail | ESP32 starts without repeated reset |
| Soil ADC | Measure dry, medium, saturated samples | Calibrated percentage is monotonic |
| Light ADC | Cover/uncover LDR | Reading changes in expected direction |
| DHT22 | Compare with reference thermometer | Stable, plausible readings; invalid reads are rejected |
| OLED | Power and run status task | Status is visible and refreshes |
| Wi-Fi | Disable/re-enable AP | Device reconnects without manual reset |
| MQTT | Subscribe to telemetry topic | Valid JSON arrives at expected interval |
| MQTT command | Publish PUMP_ON/PUMP_OFF/AUTO | Relay state follows command |
| Irrigation | Simulate low/high moisture | Hysteresis prevents rapid relay cycling |
| Pump timeout | Force low moisture for >20 s | Pump turns OFF automatically |
| OTA | Install signed test image from HTTPS | Firmware updates and boots new image |
| Deep sleep | Measure current after active window | Sleep current meets the chosen battery budget |
| Soak | Operate continuously for 24 h | No uncontrolled pump operation or repeated crash/reset |

## Acceptance notes

Record calibration constants, measured active/sleep current, Wi-Fi RSSI, MQTT latency, OTA result, and enclosure water-ingress observations in the final project report.
