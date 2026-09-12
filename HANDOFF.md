# Session handoff

This file is the starting point for a new ChatGPT session. Read this file first, then inspect the current `poc/74hc595` branch before making recommendations or changes.

## Scope

This repository is the active PoC for using AE-HY0020-DIP / FDK HY0020 (nRF52832) with ZMK and a 74HC595 matrix-column expander.

Do **not** assume this task is about the separate Tomkey repository. Do not bring in OLED, trackball, or Tomkey-specific requirements unless the user explicitly asks for them.

The current hardware goal is to define a reusable HY0020 + 74HC595 + power + battery-sense block that can later be reused for a 30-key board, a 43-key board, and a NumPad. The NumPad physical key layout is intentionally undecided.

## Manufacturing constraints

- Prefer JLCPCB Economic PCBA for the motherboard.
- AE-HY0020-DIP is installed manually after PCBA.
- Procurement simplicity and robustness are more important than minimizing module footprint.
- Do not expose the HY0020 module itself to JLCPCB Economic reflow.

## MCU / module

AE-HY0020-DIP contains FDK HY0020 / nRF52832.

Important resource limits:

- Flash: 512 KiB total
- RAM: 64 KiB

Current Flash partitioning:

- application code: 480 KiB, `0x00000000` through `0x00077fff`
- settings/NVS: 32 KiB, `0x00078000` through `0x0007ffff`

`CONFIG_USE_DT_CODE_PARTITION=y` is enabled so the linker actually respects the 480 KiB application partition. Earlier builds accidentally reported against the full 512 KiB even though the DTS described a 32 KiB NVS area. The binaries were small enough that this caused no overlap, but the partition enforcement is now fixed and verified.

## 74HC595 matrix design

Use one 74HC595 for matrix **column outputs**. Matrix row inputs remain directly connected to HY0020 GPIOs so ZMK wake/interrupt behavior is preserved.

Verified common control pins:

- `P0.07` = DATA / SPI MOSI: sends the column-output bit pattern into the 74HC595
- `P0.08` = CLOCK / SPI SCK: clocks each data bit into the shift register
- `P0.12` = LATCH / SPI CS: transfers the shifted pattern to Q0-Q7

74HC595 fixed wiring:

- VCC -> 3V3
- GND -> GND
- `/OE` -> GND
- `/SRCLR` / `/MR` -> 3V3
- 100 nF local decoupling at the IC
- Q0-Q7 -> matrix column nets
- Q7' unused unless a second shift register is later cascaded

The PoC uses ZMK `zmk,gpio-595`, and CI has confirmed that `gpio_595.c` and the SPI driver are actually compiled and linked.

## Common power block

AE-HY0020-DIP `3V3` is effectively the HY0020 VDD rail; a fully charged 1-cell LiPo must **not** be connected directly because HY0020 is specified for 1.7-3.6 V.

Current common topology:

`1S LiPo -> VBAT -> physical SPST power switch -> VBAT_SW -> TPS7A0233PDBVR -> 3V3`

3V3 powers:

- AE-HY0020-DIP
- 74HC595
- ordinary low-power 3.3 V peripherals

Preferred regulator:

- TI `TPS7A0233PDBVR`
- JLCPCB `C2887324`
- fixed 3.3 V
- 200 mA
- SOT-23-5
- active output discharge version

Pin connections:

- pin 1 IN -> `VBAT_SW`
- pin 2 GND -> GND
- pin 3 EN -> `VBAT_SW`
- pin 4 NC -> no connection
- pin 5 OUT -> `3V3`

Use 1 uF ceramic from IN to GND and 1 uF ceramic from OUT to GND, both close to the regulator.

The physical switch is intentionally before both the LDO and battery divider so OFF disconnects the electronics and prevents ADC back-powering.

See `docs/power-hardware.md` for the detailed rationale.

## Battery voltage sensing

Reserve `P0.30 / AIN6` for battery measurement.

Divider:

- `VBAT_SW -> 1 MOhm -> VBAT_SENSE -> 1 MOhm -> GND`
- `VBAT_SENSE -> P0.30 / AIN6`
- preferred resistor: JLCPCB `C26083`, 1 MOhm, 1%, 0402, used twice

