# HY0020 Keyboard Hardware Baseline

This file records the current hardware baseline for the split keyboard + standalone numpad design.

## System architecture

- LEFT: AE-HY0020-DIP / nRF52832, ZMK Split Central, BLE HID to host PC, ZMK Studio.
- RIGHT: AE-HY0020-DIP / nRF52832, ZMK Split Peripheral over BLE to LEFT.
- NUMPAD: AE-HY0020-DIP / nRF52832, standalone BLE HID keyboard, own ZMK Studio/keymap.
- No USB dongle.
- Pogo connections carry power/GND/detect only; no key data.

## MCU module

Use Akizuki AE-HY0020-DIP (DIP20 / 300 mil / 2.54 mm pitch) for all three units.

The module should be removable. The keyboard PCB therefore uses a low-profile 20-pin 300 mil machine-pin / round-pin DIP socket rather than soldering the module permanently.

Placement goal:

- Keep the PCB outline within the key outline as much as practical.
- Use hot-swap socket orientation (180 deg first, 90/270 deg only where useful) to create an electronics pocket below the key field.
- Keep the AE-HY0020-DIP and battery within the key-field footprint.
- Verify vertical clearance in 3D before PCB release.

## Switches

- MX-compatible hot-swap is mandatory.
- JLCPCB Economic PCBA is preferred.
- Hot-swap sockets and SMT electronics should be concentrated on one assembly side where practical.

## Battery

Initial common battery target for LEFT, RIGHT and NUMPAD:

- 1-cell LiPo, nominal 3.7 V.
- Approximately 500 mAh.
- Protected cell preferred.
- Japanese domestic availability and traceable safety documentation are selection priorities.
- Battery should be replaceable via a keyed 2-pin connector with explicitly verified polarity.

500 mAh is the starting point to maximize part commonality. LEFT can be increased later if real measured runtime justifies it.

## Power management

Preferred PMIC: Nordic nPM1100-QDAA-R.

Target configuration:

- 1-cell LiPo charger.
- 3.0 V buck output for AE-HY0020-DIP and peripherals.
- About 100 mA charge current as the starting point.
- USB-C is power/charging only; no USB data is required by HY0020.

Each unit has its own LiPo and nPM1100.

LEFT USB VBUS may supply 5 V to RIGHT and NUMPAD through pogo contacts, but LEFT battery power must never be boosted/exported to the other units.

## Battery gauge

Preferred fuel gauge: TI BQ27427 family.

Reasons:

- Standard Zephyr Sensor API driver path compatible with ZMK battery reporting.
- No custom ZMK fuel-gauge adapter should be required.
- Shared I2C bus with the RGB driver.

Final design-capacity, taper-current and terminate-voltage values must be matched to the actual selected LiPo before hardware release.

## RGB status

Preferred Economic-oriented candidate: NXP PCA9633 family.

- I2C LED driver.
- Three channels used for RGB, fourth channel reserved.
- Upstream Zephyr LED driver available.
- Hardware blink support can reduce periodic MCU wakeups for status indication.

The RGB/status function is mandatory on RIGHT as well as LEFT/NUMPAD.

## I2C

Use the shared I2C bus for:

- BQ27427 fuel gauge.
- RGB LED driver.

Current planned HY0020 pins:

- P0.28: SDA.
- P0.30: SCL.

RIGHT therefore consumes all 16 available GPIO when combined with its 5x9 matrix.

## Dock detection

LEFT planned detect inputs:

- P0.09: RIGHT_DET.
- P0.10: NUMPAD_DET.

Prefer active-low detection with pull-up and GPIO interrupt so no continuous polling is needed.

## Pogo power

LEFT USB VBUS export branches:

- one protected/current-limited branch to RIGHT,
- one protected/current-limited branch to NUMPAD.

A programmable/current-limited load switch such as TPS2553 is the current preferred concept for each branch.

RIGHT and NUMPAD must isolate local USB 5 V and pogo 5 V so neither source can back-feed the other. A simple Schottky-diode OR is the current baseline unless later efficiency measurements justify an active ideal-diode/power-mux solution.

## JLCPCB manufacturing target

Preferred production path:

- JLCPCB Economic PCBA wherever possible.
- AE-HY0020-DIP installed after PCBA because the module itself is not required to be SMT-assembled by JLCPCB.
- Low-profile DIP socket for the AE-HY0020-DIP is through-hole/manual assembly.
- Prefer 0603 passives unless space forces smaller parts.
- Keep all SMT components on one assembly side when possible to stay compatible with Economic assembly constraints.

## Firmware design rules

Keep custom ZMK work minimal:

- Standard ZMK BLE split transport.
- Standard ZMK BLE HID.
- Standard ZMK Studio.
- Standard Zephyr Sensor API for the fuel gauge.
- Standard Zephyr LED API for RGB.
- Only a thin external status module is acceptable for mapping ZMK battery/BLE/split/dock events to RGB indications.
- No custom split transport, dynamic-central scheme, pogo key transport, or USB dongle.

## Items still requiring physical validation

- AE-HY0020-DIP/socket height versus hot-swap sockets and bottom case.
- RF performance with the chosen plate/case materials.
- Deep-sleep and wake behavior with Studio + split central.
- Battery current and real runtime.
- Pogo voltage drop and contact reliability.
- Final LiPo dimensions, connector and polarity.
- Final JLCPCB/LCSC availability immediately before ordering.
