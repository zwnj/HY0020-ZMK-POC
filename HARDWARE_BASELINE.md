# ANNA-B402 Keyboard Hardware Baseline

Updated: 2026-09-12

This file records the current hardware baseline for the split keyboard + standalone numpad design on `poc/anna-b402`. HY0020-specific assumptions are historical and are no longer the design target for this branch.

Detailed integration/manufacturing findings are in `ANNA_B402_HARDWARE_INTEGRATION_FEASIBILITY.md`.

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

LEFT remains the ZMK Central in all normal operating modes. The desktop dongle is a BLE HID Central / USB HID bridge, not a ZMK Split Central. See `HOST_CONNECTIVITY_DECISION.md`.

## MCU module

Preferred module for LEFT, RIGHT, and NUMPAD:

- u-blox ANNA-B402-00B.
- Nordic nRF52833 SoC.
- 512 kB flash / 128 kB RAM.
- 33 GPIO.
- Native USB device support.
- Integrated 2.4 GHz antenna / antenna-pin option.
- Approx. 6.5 x 6.5 x 1.2 mm.

Reasons for selecting ANNA-B402 over the earlier HY0020 concept:

- Native USB enables USB HID on LEFT and NUMPAD without changing MCU family.
- 128 kB RAM gives substantially more ZMK headroom than the 64 kB nRF52832 baseline.
- GPIO count comfortably exceeds the RIGHT requirement of 14 matrix GPIO + 2 I2C GPIO.
- The module is physically very small.
- Antenna/RF implementation remains mostly module-level rather than requiring a discrete radio design.

### Important placement rule

The integrated antenna changes the earlier mechanical assumption: **ANNA-B402 should not simply be buried in the central electronics pocket.**

For the integrated antenna, use the u-blox reference design with the module at a PCB corner or along a PCB edge. The corner option has slightly better RF performance; the edge option is also an official reference design. Keep the specified antenna-strip area free of prohibited copper/routing on all required layers.

The central pocket remains useful for nPM1100, BQ27427, RGB driver and other electronics. Place ANNA itself at a suitable outer edge/corner while keeping the overall board within the desired key silhouette.

If modular certification reuse matters, obtain the u-blox reference-design source/stack-up and verify that the selected PCB manufacturer's stack-up reproduces it as required.

Do not copy the old HY0020 pin assignment directly. Final ANNA pin allocation must be checked against the latest u-blox data sheet / System Integration Manual, including RESET, SWD, NFC, LFCLK, and low-frequency/drive-strength guidance.

## Low-frequency clock

ANNA-B402 already contains the high-frequency clock required by the SoC. For the 32.768 kHz low-frequency clock, the preferred final direction is an external LFXO rather than relying on the internal LFRC.

Preferred reference starting point from the u-blox EVK:

- Epson FC-12M family.
- 32.768 kHz.
- 20 ppm.
- 22 pF load capacitors in the EVK implementation.
- XL1 / XL2 = nRF52833 P0.00 / P0.01, so those pins are unavailable as GPIO when LFXO is fitted.

The final exact crystal order code and capacitor values must be checked before PCB release. Place the crystal/capacitors close to XL1/XL2 with short routing away from switching-power and high-speed digital signals.

Internal LFRC remains valid for bring-up, but it is not the preferred final low-power configuration because periodic calibration increases standby current.

## USB

Native nRF52833 USB is part of the ANNA baseline.

- LEFT: USB HID + BLE HID + Split Central.
- NUMPAD: USB HID + BLE HID standalone keyboard.
- RIGHT: no direct host HID requirement; USB-C may still be present for charging / service as needed.

ANNA-B402 exposes dedicated VBUS, USBDP and USBDM pins. VBUS requires 5 V for USB operation while ANNA VCC remains on the normal low-voltage supply.

Target topology:

```text
USB-C 5 V
    +----> nPM1100 VBUS / charger / power path
    |          |
    |          +----> 3.0 V buck ----> ANNA VCC + low-voltage peripherals
    |
    +----> ANNA VBUS

USB D+ / D- ----> low-capacitance ESD ----> ANNA USBDP / USBDM
```

Do not connect USB 5 V to ANNA VCC. Final USB-C device-side CC implementation and ESD component must be selected before release.

## SWD / reset access

Expose compact programming/test access for at least:

- SWDIO,
- SWDCLK,
- GND,
- VCC / VTref,
- preferably RESET_N.

u-blox recommends making SWD available and notes that GND and VDD_IO references are required. A full 10-pin header is not necessary; test pads / pogo fixture / Tag-Connect-style access is acceptable for this mechanical design.

RESET_N has an internal pull-up.

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
- Use hot-swap footprint rotation to create electronics space; 180 degrees is the first choice, with 90/270 degrees only where useful.
- Place ANNA-B402 at an RF-compatible outer edge/corner rather than forcing it into the middle of the electronics cluster.
- Re-evaluate the remaining electronics pocket using the actual nPM1100, BQ27427, PCA9633, USB protection, connector and power-switch footprints.
- The battery does not have to fit inside the electronics pocket.
- Keep switch-socket metal, plate metal, case metal and battery conductive surfaces out of the ANNA antenna clearance region unless RF testing validates the arrangement.

