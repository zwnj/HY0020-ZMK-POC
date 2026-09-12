# NumPad hardware status

The NumPad physical key layout is intentionally undecided.

This document records only the NumPad-specific decisions that can be made before the layout exists. Shared HY0020 / 74HC595 wiring belongs in [`common-hardware.md`](common-hardware.md), and shared power / battery circuitry belongs in [`power-hardware.md`](power-hardware.md).

## Reuse the common block

The NumPad should reuse the same layout-independent hardware block as the current PoC unless a later requirement gives a concrete reason to diverge:

- AE-HY0020-DIP
- one 74HC595 for matrix column outputs
- direct HY0020 GPIO row inputs
- common 74HC595 DATA / CLOCK / LATCH pins
- common 3.3 V power block
- `P0.30 / AIN6` battery-voltage measurement
- SWD access
- reset / DFU reservations
- AE-HY0020-DIP installed manually after JLCPCB PCBA

Exact common signal and IC pin assignments are defined in [`common-hardware.md`](common-hardware.md).

## Capacity available before layout selection

One 74HC595 provides up to eight matrix column outputs.

The common row pool currently available to the PoC is:

- `ROW0` = `P0.02`
- `ROW1` = `P0.03`
- `ROW2` = `P0.04`
- `ROW3` = `P0.05`
- `ROW4` = `P0.06`
- optional `ROW5` = `P0.16`

Five rows x eight columns provides up to 40 electrical matrix positions. A sixth row raises that to 48 positions.

This is capacity, not a chosen NumPad matrix. The final physical layout can later be mapped to the electrical matrix with a ZMK matrix transform.

## What can be placed on the PCB now

Even without a key layout, it is reasonable to reserve placement for:

- AE-HY0020-DIP at a board edge with antenna clearance
- 74HC595 on the board-interior side of the HY0020
- local 74HC595 decoupling capacitor
- 3.3 V regulator and its capacitors
- battery-divider resistors near the MCU side
- battery connector boundary
- physical power-switch boundary
- SWD / reset access

Follow the detailed placement rules in `common-hardware.md` and `power-hardware.md` rather than duplicating them here.

## What remains open

Do **not** freeze these until the physical NumPad concept exists:

- key count
- key geometry
- exact row count
- exact number of used 74HC595 Q outputs
- diode / switch physical routing
- encoder presence and location
- RGB or indicator LEDs
- display
- USB / charging connector
- charger IC
- battery connector type and battery size
- final matrix transform
- keymap

Unused 74HC595 outputs may simply remain unconnected, so selecting fewer than eight columns later does not require changing the common control interface.

## Next NumPad-specific decision point

Return to this document when a physical layout candidate exists.

At that point determine:

1. required key positions
2. practical electrical row / column shape
3. which Q0-Q7 outputs are actually used
4. whether `P0.16` is needed as a sixth row
5. whether any extra peripherals need remaining GPIOs
6. the ZMK matrix transform and keymap

Until then, the common HY0020 + 74HC595 + power block can progress independently.
