from pathlib import Path
import cv2
import joblib
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC
from face_detector import detect_faces, crop_largest_face
from classifier import extract_features

ROOT = Path(__file__).resolve().parents[1]
TRAIN_DIR = ROOT / "data" / "training"
MODEL_PATH = ROOT / "models" / "visitor_classifier.joblib"


def load_images(folder, label):
    X, y = [], []
    detector = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")
    for path in sorted(folder.glob("*")):
        image = cv2.imread(str(path))
        if image is None:
            continue
        face = crop_largest_face(image, detect_faces(image, detector))
        if face is None:
            continue
        X.append(extract_features(face)[0])
        y.append(label)
    return X, y


def main():
    known_x, known_y = load_images(TRAIN_DIR / "known", "KNOWN")
    unknown_x, unknown_y = load_images(TRAIN_DIR / "unknown", "UNKNOWN")
    X, y = known_x + unknown_x, known_y + unknown_y
    if len(set(y)) < 2 or len(X) < 10:
        raise SystemExit("Need at least 10 usable face images across both known and unknown classes.")

    model = Pipeline([
        ("scale", StandardScaler()),
        ("svm", SVC(kernel="linear", probability=True, class_weight="balanced", random_state=42)),
    ])
    model.fit(X, y)
    MODEL_PATH.parent.mkdir(parents=True, exist_ok=True)
    joblib.dump(model, MODEL_PATH)
    print(f"Saved model to {MODEL_PATH} using {len(X)} face samples.")


if __name__ == "__main__":
    main()