## Battery

Preferred common battery candidate for LEFT, RIGHT and NUMPAD remains:

- DATA POWER DTP443442(NTC).
- 1-cell LiPo, nominal 3.7 V.
- 640 mAh.
- Approx. 44 x 35 x 4.6 mm.
- Integrated protection.
- Integrated 10 kOhm NTC.
- Replaceable battery with keyed 3-pin connection; verify final connector polarity explicitly.

Battery placement is a case-level mechanical problem rather than a primary MCU-placement constraint. It may sit below multiple key positions as long as there is adequate protection from hot-swap sockets, stabilizers, screws, solder tails, and other sharp or unsupported hardware.

Do not place the battery pouch in the ANNA antenna clearance / counterpoise-critical region without RF validation.

### Battery NTC gate

The nPM1100 expects a 10 kOhm battery thermistor and Nordic recommends approximately B25/50 = 3380 K / B25/85 = 3434-3435 K. The candidate battery's exact B constant remains unverified. Keep the fixed-resistor fallback in the schematic until compatibility is confirmed.

## Power management

Preferred PMIC remains Nordic nPM1100.

Target functions:

- 1-cell LiPo charging.
- Dynamic power path.
- 3.0 V buck rail for ANNA-B402 and low-voltage peripherals.
- Buck capacity up to 150 mA.
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

JLC currently lists BQ27427YZFR / C6075475 as Economic and Standard capable, DSBGA-9, with X-ray required. Final design capacity, taper current and terminate voltage must be matched to the selected LiPo.

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

ESB is not part of the current baseline and should only be reconsidered if measurements show a concrete latency or reliability benefit that justifies the extra maintenance.

## JLCPCB manufacturing target

JLCPCB assembly remains the preferred manufacturing path, but **Economic PCBA is now conditional rather than assumed**.

Known facts:

- C6124130 is listed by JLC as Economic and Standard capable, MSL 3, X-ray required, fixture required.
- u-blox specifies ANNA-B4 reflow TP absolute max = 245 °C and prefers a lower peak.
- JLC publishes Economic reflow = 255 ± 5 °C, not adjustable.
- JLC publishes Standard reflow = 240 ± 5 °C.
- JLC now offers Standard-only medium-temperature paste at 210 ± 5 °C for an additional fee.

Therefore:

- **Do not release ANNA-B402 to Economic PCBA until JLC explicitly confirms that the C6124130 Economic process keeps the module within the u-blox 245 °C limit.**
- If that cannot be confirmed, use Standard PCBA.
- Medium-temperature Standard assembly is a fallback if additional thermal margin is desired after reviewing the full BOM.

Other manufacturing goals:

- Keep the design single-sided for assembly where practical even if Standard PCBA is selected.
- Put ANNA on the assembled/reflow-up side; avoid reflowing it upside down.
- Do not hand-solder/rework ANNA for the production prototype set.
- Global Sourcing is the preferred procurement path for ANNA-B402; do not plan Japan-to-JLC consignment.
- Required first-run quantity is 15 installed ANNA modules; approximately 18-20 is the current sourcing planning range pending the JLC parts calculator / quote.
- A combined LEFT + RIGHT + NUMPAD mouse-bite panel remains a quote-driven option.

Prefer 0603 passives unless space or a reference design clearly favors smaller parts.

## Firmware design rules

Keep custom ZMK work minimal.

Preferred order:

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

Do not implement in the baseline:

- dynamic ZMK Central election,
- runtime LEFT <-> dongle Central switching,
- custom ESB split transport,
- runtime BLE Split <-> ESB Split switching,
- pogo key-data transport,
- custom I2C/UART split protocol.

## Firmware repository state

Firmware migration is intentionally lower priority than hardware feasibility at the current stage. The `anna_b402` board scaffold exists, but CI/build cleanup can wait until the manufacturing/layout gates above are closed.

## Items still requiring validation before PCB release

- JLC confirmation of C6124130 Economic reflow compatibility, or decision to use Standard.
- Exact u-blox antenna reference layout on LEFT, RIGHT and NUMPAD.
- Reference stack-up versus selected JLC stack-up if modular certification reuse matters.
- Final ANNA land pattern / KiCad footprint / CPL orientation.
- Final 32.768 kHz crystal order code and load-cap values.
- Final GPIO allocation for LEFT, RIGHT and NUMPAD.
- Battery NTC B-constant compatibility with nPM1100.
- USB ESD part and final Type-C device implementation.
- Battery current / runtime and RGB power budget.
- RF performance with the chosen plate, case, battery placement and nearby metal.
- Pogo voltage drop / contact reliability.
- Final battery connector orientation / polarity.
- Global Sourcing price, MOQ and lead time immediately before ordering.
- JLC quote comparison for three separate designs versus a combined panel.
