# ANNA-B402 hardware integration feasibility

Checked: 2026-09-12

This document focuses on whether ANNA-B402 can be integrated into the actual LEFT, RIGHT, and NUMPAD PCBs and assembled reliably by JLCPCB. Firmware is intentionally out of scope here.

## Executive conclusion

ANNA-B402 remains a technically strong fit for the project and there is no electrical reason to abandon it.

However, two manufacturing/layout items must be resolved before calling the hardware design release-ready:

1. **Antenna placement/reference layout**: when using the integrated antenna, ANNA-B402 should be placed at a PCB edge or corner and the u-blox antenna reference geometry must be respected. The earlier idea that the module could simply sit anywhere in the central electronics pocket is no longer a safe baseline assumption.
2. **JLCPCB reflow temperature**: u-blox gives ANNA-B4 an absolute maximum reflow peak of 245 °C, while JLCPCB currently publishes 255 ± 5 °C for Economic PCBA and says that Economic reflow temperature is not adjustable. Although C6124130 is listed by JLCPCB as Economic-and-Standard capable, the published process limits conflict. **Economic PCBA is therefore not yet cleared for ANNA-B402 until JLCPCB explicitly confirms a compatible process for C6124130.**

If JLC cannot confirm the Economic process stays within the u-blox limit, use Standard PCBA. JLC currently publishes 240 ± 5 °C for Standard PCBA. A newer Standard-only medium-temperature solder-paste option is also available at 210 ± 5 °C for an additional charge.

## Mechanical / placement feasibility

ANNA-B402 is approximately 6.5 x 6.5 x 1.2 mm, so the module itself is easily small enough for the keyboard geometry.

The important constraint is not module area but the integrated antenna.

u-blox provides three certified/reference integration directions:

- integrated antenna with the module at a PCB corner,
- integrated antenna with the module along a PCB edge,
- external antenna through a U.FL reference design.

For the integrated antenna designs, u-blox requires the antenna-strip clearance geometry to remain free of traces on all relevant layers. The reference drawings explicitly show a no-ground / no-routing region extending from the antenna side of the package.

### Project implication

For LEFT, RIGHT, and NUMPAD, prefer placing ANNA-B402 at a suitable **outer PCB edge or corner** while keeping it inside the key-layout silhouette. The rest of the power-management / gauge / RGB electronics may remain in the central pocket.

Do not place the following in or immediately adjacent to the antenna clearance region unless validated by RF testing:

- copper pours or signal traces prohibited by the reference layout,
- hot-swap socket metal,
- a metal switch plate or case structure,
- battery pouch / conductive battery foil,
- USB connector shell,
- high-current switching loops or noisy digital wiring.

A corner placement has slightly better antenna performance according to u-blox, but an edge placement is also an official reference option and may fit the keyboard layout more naturally.

For full reuse of the u-blox modular certification, the System Integration Manual requires the PCB layout and stack-up to follow the u-blox reference design. u-blox states that reference-design source files are available on request. This matters because JLC Economic PCBA supports only its standard stack-ups. Before relying on modular certification, obtain the u-blox reference-design source/stack-up and compare it against the selected JLC stack-up.

## Land pattern / assembly package

ANNA-B402 is an LGA-style surface-mount module with hidden solder joints. JLCPCB currently lists ANNA-B402-00B as:

- JLC part: C6124130,
- package: SMD-56P,
- Extended part,
- SMT Assembly,
- PCBA type: Economic and Standard,
- MSL 3,
- X-ray inspection required,
- assembly fixture required.

The package pitch / geometry is within JLCPCB placement capability. The final KiCad footprint must nevertheless be checked against the current u-blox package drawing/reference design rather than trusting a generic library footprint blindly.

## Reflow-temperature gate — currently the most important manufacturing issue

u-blox ANNA-B4 System Integration Manual R08 specifies a common SAC lead-free reflow process with:

- preheat: 150 to 200 °C,
- liquidus: 217 °C,
- **absolute maximum peak TP: 245 °C**,
- maximum time above TP - 5 °C: 10 s,
- maximum total reflow cycles: 2,
- lower TP is preferred.

JLCPCB currently publishes:

- Economic PCBA: **255 ± 5 °C, not adjustable**,
- Standard PCBA: **240 ± 5 °C**.

