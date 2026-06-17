# Release 1: HP 200LX KERNEL.COM

This release provides the first tested HP 200LX build of GentleOS/16.

## Included Binary

- File: `KERNEL.COM`
- Size: `46,238` bytes
- SHA-256:
  `e5c70b387a3aaae79f455f48767fccf2610318dc6a6be352c71a197601372e3b`

## Tested Hardware

- HP 200LX Palmtop PC
- Hornet-based 80186-compatible system
- Built-in HP 200LX keyboard
- CGA-compatible LCD using GentleOS CGA mode

## What Changed

GentleOS originally used a direct PC keyboard path: install an INT 09h handler,
read keyboard scan codes from I/O port `60h`, toggle port `61h`, and acknowledge
the PIC directly.

The HP 200LX internal keyboard is handled differently by the Hornet ASIC and HP
BIOS. The BIOS scans and debounces the keyboard using Hornet IRQ2/TIMER1, then
delivers PC-style keyboard events through BIOS keyboard services.

This release keeps the BIOS INT 09h path intact on HP 200LX builds and polls
BIOS INT 16h extended keyboard services from the GentleOS event loop. Returned
BIOS scan codes and modifier state are converted into GentleOS key events.

The full source diff is in `doc/hp200lx-change.diff`.

## Running

The easiest way to test is to copy `KERNEL.COM` to the HP 200LX and run `KERNEL.COM`.

When running as `KERNEL.COM`, GentleOS can return to DOS with `Shift-Q`.
