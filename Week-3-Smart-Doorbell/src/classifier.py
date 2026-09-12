from pathlib import Path
import cv2
import joblib
import numpy as np

MODEL_PATH = Path(__file__).resolve().parents[1] / "models" / "visitor_classifier.joblib"


def _hog(gray):
    win = cv2.resize(gray, (64, 64))
    gx = cv2.Sobel(win, cv2.CV_32F, 1, 0, ksize=1)
    gy = cv2.Sobel(win, cv2.CV_32F, 0, 1, ksize=1)
    mag, ang = cv2.cartToPolar(gx, gy, angleInDegrees=True)
    bins = np.int32(ang // 20) % 9
    feat = []
    for y in range(0, 64, 8):
        for x in range(0, 64, 8):
            hist = np.zeros(9, dtype=np.float32)
            for by in range(8):
                for bx in range(8):
                    hist[bins[y + by, x + bx]] += mag[y + by, x + bx]
            feat.extend(hist / (np.linalg.norm(hist) + 1e-6))
    return np.asarray(feat, dtype=np.float32)


def extract_features(face):
    gray = cv2.cvtColor(face, cv2.COLOR_BGR2GRAY) if face.ndim == 3 else face
    return _hog(gray).reshape(1, -1)


def load_classifier(path=MODEL_PATH):
    if not path.exists():
        raise FileNotFoundError(f"Model not found: {path}. Run train_classifier.py first.")
    return joblib.load(path)


def classify(face, model):
    features = extract_features(face)
    label = str(model.predict(features)[0])
    if hasattr(model, "predict_proba"):
        confidence = float(np.max(model.predict_proba(features)))
    else:
        decision = float(model.decision_function(features)[0])
        confidence = 1.0 / (1.0 + np.exp(-abs(decision)))
    return label, confidence
