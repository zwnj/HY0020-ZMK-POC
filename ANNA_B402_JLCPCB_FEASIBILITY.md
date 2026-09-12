# ANNA-B402 JLCPCB sourcing and assembly feasibility

Checked: 2026-09-12

This document tracks the procurement / PCBA gate for using ANNA-B402 on LEFT, RIGHT, and NUMPAD. Detailed electrical/layout integration is tracked in [`ANNA_B402_HARDWARE_INTEGRATION_FEASIBILITY.md`](ANNA_B402_HARDWARE_INTEGRATION_FEASIBILITY.md).

## Required quantity

The project is not a mass-production design.

Planned first build:

- LEFT: 5 boards.
- RIGHT: 5 boards.
- NUMPAD: 5 boards.
- Total installed ANNA-B402 modules: **15**.

Do not reject a sourcing path merely because inventory is not suitable for hundreds of units. The relevant question is whether the current order can reliably source 15 installed parts plus a small assembly margin. A planning quantity around **18-20 modules** is reasonable until the JLC parts calculator / final quote gives the exact attrition requirement.

MOQ should remain appropriate for this prototype scale. A sourcing option that forces purchase of 50-100+ modules is unattractive unless there is an exceptional price or technical reason.

## ANNA-B402-00B / C6124130

JLCPCB listing:

- Manufacturer: u-blox.
- MPN: ANNA-B402-00B.
- JLCPCB part: C6124130.
- JLC classification: Extended.
- Assembly type: SMT Assembly.
- Package: SMD-56P.
- PCBA type shown by JLC: **Economic and Standard**.
- MSL: 3.
- X-ray inspection: **required**.
- Assembly fixture: required by the JLC part page.
- Public stock is insufficient for the planned build and must not be relied upon.
- Minimum shown by JLC: 1.
- Public / Pre-Order pricing is materially higher than normal distributor pricing, so it is not the preferred sourcing route.

Source page:

- https://jlcpcb.com/partdetail/UBLOX-ANNA_B40200B/C6124130

## Important correction: Economic listing does not yet equal process clearance

C6124130 being marked `Economic and Standard` proves that JLC's component library accepts the part for those order types, but a separate process conflict has been identified.

u-blox ANNA-B4 System Integration Manual R08 specifies an **absolute maximum reflow peak of 245 °C** and explicitly prefers a lower peak.

JLCPCB's current PCBA capability table publishes:

- Economic PCBA: **255 ± 5 °C, not adjustable**.
- Standard PCBA: **240 ± 5 °C**.

Therefore this project must **not** state that Economic assembly is fully cleared merely because the part page says Economic compatible.

Before using Economic PCBA, obtain explicit JLCPCB confirmation that C6124130 is processed with a validated thermal profile that keeps ANNA-B402 within the u-blox 245 °C absolute maximum.

If that confirmation is not available, use Standard PCBA rather than changing the MCU module.

JLCPCB also introduced Standard-only medium/low-temperature solder paste options in September 2026. The medium option is listed at 210 ± 5 °C for roughly USD 32 additional per order and can be considered if normal Standard reflow is judged too close to the component maximum.

## Preferred sourcing route

### 1. Global Sourcing — preferred

Use JLCPCB Global Sourcing as the first choice for this prototype set.

Reasons:

- Direct JLC public stock is insufficient.
- JLC / Pre-Order pricing is high compared with normal electronics distribution.
- The project needs only a small quantity.
- JLC handles procurement into its own parts inventory, so loose ANNA modules do **not** need to be shipped from Japan to China.
- JLC can place and X-ray the LGA module as part of PCBA.

The actual decision must be based on the final Global Sourcing checkout quote, including additional charges, duty/tariff-related charges, handling, baking if applicable, and lead time. Do not assume an external distributor's displayed price will be passed through unchanged.

### 2. Pre-Order — fallback

Pre-Order remains technically usable because C6124130 already exists in the JLC parts library, but it is not preferred while its quoted/public price remains much higher than the expected Global Sourcing route.

Reconsider Pre-Order if its final all-in price becomes competitive, Global Sourcing produces a bad MOQ/lead time, or the simplicity is worth the premium.

### 3. Consignment / parts shipped from Japan — not baseline

Do not plan to buy ANNA modules in Japan and consign them to JLCPCB for the first build. International shipping, customs/service overhead, MSL handling, and inventory logistics are unnecessary when Global Sourcing can place the parts directly into JLC's inventory.

Consignment remains an emergency fallback only.

## Hand assembly decision

Do not plan to hand-solder ANNA-B402 for the production prototype set.

ANNA-B402 is an LGA-style module with hidden solder joints. JLC explicitly requires X-ray inspection for C6124130, and u-blox does not recommend hand soldering or rework of the module. Home reflow may be useful for experimentation but is not the production path for 15 boards.

## X-ray and PCBA cost

X-ray is part of the manufacturing requirement for C6124130 rather than an optional luxury inspection.

The BQ27427YZFR fuel-gauge candidate is also a hidden-joint DSBGA part and JLC currently marks it as X-ray required. Final X-ray cost must therefore be taken from the actual PCBA quote rather than estimated from ANNA count alone.

Panelization can change setup, panel, stencil, Extended-component and inspection costs. Quote both:

- three separate PCBA orders, and
- one LEFT + RIGHT + NUMPAD mouse-bite panel order.

Do not alter the electrical design merely to avoid X-ray charges.

## Module-selection status

ANNA-B402 remains the **preferred MCU/module direction**.

Technical reasons:

- nRF52833 / Zephyr / ZMK fit.
- 512 kB flash / 128 kB RAM.
- Native USB device support.
- Ample GPIO for RIGHT matrix + shared I2C.
- Compact 6.5 x 6.5 mm package.
- Integrated antenna/RF implementation.
- JLC has an established C6124130 assembly-library entry.

Alternative modules should remain fallbacks rather than an active search unless one of the following fails:

- Global Sourcing price/MOQ/lead time is unacceptable for roughly 18-20 parts.
- The ANNA antenna reference layout cannot be accommodated in the real keyboard geometry.
- A safe JLC reflow path cannot be confirmed even with Standard PCBA.
- A clearly superior similarly compact module appears.

## Final gate before PCB release

1. Refresh the Global Sourcing quote for ANNA-B402-00B and confirm MOQ / lead time for roughly 18-20 parts.
2. Ask JLCPCB to resolve the C6124130 Economic-process temperature conflict against u-blox TP max 245 °C.
3. If Economic is not explicitly cleared, quote Standard PCBA; consider the Standard-only 210 ± 5 °C medium-temperature paste only if needed.
4. Confirm X-ray / fixture / baking charges in the actual PCBA quote.
5. Lock the u-blox antenna reference geometry and compare the required stack-up against the selected JLC stack-up.
6. Recheck the u-blox land pattern against the final KiCad footprint and JLC CPL orientation.
7. Compare separate-board versus combined-panel PCBA totals.

If those checks pass, ANNA-B402 should be treated as locked for the first hardware run.
