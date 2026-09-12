# NumPad hardware block

The NumPad key layout is intentionally not fixed yet. The HY0020 and 74HC595 hardware block can still be fixed now because the shift-register interface does not depend on the final physical key arrangement.

The common battery/regulator/switch/battery-sense circuit is defined separately in [`power-hardware.md`](power-hardware.md) and should also be reused by the NumPad.

## Fixed architecture

Use the same basic architecture as the keyboard PoC:

- HY0020-DIP as the wireless MCU module
- one 74HC595 for matrix output expansion
- matrix output columns driven by the 74HC595
- matrix input rows connected directly to HY0020 GPIOs
- `P0.30 / AIN6` reserved for the common battery-voltage divider
- common `VBAT -> switch -> 3.3 V LDO` power block
- HY0020-DIP installed manually after JLCPCB PCBA
- 74HC595, regulator, and passive parts assembled by JLCPCB Economic PCBA

Do not create the final ZMK matrix transform until the NumPad key layout is known.

## 74HC595 part

Preferred production part:

- Manufacturer: Nexperia
- MPN: `74HC595D,118`
- JLCPCB part: `C5947`
- Package: SOIC-16
- Logic family: 74HC
- Supply: 3.3 V in this design

Use the HC version, not HCT, because the board is a 3.3 V system.

Re-check JLCPCB stock/status when ordering; the design decision is based on the part identity and footprint, not a frozen stock count.

## HY0020 to 74HC595 connections

Keep these control pins identical to the 30-key and 43-key PoC:

| HY0020 | 74HC595 | Function |
| --- | --- | --- |
| `P0.07` | `DS` / pin 14 | serial data / SPI MOSI |
| `P0.08` | `SHCP` / pin 11 | shift clock / SPI SCK |
| `P0.12` | `STCP` / pin 12 | storage-register clock / latch / SPI CS |

Fixed support connections:

| 74HC595 | Connection | Purpose |
| --- | --- | --- |
| pin 16 `VCC` | 3.3 V | logic supply |
| pin 8 `GND` | GND | ground |
| pin 10 `/MR` | 3.3 V | keep shift-register clear inactive |
| pin 13 `/OE` | GND | keep outputs enabled |
| pin 9 `Q7S` | NC | no daisy chain in the baseline design |

Place one 100 nF ceramic decoupling capacitor directly beside the 74HC595 between pins 16 and 8.

## Matrix nets

The 74HC595 outputs should use layout-independent net names:

- `COL0` = Q0
- `COL1` = Q1
- `COL2` = Q2
- `COL3` = Q3
- `COL4` = Q4
- `COL5` = Q5
- `COL6` = Q6
- `COL7` = Q7

The final NumPad may use fewer than eight columns. Unused outputs can simply remain unconnected.

For row inputs, reserve the same primary HY0020 GPIO pool used by the current PoC:

- `ROW0` = P0.02
- `ROW1` = P0.03
- `ROW2` = P0.04
- `ROW3` = P0.05
- `ROW4` = P0.06
- optional `ROW5` = P0.16 if the final layout needs a sixth row

Five direct rows and eight shifted columns already provide electrical capacity for up to 40 matrix positions. The optional sixth row raises that to 48 positions. The physical NumPad layout does not need to resemble this electrical matrix; ZMK matrix transforms can map the final physical ordering later.

`P0.30` is not part of the row pool because it is reserved for battery voltage measurement across all products.

## PCB placement

Placement priority:

1. Put the HY0020-DIP at a board edge with its antenna end kept clear according to the module/carrier antenna requirements.
2. Put the 74HC595 on the board-interior side of the HY0020, outside the antenna area.
3. Keep the 74HC595 physically close to the HY0020 so `P0.07`, `P0.08`, and `P0.12` are short, simple traces.
4. Put the 100 nF capacitor immediately next to the 74HC595 supply pins.
5. Fan Q0-Q7 outward from the 74HC595 toward the switch matrix area.
6. Do not place the 74HC595 underneath the HY0020-DIP carrier. Keep it accessible for inspection/rework and leave room around the DIP through-hole pads for manual soldering.
7. Keep a continuous ground reference around the logic section while respecting the HY0020 antenna keepout.
8. Follow the common power-block placement rules in `power-hardware.md` for the regulator, switch, and battery divider.

There is no benefit to placing the 74HC595 in the center of the key field. The preferred placement is near the MCU; the column traces can then fan out to the matrix.

## What remains undecided

Do not freeze these items yet:

- physical key count and arrangement
- exact row/column count actually used
- encoder presence or location
- RGB or indicator LEDs
- USB/charging connector and charger IC, if onboard charging is wanted
- battery connector and physical battery size
- final matrix transform and keymap

Those decisions can be made later without changing the HY0020-to-74HC595 control interface or the common regulator/battery-sense topology.
