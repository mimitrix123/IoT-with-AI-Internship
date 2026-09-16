# 🎲 Mini Project — Week 1: Digital Dice

A simple embedded-C/Arduino digital dice using **7 LEDs**, a **push button**, and a **piezo buzzer**.

## Objective

Press the button to roll a virtual dice. The microcontroller generates a pseudo-random value from **1 to 6** and displays the corresponding dice face using seven LEDs arranged in a standard 3×3 dice pattern. The buzzer provides audible feedback after each roll.

## Hardware

- Arduino Uno (or compatible ATmega328P board)
- 7 × LEDs
- 7 × 220Ω resistors
- 1 × push button
- 1 × 10kΩ resistor if using an external pull-down; the provided code uses the Arduino internal pull-up instead
- 1 × piezo buzzer
- Breadboard and jumper wires

## LED Layout

```text
LED1     LED2     LED3
  ●        ●        ●

LED4     LED5     LED6
  ●        ●        ●

        LED7
         ●
```

The software maps these seven LEDs to the six standard dice faces. Corner LEDs are shared between the appropriate faces; the center LED is used for 1, 3 and 5.

## Pin Mapping

| Component | Arduino Pin |
|---|---:|
| Top-left LED | D2 |
| Top-center LED | D3 |
| Top-right LED | D4 |
| Middle-left LED | D5 |
| Center LED | D6 |
| Middle-right LED | D7 |
| Bottom-center LED | D8 |
| Push button | D9 |
| Piezo buzzer | D10 |

Connect each LED through a 220Ω resistor to its pin, with the LED cathode connected to GND. Connect the push button between D9 and GND. The code enables the internal pull-up resistor, so a pressed button reads LOW.

## How It Works

1. The board initializes the seven LED outputs, button input, and buzzer.
2. A small amount of analog noise from an unused analog input is used as a seed for `randomSeed()`.
3. A button press is detected with debounce handling.
4. `random(1, 7)` generates a dice value from 1 through 6.
5. The matching LED pattern is displayed.
6. The buzzer produces a short confirmation tone.
7. The program waits until the button is released before accepting another roll.

## Source Code

The implementation is in [`src/digital_dice.ino`](src/digital_dice.ino).

## Build / Upload

1. Open `src/digital_dice.ino` in Arduino IDE.
2. Select **Arduino Uno** (or your compatible board).
3. Select the correct serial port.
4. Compile and upload.
5. Press the push button to roll the dice.

## Learning Outcomes

- Digital GPIO input/output
- LED pattern control
- Push-button debouncing
- Pseudo-random number generation in embedded C/C++
- Buzzer/tone output
- Basic embedded state handling

## Expected Result

Each button press produces a value from **1–6**, displays the corresponding dice face on the seven LEDs, and plays a short buzzer tone.

## Safety / Hardware Note

Always use a current-limiting resistor with each LED. Do not drive LEDs directly from a GPIO pin without appropriate current limiting.
