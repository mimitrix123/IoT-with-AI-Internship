"""Optional SMTP email notifier for maintenance alerts."""
import os
import smtplib
from email.message import EmailMessage


def send_alert(subject: str, body: str) -> bool:
    host = os.getenv("SMTP_HOST")
    port = int(os.getenv("SMTP_PORT", "587"))
    username = os.getenv("SMTP_USERNAME")
    password = os.getenv("SMTP_PASSWORD")
    recipient = os.getenv("ALERT_EMAIL")

    if not all([host, username, password, recipient]):
        return False

    message = EmailMessage()
    message["From"] = username
    message["To"] = recipient
    message["Subject"] = subject
    message.set_content(body)

    with smtplib.SMTP(host, port, timeout=15) as server:
        server.starttls()
        server.login(username, password)
        server.send_message(message)
    return True
