# Common HY0020 + 74HC595 hardware

This document is the hardware source of truth for the layout-independent logic block shared by the current PoC targets and the future NumPad.

It intentionally does not define the final NumPad key layout.

## Decision status

| Item | Status | Current decision |
| --- | --- | --- |
| Wireless MCU module | Fixed | AE-HY0020-DIP / FDK HY0020 / nRF52832 |
| Matrix expansion | Fixed | one 74HC595 for column outputs |
| Matrix scan direction | Fixed | `col2row` |
| Row inputs | Fixed architecture | direct HY0020 GPIO inputs |
| 74HC595 DATA | Verified in CI | `P0.07` |
| 74HC595 CLOCK | Verified in CI | `P0.08` |
| 74HC595 LATCH / CS | Verified in CI | `P0.12` |
| Battery ADC | Verified in CI | `P0.30 / AIN6` |
| Reset | Reserved | `P0.21` |
| DFU-related pin | Reserved for now | `P0.20` |
| Final NumPad rows / columns | Open | decide after physical layout |

## Why the 74HC595 is on the column side

ZMK scans the matrix with `col2row` in this PoC. The 74HC595 therefore drives the matrix columns, while the rows remain direct MCU inputs.

This preserves direct GPIO input behavior for wake and interrupts. The shift register only replaces output GPIOs; it does not sit between the switches and the MCU input pins.

## HY0020 signal assignments

| HY0020 GPIO | Net / role | Electrical purpose | Status |
| --- | --- | --- | --- |
| `P0.02` | `ROW0` | reads switch state for row 0 | used by PoC |
| `P0.03` | `ROW1` | reads switch state for row 1 | used by PoC |
| `P0.04` | `ROW2` | reads switch state for row 2 | used by PoC |
| `P0.05` | `ROW3` | reads switch state for row 3 | used by PoC |
| `P0.06` | `ROW4` | reads switch state for row 4 | used by PoC |
| `P0.16` | `ROW5` candidate | sixth direct row when required | used by right PoC target; optional elsewhere |
| `P0.07` | `SR_DATA` | sends the serial bit pattern that determines the 74HC595 column outputs | verified |
| `P0.08` | `SR_CLK` | clocks each serial data bit into the 74HC595 | verified |
| `P0.12` | `SR_LATCH` | transfers the shifted pattern to Q0-Q7 | verified |
| `P0.30 / AIN6` | `VBAT_SENSE` | measures divided battery voltage with SAADC | verified |
| `P0.20` | `DFU_RSVD` | kept free until the intended update / bootloader strategy is finalized | reserved |
| `P0.21` | `RESET` | hardware reset / recovery | reserved |
| `P0.09`, `P0.10` | spare GPIO | NFC-capable pins; board DTS permits GPIO use | unassigned |

The PoC board definition already sets `nfct-pins-as-gpios`, so `P0.09` and `P0.10` can be used later if a real peripheral needs them.

## 74HC595 part and pin connections

Preferred part:

- Manufacturer: Nexperia
- MPN: `74HC595D,118`
- JLCPCB part: `C5947`
- Package: SOIC-16
- Supply: `3V3`

| Physical pin | 74HC595 name | Net | Connection / purpose |
| ---: | --- | --- | --- |
| 1 | Q1 | `COL1` | matrix column output 1 |
| 2 | Q2 | `COL2` | matrix column output 2 |
| 3 | Q3 | `COL3` | matrix column output 3 |
| 4 | Q4 | `COL4` | matrix column output 4 |
| 5 | Q5 | `COL5` | matrix column output 5 |
| 6 | Q6 | `COL6` | matrix column output 6 |
| 7 | Q7 | `COL7` | matrix column output 7 |
| 8 | GND | `GND` | ground |
| 9 | Q7S / Q7' | NC | serial cascade output; unused with one register |
| 10 | `/MR` / `/SRCLR` | `3V3` | keeps asynchronous clear inactive |
| 11 | SHCP / SRCLK | `SR_CLK` | from HY0020 `P0.08` |
| 12 | STCP / RCLK | `SR_LATCH` | from HY0020 `P0.12` |
| 13 | `/OE` | `GND` | keeps outputs enabled |
| 14 | DS / SER | `SR_DATA` | from HY0020 `P0.07` |
| 15 | Q0 | `COL0` | matrix column output 0 |
| 16 | VCC | `3V3` | logic supply |

Place a 100 nF ceramic capacitor directly between pins 16 and 8, close to the IC.

Unused Q outputs may remain unconnected. Q7S / Q7' remains unconnected unless a later design deliberately cascades another shift register.

## PoC matrix usage

The current PoC verifies two asymmetric matrix targets using the same control interface:

- left target: 5 direct rows x 6 shifted columns
- right target: 6 direct rows x 8 shifted columns

The left overlay uses Q0-Q5. The right overlay uses Q0-Q7 and adds `P0.16` as the sixth row.

These are validation targets, not a requirement that the future NumPad use either matrix shape.

## Power and battery block

Power regulation, the physical power-switch topology, and battery sensing are defined in [`power-hardware.md`](power-hardware.md).

Important shared nets:

- `VBAT`: raw 1-cell LiPo / charger battery node
- `VBAT_SW`: battery after the physical switch
- `3V3`: regulated rail for HY0020 and 74HC595
- `VBAT_SENSE`: divided battery voltage into `P0.30 / AIN6`

## Flash and settings storage

The HY0020 has 512 KiB internal Flash. This PoC uses:

- 480 KiB application code partition
- 32 KiB settings / NVS partition

`CONFIG_USE_DT_CODE_PARTITION=y` is enabled so the linker actually respects the 480 KiB application boundary.

See [`validation.md`](validation.md) for measured Flash and RAM usage.

## PCB placement rules

- Place AE-HY0020-DIP at a board edge with the antenna region kept clear.
- Install AE-HY0020-DIP manually after JLCPCB PCBA.
- Put the 74HC595 on the board-interior side of the module, outside the antenna keepout.
- Keep `SR_DATA`, `SR_CLK`, and `SR_LATCH` short and simple.
- Put the 100 nF 74HC595 bypass capacitor immediately beside the IC.
- Do not place the 74HC595 underneath the DIP carrier.
- Preserve access around DIP pads for hand soldering and rework.

## Still intentionally open

Do not infer or freeze these from this document:

- final NumPad key count or geometry
- final NumPad row / column count
- encoder
- display
- RGB / indicator LEDs
- charging IC and USB connector
- battery connector / physical battery size
- final matrix transform or keymap

Those choices should be layered on top of this common block later.
