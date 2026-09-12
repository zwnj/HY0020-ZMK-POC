# Validation status

This document records what has actually been verified in the HY0020 ZMK PoC and separates those results from assumptions that still require hardware testing.

## Current verdict

HY0020 / nRF52832 is currently considered viable for this design.

The original concern was not a proven out-of-memory failure. It was uncertainty about whether 64 KiB RAM and 512 KiB Flash would leave enough practical headroom for ZMK split operation, ZMK Studio, settings storage, matrix expansion, and battery sensing.

Current CI measurements show comfortable static memory margin.

## Verified build configuration

The current measured feature set includes:

- HY0020 / nRF52832 custom board
- ZMK split central / peripheral targets
- ZMK Studio on the central target
- one `zmk,gpio-595` 74HC595 matrix-output expander
- SPI driver used by the 74HC595
- `zmk,battery-voltage-divider`
- nRF52 SAADC
- 480 KiB application code partition
- 32 KiB settings / NVS partition

The final validation run confirmed that `gpio_595.c` and `battery_voltage_divider.c` are actually compiled and linked.

## Flash partition validation

HY0020 has 512 KiB total internal Flash.

The PoC intentionally uses:

- `0x00000000` - `0x00077fff`: 480 KiB application code
- `0x00078000` - `0x0007ffff`: 32 KiB settings / NVS

`CONFIG_USE_DT_CODE_PARTITION=y` is enabled in the HY0020 board definition. The linker therefore reports a 480 KiB Flash region for application code rather than treating the full 512 KiB device as available to the application.

Earlier PoC builds omitted this Kconfig option. Their binaries were small enough that no overlap occurred, but their percentages were calculated against the full 512 KiB and therefore understated use of the intended application partition.

## Memory measurements

Flash percentages below are normalized against the actual 480 KiB application partition.

| Build | Flash | RAM |
| --- | ---: | ---: |
| HY0020 left, Central + ZMK Studio, baseline | 50,076 B / 480 KiB (10.19%) | 16,230 B / 64 KiB (24.77%) |
| HY0020 left, Central + ZMK Studio, + 74HC595 | 54,072 B / 480 KiB (11.00%) | 16,438 B / 64 KiB (25.08%) |
| HY0020 left, Central + ZMK Studio, + 74HC595 + battery ADC | 56,308 B / 480 KiB (11.46%) | 16,686 B / 64 KiB (25.46%) |
| HY0020 right, Peripheral, baseline | 24,880 B / 480 KiB (5.06%) | 6,352 B / 64 KiB (9.69%) |
| HY0020 right, Peripheral, + 74HC595 | 28,748 B / 480 KiB (5.85%) | 6,536 B / 64 KiB (9.97%) |
| HY0020 right, Peripheral, + 74HC595 + battery ADC | 31,496 B / 480 KiB (6.41%) | 6,776 B / 64 KiB (10.34%) |

### Feature deltas

74HC595 cost relative to baseline:

- left: +3,996 B Flash, +208 B RAM
- right: +3,868 B Flash, +184 B RAM

Battery-voltage sensing cost relative to the 74HC595-only build:

- left: +2,236 B Flash, +248 B RAM
- right: +2,748 B Flash, +240 B RAM

Total increase from baseline to the current 74HC595 + battery-ADC build:

- left: +6,232 B Flash, +456 B RAM
- right: +6,616 B Flash, +424 B RAM

The heaviest current build uses 25.46% of RAM and leaves 48,850 B, about 47.7 KiB, unused at link time.

## CI targets

All four targets in the memory workflow passed in the final validation run:

- HY0020 left
- HY0020 right
- nRF52832 DK left reference
- nRF52832 DK right reference

The reference targets are useful for distinguishing HY0020 board-definition failures from general shield / ZMK configuration failures.

## What the current result proves

The current measurements support these conclusions:

- 64 KiB RAM is not currently a blocker.
- 480 KiB application Flash is not currently a blocker.
- reserving 32 KiB for settings / NVS still leaves large application margin.
- the 74HC595 has a small RAM cost.
- battery-voltage sensing with SAADC also has a small RAM cost.
- HY0020 should not be rejected on memory-capacity grounds based on the current feature set.

## What it does not prove

The linker summary does not measure worst-case runtime stack high-water marks.

The current CI also does not replace physical validation of:

- battery ADC accuracy against a multimeter
- power-off discharge behavior
- RF / antenna performance in the final PCB
- regulator transient behavior with final peripherals
- actual sleep current
- final product feature-complete RAM / Flash use

Repeat the memory workflow when the final product feature set is known, especially after adding large or optional features such as RGB, additional peripherals, complex keymap behaviors, or a different update / bootloader strategy.

## ZMK version status

The stable ZMK v0.3-based configuration used by this PoC builds successfully for the HY0020 and nRF52832 reference targets.

Testing against current ZMK `main` exposed a separate HY0020 board-definition migration issue related to newer Zephyr hardware-model conventions. That issue is independent of the 74HC595 and memory-capacity results recorded here.
