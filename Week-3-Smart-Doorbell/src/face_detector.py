import cv2

CASCADE = cv2.data.haarcascades + "haarcascade_frontalface_default.xml"


def detect_faces(frame, detector=None):
    detector = detector or cv2.CascadeClassifier(CASCADE)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    return detector.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(60, 60))


def crop_largest_face(frame, faces):
    if len(faces) == 0:
        return None
    x, y, w, h = max(faces, key=lambda box: box[2] * box[3])
    return frame[y:y + h, x:x + w]
