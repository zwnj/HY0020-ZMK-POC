# Validation status

## Statically checked in this bundle

- Global matrix transform: **73 entries**
- LEFT physical matrix: **5 x 6 = 30 switches / 11 GPIO**
- RIGHT physical matrix: **5 x 9 capacity, 43 switches / 14 GPIO**
- RIGHT global column offset: **6**
- Physical-layout entries for Studio: **73**
- Base-layer bindings: **73**
- Function-layer bindings: **73**
- LEFT role: compile-time ZMK split central
- RIGHT role: ZMK split peripheral
- Studio: enabled only in the LEFT build
- Studio transport: BLE; no USB Studio snippet is used

## Why actual RAM/flash numbers are not in this bundle yet

This execution environment does not contain a ZMK/Zephyr west workspace and cannot directly clone GitHub repositories. No user ZMK GitHub repository is connected here either, so the official GitHub Actions build cannot be launched from this session.

The bundle is therefore CI-ready but **not falsely marked as compiled**.

## Go/no-go criteria for the first CI build

1. Both nRF52 DK reference jobs must compile.
2. Both HY0020 jobs should compile; if only these fail, fix the board definition first.
3. Record LEFT Studio RAM and flash usage from the link summary.
4. Prefer meaningful RAM margin rather than merely fitting. The Studio RPC thread alone defaults to a 4096-byte stack, so a build at ~95% RAM would not be considered comfortable even if it links.
5. Only after this baseline passes should fuel gauge and RGB be added, one feature at a time, and memory compared after each step.
