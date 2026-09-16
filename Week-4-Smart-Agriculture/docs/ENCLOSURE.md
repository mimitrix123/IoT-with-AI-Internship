# 3D-Printed Enclosure Plan

## Concept

A two-piece, gasketed electronics box with a removable lid. The ESP32/PCB sits on internal standoffs. Sensor and pump cables enter through downward-facing cable glands.

## Suggested dimensions

- Outer body: 120 x 90 x 45 mm
- Wall thickness: 2.5–3 mm
- Lid overlap: 8 mm
- PCB standoffs: M3, 6–8 mm above floor
- Mounting-hole spacing: adapt to final PCB; leave at least 5 mm edge clearance
- OLED window: sized to the exact display module after procurement

## Print recommendations

- PETG or ASA for outdoor prototypes; avoid PLA for hot/sun-exposed installations.
- 0.2 mm layer height, 4 perimeter walls, 30–40% infill.
- Print the lid flat and use an elastomer gasket or closed-cell foam tape around the perimeter.
- Add a small internal drip lip below cable entries.
- Keep the DHT22 sensing element outside the sealed electronics volume or behind a protected vent; do not expose the PCB to irrigation spray.

## Mechanical arrangement

```text
+----------------------------------------+
| OLED window                            |
|                                        |
|   [ ESP32 + PCB ]        [ cable ]     |
|      M3 standoffs             glands   |
|                                        |
|   sealed electronics compartment       |
+----------------------------------------+
          downward cable exit
```

## Field protection

The enclosure concept is weather-resistant but is **not certified IP-rated**. Validate water ingress, UV exposure, condensation, temperature rise, and mechanical strength before outdoor deployment. Keep the pump and high-current wiring physically separated from low-voltage electronics where practical.
