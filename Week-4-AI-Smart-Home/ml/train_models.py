from pathlib import Path
import joblib
import pandas as pd
from sklearn.ensemble import RandomForestClassifier, RandomForestRegressor
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, mean_absolute_error

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "data" / "sample_sensor_data.csv"
ARTIFACTS = ROOT / "ml" / "artifacts"
ARTIFACTS.mkdir(parents=True, exist_ok=True)

FEATURES = ["temperature_c", "motion", "light_lux", "sound", "hour", "weekday"]
df = pd.read_csv(DATA)
X = df[FEATURES]
y_occ = df["occupied"]
y_energy = df["energy_kwh"]

X_train, X_test, y_train, y_test = train_test_split(X, y_occ, test_size=0.25, random_state=42, stratify=y_occ)
occ = RandomForestClassifier(n_estimators=200, random_state=42, class_weight="balanced")
occ.fit(X_train, y_train)
print(f"Occupancy accuracy: {accuracy_score(y_test, occ.predict(X_test)):.2f}")
joblib.dump(occ, ARTIFACTS / "occupancy_model.joblib")

X_train, X_test, y_train, y_test = train_test_split(X, y_energy, test_size=0.25, random_state=42)
energy = RandomForestRegressor(n_estimators=200, random_state=42)
energy.fit(X_train, y_train)
print(f"Energy MAE: {mean_absolute_error(y_test, energy.predict(X_test)):.3f} kWh")
joblib.dump(energy, ARTIFACTS / "energy_model.joblib")
print("Saved ML artifacts to", ARTIFACTS)
