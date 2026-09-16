"""Capture ESP32 CSV telemetry from a serial port into machine_data.csv."""
import argparse
from pathlib import Path
import serial

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUTPUT = ROOT / "data" / "machine_data.csv"

HEADER = "timestamp_ms,temperature_c,vibration_rms_g,vibration_peak_g"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Serial port, e.g. COM5 or /dev/ttyUSB0")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--output", default=str(DEFAULT_OUTPUT))
    args = parser.parse_args()

    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)

    with serial.Serial(args.port, args.baud, timeout=2) as ser, output.open("w", encoding="utf-8") as f:
        print(f"Listening on {args.port} at {args.baud} baud. Press Ctrl+C to stop.")
        while True:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            if not line or line == HEADER or line.startswith("ERROR,"):
                continue
            parts = line.split(",")
            if len(parts) != 4:
                continue
            try:
                int(parts[0]); [float(x) for x in parts[1:]]
            except ValueError:
                continue
            if f.tell() == 0:
                f.write(HEADER + "\n")
            f.write(line + "\n")
            f.flush()
            print(line)


if __name__ == "__main__":
    main()
