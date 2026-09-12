# ANNA-B402 Keyboard Hardware Baseline

Updated: 2026-09-12

This file records the current hardware baseline for the split keyboard + standalone numpad design on `poc/anna-b402`. HY0020-specific assumptions are historical and are no longer the design target for this branch.

## System architecture

Physical order:

```text
NUMPAD <-> LEFT / CENTRAL <-> RIGHT
```

Logical roles:

- LEFT: ANNA-B402 / nRF52833, permanent ZMK Split Central.
- RIGHT: ANNA-B402 / nRF52833, ZMK Split Peripheral over standard BLE to LEFT.
- NUMPAD: ANNA-B402 / nRF52833, standalone ZMK keyboard rather than a third Split Peripheral.
- Pogo connections carry power, GND, and dock-detect signals only. Do not transport key data over pogo.

Host paths:

```text
RIGHT -- ZMK BLE Split --> LEFT -- USB HID ----------------> host
                              |
                              +-- BLE HID ------------------> laptop / other BLE host
                              |
                              +-- BLE HID --> desktop dongle --> USB HID --> desktop
```

LEFT remains the ZMK Central in all normal operating modes. The desktop dongle is a BLE HID Central / USB HID bridge, not a ZMK Split Central. See `HOST_CONNECTIVITY_DECISION.md` for the detailed decision and rationale.

## MCU module

Preferred module for LEFT, RIGHT, and NUMPAD:

- u-blox ANNA-B402-00B.
- Nordic nRF52833 SoC.
- 512 kB flash / 128 kB RAM.
- 33 GPIO.
- Native USB device support.
- Integrated 2.4 GHz antenna and RF matching.
- Approx. 6.5 x 6.5 x 1.2 mm.

Reasons for selecting ANNA-B402 over the earlier HY0020 concept:

- Native USB enables USB HID on LEFT and NUMPAD without changing MCU family.
- 128 kB RAM gives substantially more ZMK headroom than the 64 kB nRF52832 baseline.
- GPIO count comfortably exceeds the RIGHT requirement of 14 matrix GPIO + 2 I2C GPIO.
- The module remains compact enough for the central electronics pocket.
- Antenna/RF integration remains module-level, avoiding a discrete RF design.

Do not copy the old HY0020 pin assignment directly. Final ANNA pin allocation must be checked against the latest u-blox data sheet / System Integration Manual, including RESET, SWD, NFC, LFCLK, and RF-sensitive guidance.

## Low-frequency clock

ANNA-B402 already contains the high-frequency clock required by the SoC. For the 32.768 kHz low-frequency clock, the preferred direction is an external LFXO rather than relying on the internal LFRC for the final battery-powered design.

Preferred baseline:

- 32.768 kHz watch crystal.
- Approximately 20 ppm class.
- Connected to XL1 / XL2 (nRF52833 P0.00 / P0.01), making those pins unavailable as general GPIO.
- Load capacitors selected from the chosen crystal load capacitance, ANNA pin capacitance, PCB stray capacitance, and the u-blox reference design.

The u-blox EVK uses an external 32.768 kHz crystal and is the preferred reference starting point. The final crystal MPN and capacitor values remain to be locked before PCB release.

Internal LFRC remains a valid fallback for bring-up, but it is not the preferred final low-power configuration because periodic calibration increases standby current.

## USB

Native nRF52833 USB is part of the ANNA baseline.

- LEFT: USB HID + BLE HID + Split Central.
- NUMPAD: USB HID + BLE HID standalone keyboard.
- RIGHT: no direct host HID requirement; USB-C may still be present for charging / service as needed.

USB VBUS, D+, and D- must be routed according to u-blox / Nordic guidance. Include appropriate ESD protection in the final PCB design.

## Key matrices and GPIO budget

Current matrix sizes:

- LEFT: 30 keys, 5 x 6 matrix, 11 matrix GPIO.
- RIGHT: 43 keys, 5 x 9 matrix, 14 matrix GPIO.
- NUMPAD: approximately 20-27 keys; 5 x 6 remains the provisional matrix envelope until the layout is finalized.

Shared I2C requires 2 additional GPIO. The current ANNA board scaffold uses:

- SDA: P0.11.
- SCL: P0.12.

These assignments are provisional until the final per-unit pin table is reviewed against the ANNA documentation and PCB layout.

LEFT additionally requires dock-detect inputs for RIGHT and NUMPAD. Prefer active-low detection with pull-ups and GPIO interrupts so continuous polling is unnecessary.

## Switches and mechanical layout

- MX-compatible hot-swap is mandatory.
- Keep the external PCB silhouette within the key-layout outline as much as practical.
- Use hot-swap footprint rotation to create central electronics space; 180 degrees is the first choice, with 90/270 degrees only where useful.
- Re-evaluate the central electronics pocket using the actual ANNA-B402, nPM1100, BQ27427, PCA9633, USB protection, and connector footprints.
- The battery does not have to fit inside the electronics pocket.

## Battery

Preferred common battery candidate for LEFT, RIGHT, and NUMPAD remains:

- DATA POWER DTP443442(NTC).
- 1-cell LiPo, nominal 3.7 V.
- 640 mAh.
- Approx. 44 x 35 x 4.6 mm.
- Integrated protection.
- Integrated 10 kOhm NTC.
- Replaceable battery with keyed 3-pin connection; verify final connector polarity explicitly.

Battery placement is a case-level mechanical problem rather than a primary MCU-placement constraint. It may sit below multiple key positions as long as there is adequate protection from hot-swap sockets, stabilizers, screws, solder tails, and other sharp or unsupported hardware.

