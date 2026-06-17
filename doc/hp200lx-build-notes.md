# HP 200LX Build Notes

This checkout is configured for a CGA-class HP 200LX target by setting
`DEFAULT_VGA_THEME` to `0` in `_config.h`.

## Ubuntu build host

Install prerequisites:

```bash
tools/setup-ubuntu-dosbox.sh
```

Build inside DOSBox:

```bash
tools/build-dosbox.sh
```

The first real-hardware test should be `BUILD/KERNEL.COM` from DOS on the
200LX. It is safer than writing a boot image to a card, and GentleOS supports
returning to DOS with `Shift-Q`.

## Emulator smoke tests

Boot the floppy image in QEMU:

```bash
qemu-system-i386 -m 1 -drive format=raw,if=floppy,file=BUILD/FD1440.IMG -serial stdio
```

Boot the floppy image in DOSBox:

```dos
BOOT BUILD\FD1440.IMG
```

Do not write `BUILD/DISK.IMG` to a physical device until `KERNEL.COM` and an
emulator boot have both behaved as expected.
