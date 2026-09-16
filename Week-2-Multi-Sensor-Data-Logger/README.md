# 📊 Mini Project — Week 2: Multi-Sensor Data Logger

A C/Arduino-based sensor logger that reads **temperature**, **distance**, and **light** levels, prints timestamped measurements over **UART/Serial**, and activates a buzzer when configurable thresholds are exceeded.

## Features

- LM35 analog temperature sensor (default implementation)
- HC-SR04 ultrasonic distance measurement
- LDR analog light measurement
- Millisecond timestamp using `millis()`
- CSV-style UART output for easy serial-terminal logging
- Independent temperature, distance, and light threshold alerts
- Buzzer alert pattern

## Hardware

- Arduino Uno / compatible ATmega328P board
- LM35 temperature sensor
- HC-SR04 ultrasonic sensor
- LDR + 10kΩ resistor voltage divider
- Piezo buzzer
- Breadboard and jumper wires

> DHT11 can also be used as the temperature sensor, but the supplied firmware uses LM35 so the project has no external library dependency.

## Wiring

| Component | Arduino Pin |
|---|---:|
| LM35 output | A0 |
| LDR divider output | A1 |
| HC-SR04 TRIG | D6 |
| HC-SR04 ECHO | D7 |
| Piezo buzzer | D8 |

Connect the LM35 and LDR grounds to GND and their supplies to 5V. For the LDR, create a voltage divider with a 10kΩ resistor and connect the divider midpoint to A1.

For HC-SR04, connect VCC to 5V, GND to GND, TRIG to D6, and ECHO to D7.

Connect the piezo buzzer between D8 and GND. For a larger buzzer or other load, use an appropriate driver circuit rather than directly loading the GPIO pin.

## Temperature Conversion

For a typical LM35, the output is approximately **10 mV per °C**. With the Arduino Uno's default 5 V ADC reference and 10-bit ADC:

`temperature_C = analog_value × 500.0 / 1023.0 / 10.0`

The exact reading depends on the sensor, ADC reference, wiring, and calibration.

## Thresholds

The example firmware uses:

- Temperature alert: `35.0 °C` or higher
- Distance alert: `20.0 cm` or lower
- Light alert: ADC value below `250`

Change these constants in the source to suit the application.

## UART Output

Open the Serial Monitor at **9600 baud**. Output is CSV-style:

```text
Timestamp_ms,Temperature_C,Distance_cm,Light_ADC,Alert
1000,27.40,48.32,712,NORMAL
2000,35.10,17.45,190,ALERT
```

This format can be captured by a serial terminal or redirected into a file for later analysis.

## How It Works

1. `setup()` initializes UART, GPIO, and the random-free sensor interface.
2. Every `LOG_INTERVAL_MS`, the program reads all three sensors.
3. `millis()` supplies the elapsed timestamp without blocking for the main logging interval.
4. The measurements are printed as one CSV record.
5. Threshold logic checks each sensor.
6. If any threshold is violated, the buzzer emits an alert pattern.

## Source Code

[`src/multi_sensor_logger.ino`](src/multi_sensor_logger.ino)

## Learning Outcomes

- Embedded C/C++ sensor interfacing
- ADC sensor reading and conversion
- Ultrasonic pulse timing
- UART serial communication
- Timestamped data logging
- Threshold-based event detection
- Basic embedded-system monitoring design

## Expected Result

The Serial Monitor continuously displays timestamped temperature, distance, and light readings. The buzzer sounds whenever at least one configured threshold is crossed.
