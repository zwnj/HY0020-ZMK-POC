# HY0020 ZMK PoC

Proof-of-concept repository for validating HY0020 (nRF52832) as the MCU module for a ZMK split keyboard, including memory headroom, a 74HC595-based matrix-output expansion, and the common battery-sensing path.

## Why this PoC exists

HY0020 uses nRF52832, which provides 512 KiB Flash and 64 KiB RAM. Compared with nRF52840, the RAM in particular is much smaller, so the original concern was whether a practical ZMK configuration would still have enough margin when using:

- BLE split central/peripheral roles
- ZMK Studio on the central side
- ZMK/Zephyr settings storage
- battery-voltage sensing
- future features such as RGB
- a GPIO-expansion scheme for the key matrix

The purpose of this repository is to measure those constraints instead of estimating them from specifications alone.

## Flash layout

The HY0020 board definition uses:

- `0x00000000` - `0x00077fff`: code partition, 480 KiB
- `0x00078000` - `0x0007ffff`: settings/NVS storage, 32 KiB

`CONFIG_USE_DT_CODE_PARTITION=y` is enabled so the linker actually limits application code to the 480 KiB code partition. This protects the final 32 KiB from application growth and leaves it reserved for settings/NVS.

Earlier PoC builds reported percentages against the full 512 KiB device Flash because this Kconfig option was missing. The binaries were small enough that no actual overlap occurred, but those earlier percentages understated usage of the intended code partition. The table below normalizes all Flash percentages against the correct 480 KiB application budget.

The PoC assumes direct SWD flashing and does not reserve space for a bootloader.

## Current 74HC595 matrix design

A single 74HC595 is used on each side to move matrix column outputs off the HY0020 GPIOs.

Common control pins:

- `P0.07`: 74HC595 DATA / SPI MOSI
- `P0.08`: 74HC595 CLOCK / SPI SCK
- `P0.12`: 74HC595 LATCH / SPI CS

Matrix configuration:

- Left: 30 keys, 5 rows x 6 columns
- Right: 43 keys, electrically rearranged to 6 rows x 8 columns
- Matrix rows connect directly to HY0020 GPIOs so input-side wake/interrupt behavior is retained
- Matrix columns are driven by 74HC595 outputs

The right-side 6 x 8 arrangement is used because one 74HC595 provides eight outputs. The physical key layout remains independent of the electrical matrix through ZMK matrix transforms.

## Common power block

The common battery, power-switch, 3.3 V regulator, and battery-sense topology for the 30-key side, 43-key side, and future NumPad is documented in [`docs/power-hardware.md`](docs/power-hardware.md).

Key decisions:

- a 1-cell LiPo is not connected directly to AE-HY0020-DIP `3V3`
- `TPS7A0233PDBVR` is the preferred 3.3 V LDO
- the physical switch is placed between raw `VBAT` and `VBAT_SW`
- `P0.30 / AIN6` is reserved for battery measurement
- the baseline divider is 1 MΩ / 1 MΩ, using the same resistor part twice

## NumPad hardware block

The NumPad physical layout is intentionally still undecided. The layout-independent HY0020 + 74HC595 block is documented in [`docs/numpad-hardware.md`](docs/numpad-hardware.md). The control interface, common power block, and PCB placement policy can be fixed now while the final row/column usage, matrix transform, and keymap remain open.

## Measured memory usage

Measured with the repository's GitHub Actions memory workflow. Flash percentages below use the production-intent 480 KiB code partition, not the full 512 KiB device Flash.

| Build | Flash | RAM |
| --- | ---: | ---: |
| HY0020 left, Central + ZMK Studio, baseline | 50,076 B / 480 KiB (10.19%) | 16,230 B / 64 KiB (24.77%) |
| HY0020 left, Central + ZMK Studio, + 74HC595 | 54,072 B / 480 KiB (11.00%) | 16,438 B / 64 KiB (25.08%) |
| HY0020 left, Central + ZMK Studio, + 74HC595 + battery ADC | 56,308 B / 480 KiB (11.46%) | 16,686 B / 64 KiB (25.46%) |
| HY0020 right, Peripheral, baseline | 24,880 B / 480 KiB (5.06%) | 6,352 B / 64 KiB (9.69%) |
| HY0020 right, Peripheral, + 74HC595 | 28,748 B / 480 KiB (5.85%) | 6,536 B / 64 KiB (9.97%) |
| HY0020 right, Peripheral, + 74HC595 + battery ADC | 31,496 B / 480 KiB (6.41%) | 6,776 B / 64 KiB (10.34%) |

74HC595 cost in this configuration:

- Left: +3,996 B Flash, +208 B RAM
- Right: +3,868 B Flash, +184 B RAM

Battery-voltage sensing cost relative to the 74HC595-only build:

- Left: +2,236 B Flash, +248 B RAM
- Right: +2,748 B Flash, +240 B RAM

Total increase from the original baseline to the current 74HC595 + battery-ADC build:

- Left: +6,232 B Flash, +456 B RAM
- Right: +6,616 B Flash, +424 B RAM

The final validation run also confirmed that the HY0020 linker region is 480 KiB and that both `gpio_595.c` and `battery_voltage_divider.c` are actually compiled into the firmware. All four memory-workflow targets passed: both HY0020 builds and both nRF52832 DK reference builds.

## Interpretation

The original risk was not an observed out-of-memory failure. It was uncertainty about whether nRF52832's 64 KiB RAM and 512 KiB Flash would leave enough practical headroom for ZMK.

The measured result is substantially better than the conservative concern:

- The heaviest current build uses 25.46% of RAM.
- 48,850 B, about 47.7 KiB, of RAM remains in the Central + Studio build at link time.
- The heaviest current build uses only 11.46% of the actual 480 KiB application-code partition.
- The final 32 KiB of Flash is now genuinely protected for settings/NVS by the linker configuration.
- 74HC595 and battery-voltage sensing together add only 456 B of RAM on the heaviest side relative to the original baseline.

Therefore neither RAM nor Flash/storage capacity is currently a reason to reject HY0020.

The linker summary does not measure worst-case runtime stack high-water marks, so final hardware/firmware should still be tested on-device. However, the current static margin is large enough that HY0020 is considered a valid design choice.

## Design decision

Current direction:

- Use AE-HY0020-DIP as the wireless MCU module.
- Assemble the motherboard with JLCPCB Economic PCBA.
- Install AE-HY0020-DIP after PCBA so the radio module is not exposed to JLCPCB reflow.
- Use one 74HC595 per keyboard side for matrix column outputs.
- Keep matrix input rows directly connected to HY0020 GPIOs.
- Use the shared power/battery block described in `docs/power-hardware.md`.
- Reserve P0.30 / AIN6 for battery voltage measurement.
- Reserve the final 32 KiB of internal Flash for settings/NVS.
- Do not optimize around MCU-module footprint unless the product requirements change.

## Remaining validation

Before freezing the production firmware, repeat the memory measurement with every actually planned feature enabled. In particular, add optional features one at a time and compare their deltas, for example:

- RGB, if used
- final keymap behaviors, combos, macros, and Studio configuration
- any NumPad-specific features
- any charger/fuel-gauge feature added beyond the baseline voltage divider

The production gate should be based on the final feature-complete build, not only this baseline.

## ZMK version note

The stable ZMK v0.3-based build used by this PoC succeeds for the HY0020 targets and nRF52832 reference targets.

Testing against current ZMK `main` exposes a separate board-definition migration issue related to newer Zephyr hardware-model conventions. That issue is independent of 74HC595 memory usage and does not invalidate the current stable-build results.
