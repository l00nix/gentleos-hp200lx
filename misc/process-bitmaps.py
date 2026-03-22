#!/usr/bin/env python3

import re
import os
import glob

from PIL import Image

def load_pixels(path):
    img = Image.open(path).convert("RGB")
    width, height = img.size
    data = img.load()

    rows = []
    for y in range(height):
        row = []
        for x in range(width):
            r, g, b = data[x, y]
            row.append((r << 16) | (g << 8) | b)
        rows.append(row)

    return rows

def load_image_1bpp(path):
    print(f"Loading image (1bpp): {path}")
    name = os.path.splitext(os.path.basename(path))[0]
    pixels = load_pixels(path)
    width = len(pixels[0])
    height = len(pixels)
    pitch = (width + 7) // 8

    pixel_lines = []

    for row in pixels:
        bits = []
        for x in row:
            bits.append(1 if x == 0 else 0)

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
        f"    .size = {{ .width = {width}, .height = {height} }},",
        f"    .pitch = {pitch},",
        f"    .pixels = (uint8_t *)",
        *pixel_lines,
        f"}};",
        "",
    ]

    return("\n".join(lines))

def load_image(path):
    name = os.path.splitext(os.path.basename(path))[0]
    return load_image_1bpp(path)

def main():
    bitmap_files = sorted(glob.glob("bitmaps/*.bmp"))
    bitmaps = (load_image(x) for x in bitmap_files)

    lines = [
        '#include <gui.h>',
        '',
        *bitmaps,
    ]

    with open("data/data_bitmaps.c", "w") as f:
        f.write("\n".join(lines))

if __name__ == "__main__":
    main()
