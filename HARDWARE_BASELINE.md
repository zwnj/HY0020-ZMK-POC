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
- Use hot-swap socket orientation (180 deg first, 90/270 deg only where useful) to create a central electronics pocket below the key field.
- Reserve that central electronics pocket for the AE-HY0020-DIP and its surrounding electronics; the battery does not need to occupy this pocket.
- Verify vertical clearance in 3D before PCB release.

## Switches

- MX-compatible hot-swap is mandatory.
- JLCPCB Economic PCBA is preferred.
- Hot-swap sockets and SMT electronics should be concentrated on one assembly side where practical.

## Battery

Preferred common battery candidate for LEFT, RIGHT and NUMPAD:

- DATA POWER DTP443442(NTC).
- 1-cell LiPo, nominal 3.7 V.
- 640 mAh.
- Approx. 44 x 35 x 4.6 mm.
- Built-in overcharge, over-discharge and over-current protection.
- Integrated 10 kOhm NTC thermistor.
- JST ZH-series 3-pin plug (ZHR-3): + / NTC / -.
- Replaceable battery; use a mating keyed 3-pin board connector and explicitly verify polarity.

Battery placement is independent of the central electronics pocket. The preferred mechanical concept is to mount the flat pouch cell below the keyboard PCB in a dedicated bottom-case cavity, positioned wherever there is sufficient vertical clearance and mechanical protection. It may extend under multiple key positions as long as it does not interfere with hot-swap sockets, stabilizers, fasteners, pogo hardware, or the AE-HY0020 antenna region.

Do not place screws, sharp solder tails, socket pins, or unsupported PCB features directly against the LiPo pouch. Provide a rigid floor or tray plus electrical insulation between the battery and PCB hardware. Avoid placing the LiPo directly below the HY0020 antenna unless RF testing confirms that the pouch cell does not materially degrade performance.

This battery is preferred over the earlier 500 mAh target because the 4.6 mm thickness is useful for a low-profile keyboard while still increasing capacity. It is also sold through Japanese electronics distributors.

The nPM1100 supports battery thermal protection through its NTC pin and is designed around a 10 kOhm battery thermistor. Before PCB release, verify the exact thermistor B-constant in the selected battery against the nPM1100 requirement. The PCB should provide an assembly option so the nPM1100 NTC input can use either the pack NTC or the manufacturer's recommended fixed-resistor fallback, but never both at once.

Use the same battery in all three units initially for part commonality. Change LEFT capacity only if measured runtime later justifies it.

## Power management

Preferred PMIC: Nordic nPM1100-QDAA-R.

Target configuration:

- 1-cell LiPo charger.
- 3.0 V buck output for AE-HY0020-DIP and peripherals.
- About 100 mA charge current as the starting point.
- Use the battery-pack NTC for charger thermal protection if its thermistor curve is verified compatible.
- USB-C is power/charging only; no USB data is required by HY0020.

Each unit has its own LiPo and nPM1100.

LEFT USB VBUS may supply 5 V to RIGHT and NUMPAD through pogo contacts, but LEFT battery power must never be boosted/exported to the other units.

## Battery gauge

Preferred fuel gauge: TI BQ27427 family.

Reasons:

- Standard Zephyr Sensor API driver path compatible with ZMK battery reporting.
- No custom ZMK fuel-gauge adapter should be required.
- Shared I2C bus with the RGB driver.

Final design-capacity, taper-current and terminate-voltage values must be matched to the actual selected LiPo before hardware release. The current design-capacity target is 640 mAh if DTP443442(NTC) is retained.

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
- Battery connector may also be manually installed if the preferred JST ZH mating header is not economical for PCBA.
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
- Battery cavity location and 44 x 35 x 4.6 mm clearance under the PCB.
- Separation of the LiPo pouch from sharp hardware and the HY0020 antenna region.
- Exact battery NTC B-constant compatibility with nPM1100.
- RF performance with the chosen plate/case materials.
- Deep-sleep and wake behavior with Studio + split central.
- Battery current and real runtime.
- Pogo voltage drop and contact reliability.
- Final battery connector orientation and polarity.
- Final JLCPCB/LCSC availability immediately before ordering.