At 4.2 V battery voltage, the ADC node is about 2.1 V and divider current is about 2.1 uA while powered on.

The 1 MOhm + 1 MOhm divider has 500 kOhm Thevenin source resistance. ZMK v0.3's battery-voltage-divider path uses the nRF52 SAADC with a 40 us acquisition time in this configuration. Keep this as the current baseline, but verify real-board accuracy against a multimeter before production freeze.

The PoC has confirmed that both `battery_voltage_divider.c` and the nRF52 SAADC driver are actually compiled and linked.

## Verified memory results

Current feature set: 74HC595 + battery ADC, with the 480 KiB code partition enforced.

| Build | Flash | RAM |
| --- | ---: | ---: |
| HY0020 left, Central + ZMK Studio | 56,308 B / 480 KiB (11.46%) | 16,686 B / 64 KiB (25.46%) |
| HY0020 right, Peripheral | 31,496 B / 480 KiB (6.41%) | 6,776 B / 64 KiB (10.34%) |

All four memory-workflow targets passed:

- HY0020 left
- HY0020 right
- nRF52832 DK left reference
- nRF52832 DK right reference

The heavy Central + Studio build still leaves about 47.7 KiB of link-time RAM unused. Memory capacity is therefore not currently a reason to reject HY0020.

The linker summary does not measure worst-case runtime stack high-water marks. Repeat the memory test when the final product feature set is known.

## NumPad status

The NumPad physical layout is **not decided**. Do not invent a final row/column arrangement yet.

It is okay to freeze these layout-independent blocks now:

- AE-HY0020-DIP
- 74HC595 and its three control signals
- 3.3 V regulator and power switch topology
- P0.30 battery ADC
- SWD access
- reset / DFU reservations

Leave these open until the layout is known:

- row count
- number of used Q outputs
- matrix transform
- encoder
- RGB
- any display
- keymap

See `docs/numpad-hardware.md`.

## Pins that should remain conservative

- `P0.20`: keep reserved for now because AE-HY0020-DIP ships with the Adafruit nRF52 bootloader / DFU-related usage. Reclaim only after the intended firmware-update method is deliberately decided.
- `P0.21`: keep for reset / recoverability.
- `P0.30`: battery ADC.
- `P0.09` / `P0.10`: NFC-capable pins; the board DTS already supports using them as GPIO via `nfct-pins-as-gpios`, but do not assign them to a peripheral without an actual need.

## Current source of truth

Read these files before changing the design:

- `README.md` - current decisions and measured resource usage
- `docs/power-hardware.md` - common power and battery block
- `docs/numpad-hardware.md` - layout-independent NumPad block
- `boards/arm/hy0020/` - HY0020 board definition and Flash partition behavior
- `boards/shields/hy73/` - current 74HC595 matrix PoC
- `.github/workflows/` - build and memory validation

The active development branch is `poc/74hc595`. Do not assume `main` contains the latest design state.

## Next engineering task

The next useful output is a **schematic-entry connection table** for the common hardware block, detailed enough to copy into KiCad or EasyEDA.

For each part, include:

- reference / function
- manufacturer part number and JLCPCB part where fixed
- physical IC pin number
- pin name
- net name
- connected destination
- component value where applicable
- whether the decision is fixed, provisional, or still requires hardware verification

Include at minimum:

- AE-HY0020-DIP
- 74HC595
- TPS7A0233PDBVR
- LDO input/output capacitors
- 74HC595 decoupling capacitor
- 1 MOhm / 1 MOhm battery divider
- battery connector boundary
- physical power-switch boundary
- SWD / reset / DFU nets

Do not force the NumPad key matrix layout yet.

## Working style for future sessions

- Inspect this repository and branch before asking the user questions that the repo can answer.
- Explain signal purpose in concrete electrical terms, not abbreviations only.
- Separate verified facts from provisional recommendations.
- Do not introduce Tomkey/OLED/trackball assumptions into this repository unless the user explicitly asks.
- When writing code, use 4-space indentation.