This creates a real contradiction between the Economic process table and the u-blox absolute maximum, even though JLC's part library marks C6124130 as Economic compatible.

Do not infer that the Economic process is safe merely from the component-library classification. The actual component body / joint thermal profile can differ from the oven-zone setpoint, but only JLCPCB can confirm how C6124130 is processed in their line.

### Required action before ordering

Ask JLCPCB support a precise question tied to the exact part number:

> For C6124130 / u-blox ANNA-B402-00B, the u-blox System Integration Manual specifies TP absolute max 245 °C and prefers a lower peak. Your Economic PCBA capability table lists 255 ± 5 °C and non-adjustable reflow temperature. Is C6124130 assembled on Economic PCBA with a validated profile that keeps the ANNA-B402 within its 245 °C component limit? If yes, please confirm the applicable peak/component temperature or process exception.

Until that answer is obtained, treat **Standard PCBA as the safe baseline** and Economic as conditional.

JLCPCB added a Standard-only medium/low-temperature solder-paste service in September 2026:

- medium: 210 ± 5 °C, about USD 32 additional,
- low: 180 ± 5 °C, about USD 80 additional.

Do not select these automatically; first determine whether normal Standard PCBA is already acceptable and whether the lower-temperature alloy is desirable for the complete BOM.

## Single-sided assembly feasibility

The electrical design can still be organized as a single assembled side:

- ANNA-B402,
- nPM1100,
- BQ27427,
- PCA9633,
- LFXO + load capacitors,
- USB ESD / CC / passive components,
- USB-C connector,
- pogo power protection / OR components.

This is compatible with the original goal of minimizing assembly-side complexity. If Standard PCBA is used because of the reflow issue, keeping the design single-sided still reduces cost and process complexity.

u-blox recommends placing ANNA-B4 on the side that undergoes the final reflow cycle and does not recommend reflowing the module upside down because of detachment risk. A single-sided assembly naturally avoids this problem.

## Low-frequency clock

The preferred final configuration remains an external 32.768 kHz LFXO.

u-blox's ANNA-B4 EVK reference uses:

- Epson FC-12M,
- 32.768 kHz,
- 20 ppm,
- two 22 pF capacitors.

This is a strong starting reference for the keyboard design. XL1 / XL2 are P0.00 / P0.01 and therefore cease to be available as GPIO when the crystal is fitted.

The final exact crystal order code and load-capacitor values must be checked against the selected crystal's load capacitance and the ANNA reference implementation before release.

Place the crystal and capacitors immediately beside XL1 / XL2 with short, quiet routing, away from USB and switching-power nodes.

## USB path

ANNA-B402 exposes native nRF52833 USB:

- VBUS pin requires 5 V for the USB interface,
- USB_DP and USB_DM are dedicated USB data pins,
- module VCC remains the normal low-voltage supply.

Recommended power/data topology:

```text
USB-C VBUS 5 V
    |
    +----> nPM1100 VBUS / charger / dynamic power path
    |          |
    |          +----> 3.0 V buck ----> ANNA VCC + low-voltage peripherals
    |
    +----> ANNA VBUS (USB interface supply / detect)

USB-C D+ / D-
    |
    +----> low-capacitance USB ESD protection ----> ANNA USBDP / USBDM

LiPo 1S ----> nPM1100 battery input / power path
```

Do **not** connect USB 5 V to ANNA VCC. USB VBUS and ANNA VCC have different roles.

The final USB-C device circuit should include the normal Type-C device-side CC configuration and low-capacitance USB ESD protection. Keep D+ / D- short and route them as a differential pair according to the final PCB stack-up.

The current HRO TYPE-C-31-M-12 / C165948 remains manufacturing-friendly: JLC currently lists it as Economic and Standard compatible with large stock.

## SWD / reset / bring-up access

ANNA-B402 supports SWD through SWDCLK and SWDIO. u-blox recommends making the SWD interface accessible and notes that GND and VDD_IO references are required.

The keyboard PCB therefore should expose at least compact programming/test contacts for:

- SWDIO,
- SWDCLK,
- GND,
- VCC / VTref,
- preferably RESET_N as well.

A full 10-pin Cortex header is not mechanically necessary for this keyboard; compact test pads / a pogo or Tag-Connect-style fixture can provide the same signals.

RESET_N has an internal pull-up, so an external pull-up is not required merely to keep the device out of reset.

