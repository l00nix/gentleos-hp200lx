#!/usr/bin/env python3

import re
import os
import glob

def load_pbm(path):
    with open(path, "r") as fp:
        header = []
        raster_chars = []
        for line in fp:
            line = line.split("#", 1)[0]
            if len(header) < 3:
                parts = line.split()
                while parts and len(header) < 3:
                    header.append(parts.pop(0))
                raster_chars.extend(parts)
            else:
                raster_chars.append(line)

    if len(header) < 3 or header[0] != "P1":
        raise ValueError(f"Not a P1 PBM file: {path}")

    width = int(header[1])
    height = int(header[2])
    flat_pixels = [int(c) for c in "".join(raster_chars) if not c.isspace()]
    pixels = [flat_pixels[i:i + width] for i in range(0, len(flat_pixels), width)]

    return pixels

def process_image(path):
    print(f"Processing image: {path}")

    name = os.path.splitext(os.path.basename(path))[0]
    pixels = load_pbm(path)
    width = len(pixels[0])
    height = len(pixels)
    pitch = (width + 7) // 8

    pixel_lines = []

    for row in pixels:
        bits = []
        for x in row:
            bits.append(x)

        bytez = []
        for i in range(0, len(bits), 8):
            byte = 0
            for bit_pos in range(8):
                if i + bit_pos < len(bits):
                    byte |= bits[i + bit_pos] << (7 - bit_pos)
            bytez.append(byte)

        pixel_str = "".join(f"\\x{byte:02x}" for byte in bytez)
        pixel_lines.append(f'        "{pixel_str}" \\')

    lines = [
        f"bitmap_st bitmap_{name} = {{",
        f"    {{ {width}, {height} }},",
        f"    {pitch},",
        f"    (uint8_t *)",
        *pixel_lines,
        f"}};",
        "",
    ]

    return("\r\n".join(lines))

def main():
    image_files = sorted(glob.glob("bitmaps/*"))
    bitmaps = (process_image(x) for x in image_files)

    lines = [
        '#include <gui.h>',
        '',
        *bitmaps,
    ]

    with open("data/data_bmp.c", "w") as f:
        f.write("\r\n".join(lines))

if __name__ == "__main__":
    main()
