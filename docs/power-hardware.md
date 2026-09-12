# Common power and battery block

This document defines the power block to reuse across the 30-key side, 43-key side, and future NumPad. The physical key layout may differ, but the battery, regulator, power-switch, and battery-sense topology should remain the same unless a product has a specific reason to diverge.

## Important AE-HY0020-DIP constraint

AE-HY0020-DIP exposes a `3V3` pin that is connected to the HY0020 VDD rail. The carrier does not provide a regulator that makes a fully charged 1-cell LiPo safe for that pin.

HY0020 operates from 1.7 V to 3.6 V, so a 1-cell LiPo must not be connected directly to the AE-HY0020-DIP `3V3` pin. A fully charged conventional LiPo can reach 4.2 V.

## Fixed rail topology

Use these rail names in schematics:

- `VBAT`: raw 1-cell LiPo / charger battery node
- `VBAT_SW`: battery voltage after the physical power switch
- `3V3`: regulated logic rail
- `VBAT_SENSE`: divided battery voltage connected to the HY0020 ADC pin

Baseline topology:

```text
1S LiPo
  |
 VBAT -------- optional future charger
  |
 SPST power switch
  |
 VBAT_SW ------- 1 MΩ -------+------> P0.30 / AIN6
  |                          |
  |                         1 MΩ
  |                          |
  |                         GND
  |
 TPS7A0233PDBVR
  |
 3V3 ----------> AE-HY0020-DIP 3V3
  |
  +------------> 74HC595 VCC
  +------------> other low-power 3.3 V peripherals
```

The physical power switch is intentionally before both the LDO and the battery divider. When the product is switched off, neither the regulator input nor the HY0020 ADC input remains connected to the battery.

## 3.3 V regulator

Preferred production regulator:

- Manufacturer: Texas Instruments
- MPN: `TPS7A0233PDBVR`
- JLCPCB part: `C2887324`
- Package: SOT-23-5
- Output: fixed 3.3 V
- Maximum output current: 200 mA
- Input range: suitable for a 1-cell LiPo
- Very low quiescent current
- Active output discharge on the selected `P` version
- JLCPCB Economic PCBA compatible

Pin assignment for the SOT-23-5 DBV package:

| Pin | Name | Connection |
| ---: | --- | --- |
| 1 | IN | `VBAT_SW` |
| 2 | GND | GND |
| 3 | EN | `VBAT_SW` / IN, so the regulator is enabled whenever the physical switch is on |
| 4 | NC | no connection |
| 5 | OUT | `3V3` |

Use a 1 µF ceramic capacitor from IN to GND and a 1 µF ceramic capacitor from OUT to GND, placed close to the regulator. Follow the final TI datasheet and footprint recommendations when laying out the PCB.

The active-discharge version is preferred because HY0020 documentation specifically warns that VDD can remain charged for a long time in low-current systems and should fall below 0.3 V during power-off. This should still be verified on the assembled hardware with the final output capacitance.

The 200 mA rail is intended for HY0020, the 74HC595, and ordinary low-power peripherals. A future design with a large per-key RGB load should use a separately budgeted power path instead of assuming the RGB load can be added to this regulator indefinitely.

## Physical power switch

Use an SPST mechanical switch in series between `VBAT` and `VBAT_SW`.

Reasons for switching the battery rail instead of only switching the LDO enable pin:

- true disconnection of the keyboard electronics from the battery
- no battery-divider current while switched off
- no battery voltage presented to an unpowered HY0020 ADC input
- the charger, if added later, can remain connected to `VBAT` and charge the cell while the keyboard is switched off

The exact switch manufacturer/footprint is not fixed yet because enclosure and edge-placement requirements are product-specific. The electrical topology is fixed.

## Battery voltage sensing

Use HY0020 `P0.30 / AIN6` as the common battery ADC pin.

Divider:

- upper resistor (`VBAT_SW` to `VBAT_SENSE`): 1 MΩ
- lower resistor (`VBAT_SENSE` to GND): 1 MΩ
- recommended JLCPCB resistor for both positions: `C26083`, 1 MΩ, 1%, 0402, Basic

At 4.2 V battery voltage, `VBAT_SENSE` is approximately 2.1 V. Divider current is approximately 2.1 µA while the keyboard is on and zero through this path when the physical switch is off.

Using the same resistor value for both legs keeps the BOM simple and the firmware ratio exact. The divider has a 500 kΩ Thevenin source resistance. ZMK v0.3's voltage-divider driver configures the nRF52 SAADC for a 40 µs acquisition time, which is the intended baseline for this high-value divider. Final hardware should still be compared against a multimeter and the divider values revisited if real-board accuracy is not adequate.

Do not add a large capacitor from `VBAT_SENSE` to ground by default. A large stored charge on the ADC node can remain briefly after the power switch opens and can complicate power-off behavior. Add filtering only if measurements on the real PCB show that it is necessary.

ZMK configuration for this divider:

```dts
&adc {
    status = "okay";
};

/ {
    chosen {
        zmk,battery = &vbatt;
    };

    vbatt: vbatt {
        compatible = "zmk,battery-voltage-divider";
        io-channels = <&adc 6>;
        output-ohms = <1000000>;
        full-ohms = <2000000>;
    };
};
```

`adc 6` corresponds to HY0020 `P0.30 / AIN6`.

## 74HC595 supply

The 74HC595 is powered from the regulated `3V3` rail, not directly from the battery.

Keep the existing 100 nF local bypass capacitor immediately beside the 74HC595 VCC/GND pins. The LDO output capacitor is not a substitute for the IC-local bypass capacitor.

## Charger boundary

Battery charging is intentionally not frozen by this document yet. If an onboard charger is later added, connect its battery node to `VBAT`, on the battery side of the power switch. This preserves charging while the keyboard is switched off and does not require changing the downstream power block.

The choice of USB connector, charger IC, charge current, battery connector, and battery protection remains a separate decision.

## PCB placement

Recommended placement order:

1. Battery connector and physical power switch near an accessible board edge.
2. LDO and its input/output capacitors close together, outside the HY0020 antenna keepout.
3. Route `3V3` from the regulator to the HY0020-DIP and 74HC595 with a short, low-impedance path.
4. Place the battery divider close to the HY0020 side of the board and route `VBAT_SENSE` cleanly to P0.30.
5. Keep the high-impedance `VBAT_SENSE` trace away from the antenna region and unnecessarily long SPI/clock-parallel runs.
6. Preserve space around the AE-HY0020-DIP through-hole pads for manual installation and rework.

## Production validation

Before freezing a production PCB, verify on assembled hardware:

- `3V3` never exceeds the HY0020 operating limit over the expected battery range
- the product powers up reliably at high and low battery voltage
- after switching off, HY0020 VDD falls below 0.3 V promptly
- the battery reading matches a multimeter closely enough for ZMK battery reporting
- regulator temperature and transient behavior are acceptable with all planned peripherals enabled
- final worst-case 3.3 V rail current remains comfortably below the regulator limit