Avoid placing conductive structures immediately in the ANNA antenna keep-out / counterpoise region unless RF testing supports the arrangement.

## Power management

Preferred PMIC remains Nordic nPM1100.

Target functions:

- 1-cell LiPo charging.
- Power path.
- 3.0 V buck rail for ANNA-B402 and peripherals.
- Battery-pack NTC support if the selected pack curve is compatible.

Each keyboard unit has its own LiPo and PMIC.

LEFT USB VBUS may supply 5 V to RIGHT and NUMPAD through protected/current-limited pogo branches. LEFT battery power must not be boosted and exported to the other units.

RIGHT and NUMPAD must isolate local USB 5 V and pogo 5 V so neither source back-feeds the other. Schottky OR remains the simple baseline unless later efficiency measurements justify an active power mux / ideal-diode solution.

## Battery gauge

Preferred fuel gauge remains TI BQ27427 family.

Reasons:

- Zephyr `ti,bq274xx` support is available.
- Keep battery reporting on the standard Zephyr Sensor API path.
- Avoid a custom ZMK fuel-gauge adapter.
- Share the I2C bus with the RGB driver.

Final design capacity, taper current, terminate voltage, and battery thermistor details must be matched to the final LiPo before release.

## RGB status

RIGHT RGB remains mandatory.

Preferred current direction:

- NXP PCA9633 family.
- I2C 4-channel LED driver.
- Three channels for RGB, one spare.
- Use upstream Zephyr LED support where practical.

Supply availability and exact JLC part number must be rechecked before ordering.

## Docking / pogo

Pogo signals are limited to:

- GND.
- Optional second GND.
- USB-derived 5 V.
- Dock detect.
- Optional reserve contact if mechanically useful.

Do not transport keyboard data over pogo.

Do not implement runtime wired/BLE split switching based on docking state.

## Desktop dongle

A dedicated desktop dongle is part of the intended user experience, but it does not change the keyboard split topology.

Preferred behavior:

- Keyboard side remains standard BLE HID.
- Dongle acts as BLE HID Central and forwards reports to the desktop over USB HID.
- A Prospector-like display can be added later.
- If the display needs information not present in HID/Battery Service, add the smallest practical status channel, preferably a small custom BLE GATT service.

Nordic nRF Desktop is a candidate reference/base for the BLE-to-USB bridge. ESB is not part of the current baseline and should only be reconsidered if measurements show a concrete latency or reliability benefit that justifies the extra maintenance.

## JLCPCB manufacturing target

Preferred production path:

- JLCPCB Economic PCBA wherever practical.
- Single-sided SMT placement where possible.
- Concentrate auto-assembled SMD parts on one assembly side.
- ANNA-B402 is machine-assembled by JLCPCB; do not plan on hand-soldering the LGA module for the production prototype set.
- ANNA-B402 JLC part C6124130 is currently listed as Economic and Standard capable and requires X-ray inspection.
- Global Sourcing is the preferred procurement direction for ANNA-B402 unless the direct JLC stock / Pre-Order economics improve.
- Required production quantity is 5 each of LEFT, RIGHT, and NUMPAD = 15 installed ANNA modules. Source a small margin above 15 for assembly attrition; approximately 18-20 is the current planning range, with the final quantity determined by the JLC parts calculator / quote.
- A combined LEFT + RIGHT + NUMPAD mouse-bite panel may be cost-effective, but panelization remains a quote-driven manufacturing decision rather than a hardware requirement.

Prefer 0603 passives unless space or the reference design clearly favors smaller parts.

## Firmware design rules

Keep custom ZMK work minimal.

Preferred order of implementation:

1. Standard ZMK functionality.
2. Devicetree / Kconfig + standard Zephyr drivers.
3. Small external custom module only where necessary.

Baseline:

- Standard ZMK BLE split transport between RIGHT and LEFT.
- Standard ZMK BLE HID from LEFT.
- Standard ZMK USB HID from LEFT.
- Standard ZMK USB/BLE HID on NUMPAD.
- ZMK Studio on LEFT and NUMPAD as practical.
- Standard Zephyr Sensor API for the fuel gauge.
- Standard Zephyr LED API for RGB.
- Only a thin custom status module is acceptable for battery/BLE/dock/RGB policy.

Do not implement in the baseline:

- dynamic ZMK Central election,
- runtime LEFT <-> dongle Central switching,
- custom ESB split transport,
- runtime BLE Split <-> ESB Split switching,
- pogo key-data transport,
- custom I2C/UART split protocol.

## Firmware repository state

The `poc/anna-b402` branch already contains an `anna_b402` board scaffold with nRF52833, GPIO0/GPIO1, I2C, and native USB enabled.

The branch is not yet fully migrated: `build.yaml` still contains HY0020 / nRF52832 jobs. CI must be converted to build `anna_b402` explicitly before ANNA validation results are considered authoritative.

## Items still requiring validation

- Final ANNA-B402 land pattern and JLC footprint review against u-blox recommendations.
- Antenna keep-out / counterpoise implementation in each unit.
- Final 32.768 kHz crystal and load-capacitor values.
- Final GPIO allocation for LEFT, RIGHT, and NUMPAD.
- LEFT USB + BLE + Split + Studio build and memory usage on nRF52833.
- RIGHT BLE Split build.
- NUMPAD USB/BLE standalone build.
- Battery current and real runtime.
- RF performance with the chosen plate, case, battery placement, and nearby metal.
- Battery NTC compatibility with nPM1100.
- Pogo voltage drop and contact reliability.
- Final battery connector orientation and polarity.
- Global Sourcing price/MOQ/lead-time immediately before ordering.
- JLCPCB PCBA quote comparison for three separate designs versus a combined panel.
