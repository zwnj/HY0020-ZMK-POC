# HY0020 ZMK PoC

Proof-of-concept repository for validating AE-HY0020-DIP / FDK HY0020 (nRF52832) as a practical ZMK wireless-keyboard controller.

The current PoC validates three things together:

- one 74HC595 for matrix column outputs
- a reusable 1-cell LiPo -> 3.3 V power block
- battery-voltage sensing with the nRF52 SAADC

The active development branch is **`poc/74hc595`**. Do not assume `main` contains the latest design state.

## Current verdict

HY0020 is currently a viable design choice.

The heaviest measured build, Central + ZMK Studio + 74HC595 + battery ADC, uses:

- Flash: **56,308 B / 480 KiB (11.46%)**
- RAM: **16,686 B / 64 KiB (25.46%)**

The final 32 KiB of the 512 KiB internal Flash is reserved for settings / NVS, and `CONFIG_USE_DT_CODE_PARTITION=y` is enabled so the linker actually protects that region.

See [`docs/validation.md`](docs/validation.md) for the full measurement history and validation limits.

## Documentation map

| Document | Purpose |
| --- | --- |
| [`HANDOFF.md`](HANDOFF.md) | short entry point for a new ChatGPT / engineering session |
| [`docs/common-hardware.md`](docs/common-hardware.md) | source of truth for HY0020 GPIO roles, 74HC595 wiring, shared logic block, and fixed/open decisions |
| [`docs/power-hardware.md`](docs/power-hardware.md) | source of truth for LiPo, power switch, 3.3 V regulator, battery divider, and power-validation requirements |
| [`docs/validation.md`](docs/validation.md) | source of truth for CI results, Flash/RAM measurements, partition validation, and remaining test limits |
| [`docs/numpad-hardware.md`](docs/numpad-hardware.md) | NumPad-specific constraints that remain valid before the physical layout is decided |

Firmware and board definitions live under:

- `boards/arm/hy0020/` - HY0020 board definition and Flash partitioning
- `boards/shields/hy73/` - current split / 74HC595 PoC
- `.github/workflows/` - build and memory-validation workflows

## Fixed common architecture

The current common hardware direction is:

- AE-HY0020-DIP installed manually after JLCPCB Economic PCBA
- one Nexperia `74HC595D,118` (`C5947`) for matrix column outputs
- matrix rows remain direct HY0020 GPIO inputs
- `P0.07` = 74HC595 DATA
- `P0.08` = 74HC595 CLOCK
- `P0.12` = 74HC595 LATCH / CS
- `P0.30 / AIN6` = battery-voltage ADC
- physical power switch before the regulator and divider
- TI `TPS7A0233PDBVR` (`C2887324`) as the current 3.3 V LDO choice
- 480 KiB application Flash + 32 KiB settings / NVS

Details and exact 74HC595 pin numbers are in [`docs/common-hardware.md`](docs/common-hardware.md).

## PoC matrix targets

The current validation shield uses asymmetric targets:

- left: 5 rows x 6 shifted columns
- right: 6 rows x 8 shifted columns

These are PoC validation shapes. They do **not** define the future NumPad layout.

## Manufacturing direction

- Prefer JLCPCB Economic PCBA for the motherboard.
- Install AE-HY0020-DIP manually after assembly.
- Keep the radio module out of the Economic reflow process.
- Prefer procurement simplicity and robust assembly over minimizing module footprint.

## Next engineering task

Build a schematic-entry connection table for the common block, including physical pin numbers, net names, values, destinations, JLCPCB part numbers where fixed, and a status column for fixed / provisional / hardware-verification-required decisions.

Do not force the NumPad key layout yet.
