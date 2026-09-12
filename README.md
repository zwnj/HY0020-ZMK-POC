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

## Current decision documents

- [`HARDWARE_BASELINE.md`](HARDWARE_BASELINE.md) — current ANNA-B402 hardware, power, clock, manufacturing, and firmware baseline.
- [`HOST_CONNECTIVITY_DECISION.md`](HOST_CONNECTIVITY_DECISION.md) — BLE-first host/split architecture, permanent LEFT Central, and Prospector-like BLE-to-USB desktop dongle direction.
- [`ANNA_B402_JLCPCB_FEASIBILITY.md`](ANNA_B402_JLCPCB_FEASIBILITY.md) — JLCPCB Economic compatibility, prototype quantity, Global Sourcing direction, and manufacturing gate.
- [`VALIDATION.md`](VALIDATION.md) — ANNA firmware/CI, host-connectivity, and hardware validation plan.

## Current MCU direction

Preferred module: u-blox ANNA-B402-00B / nRF52833.

Key reasons:

- 512 kB flash / 128 kB RAM.
- Native USB device support.
- 33 GPIO.
- Integrated antenna/RF implementation.
- Compact 6.5 x 6.5 mm module.
- JLCPCB C6124130 is compatible with Economic PCBA, although sourcing is currently expected to use Global Sourcing rather than public JLC stock.

The preferred final low-frequency clock direction is an external 32.768 kHz LFXO. Internal LFRC remains useful for bring-up but is not the preferred final low-power configuration.

## Repository migration status

Documentation has been migrated to the ANNA-B402 baseline.

Firmware/build migration is still in progress:

- `boards/arm/anna_b402/` scaffold exists.
- Native USB and I2C are present in the ANNA board definition.
- `build.yaml` still contains HY0020 / nRF52832 jobs and must be migrated to explicit `anna_b402` builds.
- The next meaningful firmware milestone is a clean LEFT + RIGHT ANNA CI build with flash/RAM usage recorded.

Historical HY0020 work should be treated as reference material, not as the acceptance baseline for `poc/anna-b402`.
