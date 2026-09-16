/*
 * Mini Project — Week 1: Digital Dice
 * IoT-with-C-Internship
 *
 * 7 LEDs + push button + piezo buzzer.
 * Arduino Uno / ATmega328P compatible.
 */

const byte LED_COUNT = 7;

// LED positions: top-left, top-center, top-right,
// middle-left, center, middle-right, bottom-center.
const byte ledPins[LED_COUNT] = {2, 3, 4, 5, 6, 7, 8};

const byte BUTTON_PIN = 9;
const byte BUZZER_PIN = 10;

// Dice faces. Each row contains the LEDs that should be ON.
// 1 = top-left, 2 = top-center, 3 = top-right,
// 4 = middle-left, 5 = center, 6 = middle-right, 7 = bottom-center.
const bool diceFaces[6][LED_COUNT] = {
  // 1: center
  {false, false, false, false, true,  false, false},

  // 2: top-left + bottom-right
  {true,  false, false, false, false, false, true},

  // 3: top-left + center + bottom-right
  {true,  false, false, false, true,  false, true},

  // 4: top-left + top-right + bottom-left + bottom-right
  {true,  false, true,  true,  false, true, true},

  // 5: four corners + center
  {true,  false, true,  true, true,  true, true},

  // 6: four corners + top-center + bottom-center
  {true,  true,  true,  true, false, true, true}
};

void clearLeds() {
  for (byte i = 0; i < LED_COUNT; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

void showFace(byte value) {
  if (value < 1 || value > 6) {
    clearLeds();
    return;
  }

  for (byte i = 0; i < LED_COUNT; i++) {
    digitalWrite(ledPins[i], diceFaces[value - 1][i] ? HIGH : LOW);
  }
}

void rollFeedback(byte value) {
  // Short roll sequence for visual/audible feedback.
  for (byte i = 0; i < 3; i++) {
    showFace(random(1, 7));
    tone(BUZZER_PIN, 700 + (i * 150), 70);
    delay(100);
  }

  showFace(value);
  tone(BUZZER_PIN, 1000, 120);
}

void setup() {
  for (byte i = 0; i < LED_COUNT; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  clearLeds();

  // Use analog noise as a simple seed source.
  randomSeed(analogRead(A0));
}

void loop() {
  static bool lastButtonState = HIGH;
  bool buttonState = digitalRead(BUTTON_PIN);

  // Button is active LOW because INPUT_PULLUP is enabled.
  if (lastButtonState == HIGH && buttonState == LOW) {
    delay(25); // debounce

    if (digitalRead(BUTTON_PIN) == LOW) {
      byte result = random(1, 7); // 1..6
      rollFeedback(result);

      // Wait for release to prevent repeated rolls while held.
      while (digitalRead(BUTTON_PIN) == LOW) {
        delay(5);
      }
      delay(25);
    }
  }

  lastButtonState = digitalRead(BUTTON_PIN);
}
