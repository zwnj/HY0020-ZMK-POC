# ANNA-B402 ZMK Split Keyboard PoC

Proof-of-concept repository for the split wireless keyboard + standalone numpad architecture.

Current working branch: `poc/anna-b402`.

The current design direction is ANNA-B402 / nRF52833. HY0020 remains historical context and is no longer the hardware baseline for this branch.

## Current architecture

- LEFT: permanent ZMK Split Central, USB HID or BLE HID to host.
- RIGHT: standard ZMK BLE Split Peripheral to LEFT.
- NUMPAD: standalone ZMK keyboard with USB/BLE target support.
- Desktop: optional Prospector-like BLE HID -> USB HID dongle. The dongle is not the ZMK Central.
- Pogo: power / GND / dock detect only; no key data.
- ESB: not part of the current baseline; retain only as a possible future latency experiment.

## Current decision / feasibility documents

- [`HARDWARE_BASELINE.md`](HARDWARE_BASELINE.md) — current ANNA-B402 hardware baseline.
- [`ANNA_B402_HARDWARE_INTEGRATION_FEASIBILITY.md`](ANNA_B402_HARDWARE_INTEGRATION_FEASIBILITY.md) — detailed antenna, reflow, LFXO, USB, SWD, power and one-sided-PCBA feasibility review.
- [`ANNA_B402_JLCPCB_FEASIBILITY.md`](ANNA_B402_JLCPCB_FEASIBILITY.md) — Global Sourcing, prototype quantity, JLC assembly and procurement gates.
- [`HOST_CONNECTIVITY_DECISION.md`](HOST_CONNECTIVITY_DECISION.md) — BLE-first split/host architecture and Prospector-like BLE-to-USB desktop dongle direction.
- [`VALIDATION.md`](VALIDATION.md) — later firmware/CI and system validation plan.

## Current MCU direction

Preferred module: u-blox ANNA-B402-00B / nRF52833.

Key reasons:

- 512 kB flash / 128 kB RAM.
- Native USB device support.
- 33 GPIO.
- Integrated antenna / RF implementation.
- Compact 6.5 x 6.5 mm module.
- JLCPCB already has C6124130 as an assembly-library part.

The preferred final low-frequency clock direction is an external 32.768 kHz LFXO. Internal LFRC remains useful for bring-up but is not the preferred final low-power configuration.

## Important current hardware gates

ANNA-B402 remains the preferred module, but PCB release is blocked on two items:

1. **Antenna integration** — the integrated antenna should use the u-blox edge/corner reference layout. ANNA itself should therefore sit at a suitable PCB outer edge/corner rather than simply inside the central electronics pocket.
2. **JLC reflow process** — u-blox specifies ANNA-B4 TP absolute max = 245 °C, while JLC publishes Economic PCBA reflow = 255 ± 5 °C and non-adjustable. C6124130 is listed as Economic compatible, but Economic assembly is not considered cleared until JLC explicitly confirms a compatible thermal process. Standard PCBA (published 240 ± 5 °C) is the current safe fallback.

Global Sourcing remains the preferred procurement route. The first run needs 15 installed modules (5 LEFT + 5 RIGHT + 5 NUMPAD), with roughly 18-20 parts as the working sourcing quantity until final attrition is known. Do not plan to consign loose parts from Japan.

## Repository migration status

Documentation is now ANNA-B402-first.

Firmware/build migration is intentionally lower priority than closing the hardware/manufacturing gates above. The `boards/arm/anna_b402/` scaffold exists, but `build.yaml` can be cleaned up after PCB feasibility is settled.

Historical HY0020 work should be treated as reference material, not as the acceptance baseline for `poc/anna-b402`.
