import os
import requests
from dotenv import load_dotenv

load_dotenv()
API = "http://127.0.0.1:5000/command"
TOKEN = os.getenv("CONTROL_API_TOKEN", "")
COMMANDS = {"turn on the light": "light_on", "turn off the light": "light_off", "turn on the fan": "fan_on", "turn off the fan": "fan_off"}


def send(command):
    headers = {"Authorization": f"Bearer {TOKEN}"} if TOKEN else {}
    response = requests.post(API, json={"command": command}, headers=headers, timeout=5)
    response.raise_for_status()


def main():
    try:
        import speech_recognition as sr
    except ImportError:
        sr = None
    if sr is None:
        print("SpeechRecognition is not installed; use the control API directly.")
        return
    recognizer = sr.Recognizer()
    with sr.Microphone() as source:
        print("Listening for smart-home commands. Say 'turn on the light', etc.")
        while True:
            try:
                text = recognizer.recognize_google(recognizer.listen(source, timeout=10, phrase_time_limit=5)).lower()
                if text in COMMANDS:
                    send(COMMANDS[text])
                    print("Executed:", text)
                else:
                    print("Unsupported command:", text)
            except (sr.WaitTimeoutError, sr.UnknownValueError):
                continue
            except KeyboardInterrupt:
                break

if __name__ == "__main__":
    main()
