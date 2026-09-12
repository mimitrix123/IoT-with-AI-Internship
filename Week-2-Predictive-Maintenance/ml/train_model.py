from pathlib import Path
import pickle
import numpy as np
import pandas as pd
from sklearn.ensemble import IsolationForest
from sklearn.preprocessing import StandardScaler

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "data" / "sample_machine_data.csv"
MODEL_DIR = ROOT / "ml" / "artifacts"
MODEL_DIR.mkdir(exist_ok=True)

FEATURES = ["temperature_c", "vibration_rms_g", "vibration_peak_g"]

df = pd.read_csv(DATA)
X = df[FEATURES].replace([np.inf, -np.inf], np.nan).dropna()

# Train on the first 80% of the sample, representing baseline/normal operation.
train_end = max(10, int(len(X) * 0.80))
X_train = X.iloc[:train_end]

scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)

model = IsolationForest(
    n_estimators=200,
    contamination=0.10,
    random_state=42,
)
model.fit(X_train_scaled)

with open(MODEL_DIR / "scaler.pkl", "wb") as f:
    pickle.dump(scaler, f)
with open(MODEL_DIR / "isolation_forest.pkl", "wb") as f:
    pickle.dump(model, f)

# Predict the complete dataset and save results for the dashboard.
X_all = df[FEATURES].replace([np.inf, -np.inf], np.nan)
valid = X_all.notna().all(axis=1)
df["anomaly_score"] = np.nan
df["prediction"] = "UNKNOWN"
df.loc[valid, "anomaly_score"] = model.decision_function(scaler.transform(X_all.loc[valid]))
df.loc[valid, "prediction"] = np.where(
    model.predict(scaler.transform(X_all.loc[valid])) == -1,
    "ANOMALY",
    "NORMAL",
)

output = ROOT / "data" / "predictions.csv"
df.to_csv(output, index=False)

print(f"Training rows: {len(X_train)}")
print(f"Total rows predicted: {valid.sum()}")
print(f"Predictions saved to: {output}")
print(df["prediction"].value_counts(dropna=False))