## Power-management feasibility

The nPM1100 remains a good electrical fit.

Nordic specifies:

- dynamic power path,
- Li-ion / LiPo charger,
- buck output options including 3.0 V,
- buck output current up to 150 mA,
- USB-compatible input/current limiting,
- battery NTC protection.

The 3.0 V buck rail is suitable for ANNA-B402 VCC. ANNA-B402's radio current is far below the nPM1100 150 mA buck limit, leaving useful margin for the MCU and low-power peripherals. RGB current still needs its own explicit worst-case budget.

### Battery NTC gate

nPM1100 expects a 10 kOhm thermistor and Nordic's recommended curves are approximately:

- B25/50 = 3380 K,
- B25/85 = 3434 to 3435 K.

The candidate DTP443442(NTC) battery is known to contain a 10 kOhm NTC, but the exact B constant has not yet been verified. This remains a real hardware gate. Do not connect the pack NTC to nPM1100 until its curve is confirmed compatible; retain the fixed-resistor fallback option in the schematic until then.

## Battery gauge / RGB manufacturing feasibility

The existing candidates remain plausible:

- TI BQ27427YZFR / C6075475: JLC currently lists Economic and Standard assembly, DSBGA-9, X-ray required.
- NXP PCA9633DP1,118 / C88381: retain as the current RGB-driver direction, but recheck stock and exact assembly classification at order time.

Because BQ27427 is also a hidden-joint package requiring X-ray, the final PCBA quote must account for more than just the ANNA module when evaluating X-ray cost.

## Moisture sensitivity

ANNA-B402 is MSL 3. u-blox specifies that after opening the dry pack, modules must be mounted within 168 hours at no more than 30 °C / 60% RH, or stored below 10% RH; otherwise baking may be required.

This is another reason to have JLCPCB source/store/assemble the modules rather than shipping loose parts through Japan or attempting home assembly.

## Sourcing flow

Global Sourcing remains the preferred route for the first run. It does not require the modules to pass through Japan.

Target flow:

```text
Global distributor / JLC sourcing channel
                |
                v
        JLCPCB parts inventory
                |
                v
        JLCPCB PCBA assembly
                |
                v
        completed PCBAs -> Japan
```

Plan approximately 18-20 sourced modules for 15 installed modules unless the final JLC parts calculator gives a different attrition requirement.

## Go / no-go status

### Green

- ANNA-B402 electrical features and supply voltage.
- GPIO capacity.
- Native USB availability.
- External LFXO option.
- SWD bring-up access.
- nPM1100 3.0 V power architecture.
- JLC placement capability for the package.
- Global Sourcing concept for prototype quantity.

### Yellow / must close before PCB release

- Physical edge/corner placement and exact antenna no-routing/no-ground geometry on all three PCBs.
- u-blox reference stack-up versus selected JLC stack-up if modular certification reuse matters.
- Exact FC-12M order code / load-cap values.
- Candidate battery NTC B constant.
- USB ESD part and final Type-C implementation.
- Exact final JLC quote, X-ray/fixture charges, and panel strategy.

### Red / current manufacturing gate

- **Economic PCBA reflow temperature is not yet demonstrated compatible with the ANNA-B402 245 °C absolute maximum.**

If JLC confirms a validated C6124130 Economic process within the u-blox limit, Economic can return to the preferred path. Otherwise move the build to Standard PCBA rather than changing the MCU module.

## Primary references

- u-blox ANNA-B4 System Integration Manual, UBX-21000517 R08:
  https://content.u-blox.com/sites/default/files/ANNA-B4_SIM_UBX-21000517.pdf
- u-blox ANNA-B402 Data Sheet, UBX-20032372:
  https://content.u-blox.com/sites/default/files/ANNA-B402_DataSheet_UBX-20032372.pdf
- JLCPCB PCBA capabilities:
  https://jlcpcb.com/capabilities/pcb-assembly-capabilities
- JLCPCB medium / low temperature solder-paste service:
  https://jlcpcb.com/help/article/medium-low-temperature-solder-paste-service
- JLCPCB ANNA-B402 / C6124130 part page:
  https://jlcpcb.com/partdetail/UBLOX-ANNA_B40200B/C6124130
- Nordic nPM1100 product / documentation:
  https://www.nordicsemi.com/Products/nPM1100
