# ANNA-B402 JLCPCB Economic feasibility

Checked: 2026-09-12

This document tracks the manufacturing and sourcing gate for using ANNA-B402 on LEFT, RIGHT, and NUMPAD.

## Required quantity

The project is not a mass-production design.

Planned first build:

- LEFT: 5 boards.
- RIGHT: 5 boards.
- NUMPAD: 5 boards.
- Total installed ANNA-B402 modules: **15**.

Do not reject a sourcing path merely because inventory is not suitable for hundreds of units. The relevant question is whether the current order can reliably source roughly 15 installed parts plus a small assembly margin. A planning quantity around **18-20 modules** is reasonable until the JLC parts calculator / final quote gives the exact attrition requirement.

MOQ should also remain appropriate for this prototype scale. A sourcing option that forces purchase of 50-100+ modules is unattractive unless there is an exceptional price or technical reason.

## ANNA-B402-00B / C6124130

JLCPCB listing:

- Manufacturer: u-blox.
- MPN: ANNA-B402-00B.
- JLCPCB part: C6124130.
- JLC classification: Extended.
- Assembly type: SMT Assembly.
- Package: SMD-56P.
- PCBA type: **Economic and Standard**.
- MSL: 3.
- X-ray inspection: **required**.
- Assembly fixture: required by the JLC part page.
- Public JLC stock observed at the latest check: **0**.
- Minimum shown by the JLC part page: 1.
- Public JLC in-stock pricing shown at the latest check: about USD 12.825 at qty 1 and USD 12.5175 at qty 10+.

Source page:

- https://jlcpcb.com/partdetail/UBLOX-ANNA_B40200B/C6124130

The important conclusion is that **ANNA-B402 passes the Economic PCBA compatibility gate**. The current problem is procurement route / price, not assembly-process incompatibility.

## Preferred sourcing route

### 1. Global Sourcing — preferred

Use JLCPCB Global Sourcing as the first choice for this prototype set.

Reasons:

- Direct JLC public stock is currently insufficient.
- The public JLC / Pre-Order price level is high compared with prices often seen through normal electronics distribution.
- The project only needs a small quantity, so a Global Sourcing quote with a sensible MOQ can be economically attractive even after sourcing-related charges.
- Parts sourced through JLC can still enter the JLC parts inventory and be assembled by JLCPCB, avoiding manual placement of the LGA module.

The actual decision must be based on the **final Global Sourcing checkout quote**, including any additional charge, duty/tariff-related charge, handling, baking if applicable, and lead time. Do not assume that an external distributor's displayed price will be passed through unchanged.

### 2. Pre-Order — fallback

Pre-Order remains technically usable because C6124130 already exists in the JLC parts library, but it is not the preferred path while the quoted/public JLC price is materially higher than the expected Global Sourcing route.

Reconsider Pre-Order if:

- its final price drops substantially,
- Global Sourcing produces an excessive MOQ,
- Global Sourcing lead time becomes unacceptable, or
- the final all-in difference is small enough that the simpler inventory path is worth paying for.

### 3. Consignment / self-purchased parts — last resort

Consignment is not the preferred path for a 15-board prototype because the logistics and service overhead can erase the component-price advantage.

It is also preferable to have JLCPCB place and inspect ANNA-B402 rather than hand-soldering the LGA module.

## Hand assembly decision

Do not plan to hand-solder ANNA-B402 for the production prototype set.

ANNA-B402 is a leadless/LGA-style module with hidden solder joints and JLC explicitly requires X-ray inspection for the part. Home reflow is possible as an experiment, but reliable assembly and fault diagnosis across 15 boards would require more process control than is justified here.

JLCPCB assembly is the baseline.

## X-ray and PCBA cost

X-ray is a real additional cost, but it is part of the required assembly process for C6124130 rather than an optional luxury inspection.

JLCPCB's published Economic PCBA fee structure should be rechecked at order time. The current fee schedule also means that panelization and order grouping can materially change the total cost through setup, panel, stencil, Extended-component, and X-ray quantity bands.

A combined LEFT + RIGHT + NUMPAD mouse-bite panel may therefore be worth quoting, but do not change the electrical design purely to avoid X-ray charges. Compare the final JLC quotes for:

- three separate PCBA orders, and
- one three-design panel order.

## Module-selection status

ANNA-B402 is now the **preferred MCU/module direction** for this branch rather than merely an exploratory candidate.

Technical reasons:

- nRF52833 is supported by Zephyr/ZMK.
- 512 kB flash / 128 kB RAM.
- Native USB device support.
- 33 GPIO gives ample margin for the RIGHT matrix + shared I2C.
- Compact 6.5 x 6.5 mm module.
- Integrated antenna/RF implementation.
- JLCPCB Economic assembly compatibility is confirmed for C6124130.

Alternative modules should remain a fallback rather than an active search unless one of the following fails:

- Global Sourcing price/MOQ/lead time is unacceptable for roughly 18-20 parts.
- ANNA antenna / counterpoise rules cannot be met in the real keyboard PCB geometry.
- ZMK/Zephyr validation exposes an ANNA-specific problem.
- A clearly superior, similarly compact Economic-compatible module becomes available.

## Alternative-module notes

Previously investigated alternatives include Raytac MDBT50Q-family and MinewSemi nRF52840 modules. They remain useful backups but are generally physically larger than ANNA-B402, and exact JLC part numbers can differ in Economic/Standard classification and MOQ.

Do not spend design time migrating to an alternative merely because C6124130 public stock is zero. Global Sourcing is now an accepted procurement path for this low-volume build.

## Final gate before PCB release

Immediately before ordering:

1. Recheck C6124130 PCBA classification and assembly notes.
2. Request / refresh the Global Sourcing quote for ANNA-B402-00B.
3. Confirm final MOQ and the quantity required for 15 placements plus attrition.
4. Confirm total landed sourcing cost and lead time.
5. Confirm X-ray and fixture-related assembly charges in the actual PCBA quote.
6. Recheck the u-blox land pattern against the final KiCad footprint and JLC CPL orientation.
7. Compare separate-board versus combined-panel PCBA totals.

If those checks pass, ANNA-B402 should be treated as the locked module for the first hardware run.
