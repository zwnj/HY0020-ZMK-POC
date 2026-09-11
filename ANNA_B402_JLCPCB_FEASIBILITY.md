# ANNA-B402 JLCPCB Economic feasibility

Checked 2026-09-11.

## ANNA-B402-00B / C6124130

- JLCPCB part: C6124130
- JLCPCB PCBA type: Economic and Standard
- Assembly type: SMT
- Package: SMD-56P
- X-ray inspection: required
- Assembly fixture: required by JLCPCB part page
- JLC/LCSC public stock observed: 0
- JLC listed unit price while stocked: about USD 12.8 qty 1
- External distributor availability is substantially better (for example DigiKey/Mouser listings showed stock), so global sourcing / pre-order / consignment remain possible paths.

Conclusion: ANNA-B402 is technically compatible with Economic PCBA, but it is not currently a low-risk public-stock choice at JLCPCB/LCSC.

## Comparison candidates checked

### MinewSemi MS88SF31 / nRF52840

- Some JLCPCB Assembly listings (for example C9900097622) are marked Economic and Standard.
- Public stock observed: 0.
- Another manufacturer-specific MS88SF3/nRF52840 listing (C20416747) is Standard Only, showing that exact JLC part number matters.

### Raytac MDBT50Q / nRF52840

- C5118826 (MDBT50Q-1MV2) is marked Economic and Standard.
- Public stock observed on checked listings: 0.
- Larger than ANNA-B402.

### Seeed 113990583 / 113990584 (MDBT50Q family)

- C608478 / C608479.
- nRF52840, USB capable, Economic-compatible listings exist.
- Public JLC stock observed: 0.
- External distributor stock exists, so sourcing is possible but not turnkey from JLC public inventory.

## Manufacturing implication

For the current keyboard project, distinguish two requirements:

1. **Economic assembly compatibility**: ANNA-B402 passes.
2. **Stable public JLC stock**: ANNA-B402 currently fails.

JLCPCB documents that public stock parts, global-sourcing parts, pre-order parts, and consigned parts can all be used for PCBA once the parts are received into the appropriate inventory. For a prototype, pre-order/global sourcing is therefore workable, but for a repeatable design it adds procurement risk and lead time.

## Current recommendation

Keep ANNA-B402 as the leading technical candidate, but do not lock it as the production MCU module until one of the following is true:

- JLC public stock becomes healthy, or
- a JLC pre-order/global-sourcing route is tested successfully and considered acceptable, or
- a second footprint-compatible / design-compatible supply strategy is defined.

Exact JLC part numbers must be rechecked immediately before PCB release because PCBA type and stock can differ between otherwise similar module listings.
