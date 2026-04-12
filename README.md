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
