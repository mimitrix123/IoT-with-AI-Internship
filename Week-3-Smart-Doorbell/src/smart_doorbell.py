import time
from datetime import datetime
import cv2

from config import CAMERA_INDEX, DOORBELL_GPIO, UNKNOWN_THRESHOLD, SNAPSHOT_DIR
from database import init_db, log_event
from face_detector import detect_faces, crop_largest_face
from classifier import load_classifier, classify
from notifier import notify_telegram

try:
    import RPi.GPIO as GPIO
except ImportError:
    GPIO = None


def doorbell_pressed():
    if GPIO is None:
        return False
    return GPIO.input(DOORBELL_GPIO) == GPIO.LOW


def main():
    init_db()
    model = load_classifier()
    detector = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")
    camera = cv2.VideoCapture(CAMERA_INDEX)
    if not camera.isOpened():
        raise RuntimeError("Could not open camera")

    if GPIO is not None:
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(DOORBELL_GPIO, GPIO.IN, pull_up_down=GPIO.PUD_UP)

    last_event = 0.0
    print("Smart doorbell running. Press the physical button or Ctrl+C to stop.")
    try:
        while True:
            pressed = doorbell_pressed()
            if pressed and time.time() - last_event > 3:
                ok, frame = camera.read()
                if not ok:
                    print("Camera frame unavailable")
                    continue
                faces = detect_faces(frame, detector)
                face = crop_largest_face(frame, faces)
                now = datetime.now().strftime("%Y%m%d_%H%M%S")
                if face is None:
                    log_event("NO_FACE", 0.0)
                    last_event = time.time()
                    time.sleep(0.5)
                    continue

                label, confidence = classify(face, model)
                visitor_class = label if confidence >= UNKNOWN_THRESHOLD else "UNKNOWN"
                snapshot_path = None
                if visitor_class == "UNKNOWN":
                    snapshot_path = SNAPSHOT_DIR / f"unknown_{now}.jpg"
                    cv2.imwrite(str(snapshot_path), frame)

                log_event(visitor_class, confidence, str(snapshot_path) if snapshot_path else None)
                if visitor_class == "UNKNOWN":
                    notify_telegram(
                        f"Smart doorbell: UNKNOWN visitor detected at {now}. Confidence: {confidence:.2f}",
                        snapshot_path,
                    )
                print(f"Visitor: {visitor_class} confidence={confidence:.2f}")
                last_event = time.time()
            time.sleep(0.1)
    finally:
        camera.release()
        if GPIO is not None:
            GPIO.cleanup()


if __name__ == "__main__":
    main()
