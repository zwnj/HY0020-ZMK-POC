# Session handoff

Use this file as the entry point when continuing the project in a new ChatGPT or engineering session.

## Scope

This repository is the active PoC for AE-HY0020-DIP / FDK HY0020 (nRF52832) with ZMK, one 74HC595 matrix-column expander, the common 3.3 V power block, and battery-voltage sensing.

Do **not** assume this task is about the separate Tomkey repository. Do not introduce OLED, trackball, or Tomkey-specific requirements unless the user explicitly asks for them.

The active development branch is **`poc/74hc595`**. Read the current branch before making recommendations or changes.

## Read these first

The documentation is intentionally split by responsibility:

| Document | Source of truth for |
| --- | --- |
| [`README.md`](README.md) | project overview, current verdict, documentation map |
| [`docs/common-hardware.md`](docs/common-hardware.md) | HY0020 GPIO roles, exact 74HC595 wiring, shared logic architecture |
| [`docs/power-hardware.md`](docs/power-hardware.md) | LiPo, switch, LDO, battery divider, power validation |
| [`docs/validation.md`](docs/validation.md) | CI results, Flash/RAM measurements, Flash partition validation |
| [`docs/numpad-hardware.md`](docs/numpad-hardware.md) | NumPad-specific decisions that can be made before its layout is known |

Do not duplicate detailed design facts into this file when one of those documents already owns them.

## Current snapshot

The common architecture currently uses:

- AE-HY0020-DIP, manually installed after JLCPCB Economic PCBA
- one `74HC595D,118` (`C5947`) for matrix column outputs
- direct HY0020 GPIO inputs for matrix rows
- `P0.07` = 74HC595 DATA
- `P0.08` = 74HC595 CLOCK
- `P0.12` = 74HC595 LATCH / CS
- `P0.30 / AIN6` = battery ADC
- `P0.21` reserved for reset
- `P0.20` reserved for now because the update / bootloader strategy is not finalized
- TI `TPS7A0233PDBVR` (`C2887324`) as the current 3.3 V LDO choice
- 480 KiB application Flash + 32 KiB settings / NVS

The heaviest verified build currently uses:

- Flash: **56,308 B / 480 KiB (11.46%)**
- RAM: **16,686 B / 64 KiB (25.46%)**

Memory capacity is therefore not currently a reason to reject HY0020. See `docs/validation.md` before quoting or updating any measurement.

The NumPad physical layout is still intentionally undecided. Do not invent its final row/column arrangement.

## Next engineering task

Create a schematic-entry connection table for the common hardware block that can be copied into KiCad or EasyEDA.

For each item include:

- reference / function
- manufacturer part number and JLCPCB part where fixed
- physical pin number
- pin name
- net name
- connected destination
- value where applicable
- status: fixed, provisional, or hardware-verification-required

Cover at least AE-HY0020-DIP, 74HC595, TPS7A0233PDBVR, regulator capacitors, 74HC595 decoupling, battery divider, battery / switch boundaries, SWD, reset, and DFU-related nets.

Do not force the NumPad matrix layout yet.

## Working rules

- Inspect the repository before asking questions the repository can answer.
- Explain signal purpose in concrete electrical terms, not abbreviations only.
- Keep verified facts separate from provisional recommendations.
- Update the document that owns a decision instead of copying the same detail into multiple files.
- When writing code, use 4-space indentation.
