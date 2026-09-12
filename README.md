# HY0020 ZMK PoC

Proof-of-concept repository for validating HY0020 (nRF52832) as the MCU module for a ZMK split keyboard, including memory headroom and a 74HC595-based matrix-output expansion.

## Why this PoC exists

HY0020 uses nRF52832, which provides 512 KiB Flash and 64 KiB RAM. Compared with nRF52840, the RAM in particular is much smaller, so the original concern was whether a practical ZMK configuration would still have enough margin when using:

- BLE split central/peripheral roles
- ZMK Studio on the central side
- ZMK/Zephyr settings storage
- future features such as battery/fuel-gauge support or RGB
- a GPIO-expansion scheme for the key matrix

The purpose of this repository is to measure those constraints instead of estimating them from specifications alone.

## Flash layout

The HY0020 board definition currently uses:

- `0x00000000` - `0x00077fff`: code partition, 480 KiB
- `0x00078000` - `0x0007ffff`: settings/NVS storage, 32 KiB

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

## Measured memory usage

Measured with the repository's GitHub Actions memory workflow.

| Build | Flash | RAM |
| --- | ---: | ---: |
| HY0020 left, Central + ZMK Studio, before 74HC595 | 50,076 B / 512 KiB (9.55%) | 16,230 B / 64 KiB (24.77%) |
| HY0020 left, Central + ZMK Studio, with 74HC595 | 54,072 B / 512 KiB (10.31%) | 16,438 B / 64 KiB (25.08%) |
| HY0020 right, Peripheral, before 74HC595 | 24,880 B / 512 KiB (4.75%) | 6,352 B / 64 KiB (9.69%) |
| HY0020 right, Peripheral, with 74HC595 | 28,748 B / 512 KiB (5.48%) | 6,536 B / 64 KiB (9.97%) |

74HC595 cost in this configuration:

- Left: +3,996 B Flash, +208 B RAM
- Right: +3,868 B Flash, +184 B RAM

## Interpretation

The original risk was not an observed out-of-memory failure. It was uncertainty about whether nRF52832's 64 KiB RAM and 512 KiB Flash would leave enough practical headroom for ZMK.

The measured result is substantially better than the conservative concern:

- The heaviest current build uses about 25% of RAM.
- About 49 KiB of RAM remains in the Central + Studio build at link time.
- Flash usage remains close to 10% even after adding the 74HC595 driver.
- The 74HC595 itself has negligible RAM impact in this design.

Therefore memory capacity is not currently a reason to reject HY0020.

The linker summary does not measure worst-case runtime stack high-water marks, so final hardware/firmware should still be tested on-device. However, the current static margin is large enough that HY0020 is considered a valid design choice.

## Design decision

Current direction:

- Use HY0020-DIP as the wireless MCU module.
- Assemble the motherboard with JLCPCB Economic PCBA.
- Install HY0020-DIP after PCBA so the radio module is not exposed to JLCPCB reflow.
- Use one 74HC595 per keyboard side for matrix column outputs.
- Keep matrix input rows directly connected to HY0020 GPIOs.
- Do not optimize around MCU-module footprint unless the product requirements change.

## Remaining validation

Before freezing the production firmware, repeat the memory measurement with every actually planned feature enabled. In particular, add optional features one at a time and compare their deltas, for example:

- battery voltage / fuel-gauge support
- RGB, if used
- final keymap behaviors, combos, macros, and Studio configuration
- any NumPad-specific features

The production gate should be based on the final feature-complete build, not only this baseline.

## ZMK version note

The stable ZMK v0.3-based build used by this PoC succeeds for the HY0020 targets and nRF52832 reference targets.

Testing against current ZMK `main` exposes a separate board-definition migration issue related to newer Zephyr hardware-model conventions. That issue is independent of 74HC595 memory usage and does not invalidate the current stable-build results.
