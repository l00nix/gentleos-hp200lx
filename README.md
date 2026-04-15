# gentleOS

TODO: A hobby operating system for vintage 16-bit PCs

## Requirements

### Building

TODO

### Running

- An i486-capable emulator (preferably QEMU)
- An i486+ PC with BIOS (no guarantees, try at your own risk)

### Development

TODO

## Building

TODO

## Testing ###

### QEMU

Run:

```bash
qemu-system-i386 -drive format=raw,file=build/disk.img -m 8 -debugcon stdio
```

For audio support on Macs, also add:

```
-audiodev coreaudio,id=snd0 -machine pcspk-audiodev=snd0
```

### Real hardware

If your device is capable of USB boot, just write the image file
to an empty pendrive:

```bash
dd if=build/disk.img of=<YOUR PENDRIVE> bs=1M conv=fsync
```

Otherwise, if you have GRUB installed, you can point it directly to
the kernel.elf file (see misc/grub.cfg)


## Attributions

- Assets in [vendor/icons8](vendor/icons8) have been sourced from
  [Icons8](https://icons8.com/) using the
  [free license](https://web.archive.org/web/20260325111643/https://icons8.com/license)
  and modified

- Assets in [vendor/mona](vendor/mona) have been extracted from the
  [Mona Font](https://github.com/MonadABXY/mona-font) and modified

- Assets in [vendor/int10h](vendor/int10h) have been extracted from the
  [The Ultimate Oldschool PC Font Pack](https://int10h.org/oldschool-pc-fonts/)
  and modified

- The [Atari Small](vendor/atarism) font by [Tom Fine](https://hea-www.harvard.edu/~fine/)
  has been obtained from https://hea-www.harvard.edu/~fine/Tech/x11fonts.html

- [Font 4x6](vendor/font4x6) by [Luiz Bills](https://www.luizpb.com/) has been
  sourced from https://github.com/luizbills/font4x6
