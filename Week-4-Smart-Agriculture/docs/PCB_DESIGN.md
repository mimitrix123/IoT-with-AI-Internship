# PCB Design Plan

## Schematic blocks

```text
J1 Battery/DC IN -> Fuse/TVS -> 3.3 V regulator -> ESP32
                                      |
       +------------------------------+----------------+
       |               |              |                |
     ADC34           ADC35          GPIO4            I2C
       |               |              |             SDA/SCL
  Soil probe          LDR           DHT22              |
       |               |              |            SSD1306
       +---------------+--------------+----------------+
                                      |
                                  GPIO26
                                      |
                               Relay driver/module
                                      |
                              DC pump supply/load
```

## Layout guidance

1. Put the ESP32 antenna at the board edge with a keep-out beneath/in front of the antenna region.
2. Keep the switching relay/pump current loop away from ADC traces and the OLED/I2C lines.
3. Use a star-like power return so pump current does not share thin sensor ground traces.
4. Add bulk capacitance near the ESP32 regulator and relay/pump interface; size it from measured transient current.
5. Add input fuse and reverse-polarity protection on the battery/DC input.
6. Add test points for 3V3, GND, soil ADC, light ADC, UART TX/RX, and relay control.
7. Provide screw terminals/JST connectors with keyed polarity for field wiring.
8. Add mounting holes at four corners and silkscreen labels for every connector.

## BOM baseline

- ESP32-WROOM module or development-module footprint
- 3.3 V buck/LDO selected for the measured Wi-Fi peak current
- Capacitive soil probe connector
- DHT22 connector
- LDR + resistor divider
- SSD1306 4-pin connector
- Logic-level relay driver/module or MOSFET driver stage
- Fuse, reverse-polarity protection, TVS as appropriate
- Decoupling and bulk capacitors
- Battery/DC input connector

## Fabrication checklist

- Run ERC/DRC.
- Verify ADC input voltage never exceeds ESP32 limits.
- Verify relay coil current is not sourced directly from an ESP32 GPIO.
- Confirm creepage/clearance if any higher voltage is present; otherwise keep the prototype low-voltage.
- Export Gerbers, drill files, BOM, and pick-and-place only after a final schematic review.
