import os
from pathlib import Path
import requests


def notify_telegram(message, image_path=None):
    token = os.getenv("TELEGRAM_BOT_TOKEN", "").strip()
    chat_id = os.getenv("TELEGRAM_CHAT_ID", "").strip()
    if not token or not chat_id:
        return False, "Telegram is not configured"

    base = f"https://api.telegram.org/bot{token}"
    try:
        if image_path and Path(image_path).exists():
            with open(image_path, "rb") as photo:
                response = requests.post(
                    f"{base}/sendPhoto",
                    data={"chat_id": chat_id, "caption": message},
                    files={"photo": photo},
                    timeout=15,
                )
        else:
            response = requests.post(
                f"{base}/sendMessage",
                data={"chat_id": chat_id, "text": message},
                timeout=15,
            )
        response.raise_for_status()
        return True, "sent"
    except requests.RequestException as exc:
        return False, str(exc)
