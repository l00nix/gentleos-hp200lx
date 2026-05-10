# GentleOS/16

A hobby operating system for vintage 16-bit PCs (80186+).

It's designed as a platform for tinkering with retro hardware, so it
doesn't have most of the features of general-purpose operating systems.
Its goal is to support building user-facing apps with minimal infrastructure.

GentleOS/16 is a sibling of [GentleOS/32](https://github.com/luke8086/gentleos32),
a slightly more advanced 32-bit OS that targets 486+ devices.

## Building

The only prerequisite is [DOSBox](https://www.dosbox.com/), the
entire toolchain (OpenWatcom, NASM, Perl) is included in the repo.

To build GentleOS, start DOSBox from the source directory and run
the following commands:

```dos
Z:\>MOUNT C .
Z:\>C:
C:\>ENV
C:\>COPY _CONFIG.H CONFIG.H
C:\>AUTOGEN
C:\>WMAKE
```

## Running in DOSBox

GentleOS can be booted in DOSBox as a standalone OS:

```dos
C:\>BOOT BUILD\FD1440.IMG
```

## Running in QEMU

Run:

```bash
$ qemu-system-i386 -m 1 -drive format=raw,if=floppy,file=BUILD/FD1440.IMG -serial stdio
```

For audio support on Macs, also add:

```bash
-audiodev coreaudio,id=snd0 -machine pcspk-audiodev=snd0
```

## Running on real devices

> [!WARNING]
> The author takes no responsibility for any lost data
> or damaged hardware. Proceed at your own risk, and only if you
> fully understand what you're doing.

To prepare a bootable HDD/floppy/pendrive, run the following command.
Note it'll **permanently destroy** data on the target disk, and there
will be no confirmation prompt.

```bash
dd if=BUILD/DISK.IMG of=<TARGET DISK> bs=32k conv=fsync status=progress
```

There's no need to copy the entire `FDxxx.IMG` images, they're just
padded with zeros for use in emulators.

Alternatively, if you have [Gotek](https://www.gotekemulator.com/) with
[FlashFloppy](https://github.com/keirf/flashfloppy), you can directly
use `BUILD/FD720.IMG` or `BUILD/FD1440.IMG` as raw disk images.

## Development notes

- The official compiler is [OpenWatcom 1.9](https://www.openwatcom.org/),
  because it can be freely distributed, but
  [Turbo C 2.01](https://duckduckgo.com/?q=Turbo+C+2x)
  is also supported and it's much faster.
  You can install it to `C:\TMP\TC` and use with `WMAKE TC=1`

- For faster compilation, you can try setting `cycles=fixed 99999`
  in the `[cpu]` section of your DOSBox config file

- For a quick turnaround, GentleOS can be started as a COM file
  with `BUILD\KERNEL.COM`. Pressing `Shift-Q` returns back to DOS.

## Attributions

- All images in [vendor/icons8](vendor/icons8) have been sourced from
  [Icons8](https://icons8.com/) using the
  [free license](https://web.archive.org/web/20260325111643/https://icons8.com/license)
  and modified

- All images in [vendor/mona](vendor/mona) have been extracted from the
  [Mona Font](https://github.com/MonadABXY/mona-font) and modified
  ([LICENSE](vendor/mona/LICENSE.txt))

- All fonts in [vendor/int10h](vendor/int10h) have been extracted from the
  [The Ultimate Oldschool PC Font Pack](https://int10h.org/oldschool-pc-fonts/)
  and modified ([LICENSE](vendor/int10h/LICENSE.txt))

- The [Atari Small](vendor/atarism) font by [Tom Fine](https://hea-www.harvard.edu/~fine/)
  has been obtained from https://hea-www.harvard.edu/~fine/Tech/x11fonts.html
  ([LICENSE](vendor/atarism/LICENSE.txt))

- [Font 4x6](vendor/font4x6) by [Luiz Bills](https://www.luizpb.com/) has been
  sourced from https://github.com/luizbills/font4x6
  ([LICENSE](vendor/font4x6/LICENSE.txt))

## License

Except where otherwise noted, GentleOS/16 is licensed under [GPLv2](LICENSE).
