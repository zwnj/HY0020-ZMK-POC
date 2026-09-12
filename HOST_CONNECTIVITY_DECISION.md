# Host Connectivity Decision

Updated: 2026-09-12

This document records the current host-connectivity decision for the split keyboard project. It supersedes the earlier assumption that a USB dongle would not be used.

## Goal

The actual user requirement is:

- When using a laptop, connect directly from the keyboard without requiring a dongle.
- When using a desktop, allow a dedicated Prospector-like USB dongle to be used.
- Keep the keyboard architecture and ZMK integration as simple and maintainable as practical.
- Do not introduce a proprietary 2.4 GHz transport unless it provides a clear benefit over the BLE solution.

Low-latency ESB itself is not a requirement. The dongle experience is the requirement.

## Selected architecture

LEFT remains the permanent ZMK Split Central in all normal operating modes.

```text
RIGHT
  |
  | ZMK standard BLE Split
  v
LEFT / permanent ZMK Central
  |
  +-- USB HID ------------------------------> host PC
  |
  +-- BLE HID profile 0 --------------------> laptop
  |
  +-- BLE HID profile 1 --------------------> dedicated desktop dongle
  |                                             |
  |                                             +-- USB HID --> desktop PC
  |
  +-- BLE HID profile 2... -----------------> other hosts
```

The dedicated desktop dongle is not a ZMK Split Central. It acts as a BLE HID Central / USB HID bridge.

This avoids dynamic Central role switching between LEFT and the dongle.

## Split transport

RIGHT -> LEFT remains standard ZMK BLE Split.

Reasons:

- It is the normal and well-supported ZMK split topology.
- Both battery-powered halves can use BLE connection scheduling and sleep efficiently.
- It avoids making LEFT an always-listening ESB PRX.
- It avoids a custom split transport and minimizes ZMK-specific maintenance.
- The same split topology remains active whether the host is a laptop, a desktop dongle, or a direct USB connection.

## Desktop dongle

The preferred starting point for the desktop dongle is a Nordic/Zephyr BLE HID Central -> USB HID bridge implementation, with Nordic nRF Desktop as a candidate reference/base.

From the keyboard's point of view, the dongle should behave like an ordinary BLE host. Desktop selection should therefore be handled as a normal ZMK BLE profile selection rather than as a topology or Central-role change.

The intended user flow is approximately:

```text
Laptop:
    select laptop BLE profile
    LEFT -> BLE HID -> laptop

Desktop:
    select dongle BLE profile
    LEFT -> BLE HID -> dongle -> USB HID -> desktop
```

LEFT may still support direct USB HID independently of the dongle path.

## Prospector-like display/status

A Prospector-like display on the desktop dongle remains desirable, but it must not force the dongle to become the ZMK Central.

Standard BLE HID is sufficient for keyboard input. If the dongle display later needs ZMK-internal status that is not naturally exposed through HID, such as:

- active layer,
- RIGHT battery state,
- dock state,
- RGB/status state,

then add the smallest practical status channel separately. A small custom BLE GATT service is the preferred direction before considering a proprietary transport.

This status extension is optional and should be developed only after the basic BLE HID -> USB HID bridge is working.

## ESB decision

Nordic Enhanced ShockBurst (ESB) was investigated because existing ZMK community projects demonstrate both ESB split transport and ESB HID endpoint/dongle concepts.

ESB is technically viable, and remains an option for future experimentation, especially if there is a later requirement for very low latency or a proprietary 2.4 GHz link.

It is not part of the current baseline because using ESB broadly would add complexity without solving a requirement that BLE cannot already satisfy.

In particular:

- ESB split transport is outside standard ZMK.
- BLE + ESB coexistence on one radio requires additional multiprotocol integration.
- An ESB PRX role on a battery-powered LEFT can increase power consumption substantially compared with a scheduled BLE link.
- Current community ESB integrations have version-specific ZMK/NCS maintenance requirements.
- The desired desktop dongle experience can instead be achieved while leaving the keyboard itself on standard BLE.

Do not implement ESB unless later measurements or requirements show a concrete advantage worth the additional maintenance.

## Explicit non-goals

For the current baseline, do not implement:

- dynamic ZMK Central election,
- runtime LEFT <-> dongle Central role switching,
- runtime BLE Split <-> ESB Split switching,
- pogo-pin key-data transport,
- a custom wired split protocol,
- ESB merely for protocol uniformity.

## Numpad

NUMPAD remains a standalone ZMK keyboard rather than a third Split Peripheral.

Its host connectivity can continue to use USB HID and/or BLE HID independently. If desktop operation through the dedicated dongle later needs the NUMPAD as well, treat that as a separate dongle multi-device requirement rather than changing the LEFT/RIGHT split topology.

## Validation before committing to dongle hardware

Before designing a custom dongle PCB, validate the following in firmware using development boards:

1. LEFT/RIGHT standard ZMK BLE Split remains stable with the target ANNA-B402/nRF52833 configuration.
2. LEFT can switch cleanly between a normal laptop BLE profile and the BLE dongle profile.
3. A BLE HID Central -> USB HID bridge can forward all required keyboard and consumer reports correctly.
4. Reconnect behavior after dongle or keyboard power cycling is acceptable.
5. If a display is required, determine exactly which status fields are missing from standard HID/BAS and therefore need an additional GATT service.
6. Measure end-to-end latency before considering ESB as an optimization.

## Decision summary

Use BLE as the default radio transport throughout the keyboard:

- RIGHT -> LEFT: standard ZMK BLE Split.
- LEFT -> laptop/other wireless hosts: standard ZMK BLE HID.
- LEFT -> desktop dongle: standard BLE HID.
- dongle -> desktop: USB HID.

Keep LEFT permanently as the ZMK Central. Treat ESB as a future optimization/experiment, not as a baseline requirement.
