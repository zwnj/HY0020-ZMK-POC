# Validation status

Updated: 2026-09-12

This file tracks validation for the `poc/anna-b402` branch. HY0020-specific build criteria are historical and should not be used as the go/no-go gate for this branch.

## Statically checked design facts

- Global matrix transform: **73 entries**.
- LEFT physical matrix: **5 x 6 = 30 switches / 11 GPIO**.
- RIGHT physical matrix: **5 x 9 capacity, 43 switches / 14 GPIO**.
- RIGHT global column offset: **6**.
- Physical-layout entries for Studio: **73**.
- Base-layer bindings: **73**.
- Function-layer bindings: **73**.
- LEFT role: permanent compile-time ZMK Split Central.
- RIGHT role: ZMK Split Peripheral over standard BLE.
- NUMPAD role: standalone keyboard, not a third Split Peripheral.
- Desktop dongle does not become the ZMK Central; see `HOST_CONNECTIVITY_DECISION.md`.

## ANNA-B402 board scaffold

The branch contains `boards/arm/anna_b402/` with:

- nRF52833 SoC definition.
- SRAM / flash chosen nodes.
- GPIO0 and GPIO1 enabled.
- I2C enabled.
- Native nRF52833 USB device controller enabled.
- Current I2C pinctrl using P0.11 SDA / P0.12 SCL.
- Direct-SWD PoC flash partitioning.

These files are scaffolding and still require actual ZMK build validation before they are treated as a production board definition.

## Known migration gap

`build.yaml` still contains the old HY0020 / nRF52832 jobs. This is an expected migration task, not evidence that ANNA-B402 is unsupported.

The earlier failure where an HY0020 build saw an ANNA-oriented shield overlay using `gpio1` is a mixed-target CI problem. Do not use HY0020 jobs as the acceptance test for the ANNA branch.

## Required CI migration

Replace or separate the old jobs so the ANNA branch explicitly builds `anna_b402`.

Required initial jobs:

1. LEFT on `anna_b402` with BLE HID + BLE Split Central + ZMK Studio + USB HID.
2. RIGHT on `anna_b402` with BLE Split Peripheral.
3. NUMPAD on `anna_b402` as a standalone USB/BLE keyboard once its shield configuration is ready.

Keep a legacy HY0020 job only if it is intentionally testing the historical branch/configuration; it must not share ANNA-only overlays that require `gpio1`.

## Go/no-go criteria for ANNA firmware

### LEFT

Must compile with:

- ZMK Split Central.
- BLE HID host output.
- USB HID host output.
- ZMK Studio.
- Battery reporting baseline.

Record final flash and RAM usage from the linker summary.

The important memory criterion is meaningful margin rather than merely fitting. ANNA-B402 provides 128 kB RAM, so the old nRF52832 result near 57 kB / 64 kB should no longer be close to the limit, but this must be measured on the actual ANNA build.

### RIGHT

Must compile with:

- standard ZMK BLE Split Peripheral,
- 5 x 9 / 43-key matrix mapping,
- shared I2C path available for fuel gauge / RGB,
- battery reporting.

RIGHT RGB is mandatory in the final design, but bring-up may validate the base split first and add the RGB peripheral immediately afterward.

### NUMPAD

Must compile as a standalone ZMK keyboard with:

- BLE HID,
- USB HID,
- its own keymap,
- ZMK Studio if memory / configuration remains practical.

## Low-frequency clock validation

The preferred final hardware direction is an external 32.768 kHz LFXO on ANNA-B402.

Before freezing the board definition:

- select the actual 32.768 kHz crystal,
- confirm the correct Zephyr LFCLK source configuration,
- confirm P0.00 / P0.01 are not used elsewhere,
- verify final load-capacitor values against the crystal and u-blox reference design.

Internal LFRC may be used temporarily during firmware bring-up if that makes early testing easier, but the final low-power measurements should use the selected LFXO configuration.

## Peripheral integration sequence

After the base ANNA LEFT/RIGHT builds pass, add peripherals incrementally so memory and power impact remain attributable:

1. BQ27427 fuel gauge through standard Zephyr sensor support.
2. PCA9633-family RGB/status driver through standard Zephyr LED support.
3. Thin custom status policy only if needed.
4. Dock-detect inputs / status behavior.

Avoid ZMK core patches for these features.

## Host-connectivity validation

Before committing to a custom desktop dongle PCB:

1. Confirm LEFT/RIGHT standard BLE Split stability on ANNA-B402/nRF52833.
2. Confirm LEFT can switch between a normal laptop BLE profile and a dedicated dongle BLE profile.
3. Confirm direct LEFT USB HID works independently of BLE profiles.
4. Prototype a BLE HID Central -> USB HID bridge using development hardware.
5. Confirm keyboard and consumer-control reports forward correctly.
6. Test reconnection after keyboard and dongle power cycles.
7. Measure end-to-end latency before deciding whether ESB is worth revisiting.
8. If a Prospector-like display is desired, identify the exact information not already available through standard HID / Battery Service before adding a custom GATT status service.

## Hardware / manufacturing validation

Before PCB release:

- Validate the ANNA-B402 land pattern against the latest u-blox documents.
- Validate antenna keep-out / counterpoise constraints in LEFT, RIGHT, and NUMPAD layouts.
- Confirm USB routing / ESD design for LEFT and NUMPAD.
- Confirm nPM1100 / battery / NTC values.
- Confirm final GPIO assignment avoids RESET/SWD/LFCLK/RF-sensitive conflicts.
- Refresh the Global Sourcing quote for roughly 15 installed modules plus assembly attrition.
- Confirm Economic PCBA, X-ray, fixture, and panelization costs in the actual JLCPCB quote.

## Current acceptance state

The architecture and ANNA-B402 direction are selected, but the branch is **not yet firmware-validated** because CI still needs to be migrated from the HY0020 jobs to explicit `anna_b402` builds.

Do not interpret the presence of the board scaffold as a successful compile result. The next meaningful milestone is a clean ANNA LEFT + RIGHT CI build with memory numbers recorded.
