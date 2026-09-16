/*
 * Mini Project — Week 1: Digital Dice
 * IoT-with-C-Internship
 *
 * 7 LEDs + push button + piezo buzzer.
 * Arduino Uno / ATmega328P compatible.
 */

const byte LED_COUNT = 7;

// LED positions: top-left, top-right, middle-left, center,
// middle-right, bottom-left, bottom-right.
const byte ledPins[LED_COUNT] = {2, 3, 4, 5, 6, 7, 8};

const byte BUTTON_PIN = 9;
const byte BUZZER_PIN = 10;

// LED order:
// 0 = top-left, 1 = top-right, 2 = middle-left,
// 3 = center, 4 = middle-right, 5 = bottom-left, 6 = bottom-right.
const bool diceFaces[6][LED_COUNT] = {
  // 1: center
  {false, false, false, true,  false, false, false},

  // 2: top-left + bottom-right
  {true,  false, false, false, false, false, true},

  // 3: top-left + center + bottom-right
  {true,  false, false, true,  false, false, true},

  // 4: four corners
  {true,  true,  false, false, false, true, true},

  // 5: four corners + center
  {true,  true,  false, true,  false, true, true},

  // 6: four corners + both middle side LEDs
  {true,  true,  true, false, true,  true, true}
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
  // Short rolling animation with audible feedback.
  for (byte i = 0; i < 3; i++) {
    showFace(random(1, 7));
    tone(BUZZER_PIN, 700 + (i * 150), 70);
    delay(100);
  }

  // Display the final result and play a confirmation tone.
  showFace(value);
  tone(BUZZER_PIN, 1000, 120);
}

void setup() {
  for (byte i = 0; i < LED_COUNT; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // Button connects between D9 and GND.
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

  lastButtonState = buttonState;
}
