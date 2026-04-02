#!/usr/bin/env python3

FONTS = [
    {
        "path": "vendor/fonts/EverexME_5x8-8.png",
        "name": "EverexME_5x8",
        "width": 5,
        "height": 8,
        "pitch": 8,
    }
]

MAX_CHARS = 128


import re
import os
import glob

from PIL import Image


def load_font(font):
    path = font["path"]
    name = font["name"]
    width = font["width"]
    height = font["height"]
    pitch = font["pitch"]

    print(f"Loading font: {path}")
    img = Image.open(path).convert("RGB")
    img_width, img_height = img.size
    data = img.load()

    cols = img_width // pitch
    rows = img_height // height
    num_chars = cols * rows

    print(f"  Image: {img_width}x{img_height}, grid: {cols}x{rows} = {num_chars} chars")

    num_chars = min(num_chars, MAX_CHARS)
    max_bytes = MAX_CHARS * height

    glyph_bytes = []
    for ch in range(num_chars):
        col = ch % cols
        row = ch // cols
        x_start = col * pitch
        y_start = row * height

        for j in range(height):
            byte = 0
            for i in range(pitch):
                x = x_start + i
                y = y_start + j
                r, g, b = data[x, y]
                if (r << 16) | (g << 8) | b == 0:
                    byte |= 1 << (7 - i)
            glyph_bytes.append(byte)

    while len(glyph_bytes) < max_bytes:
        glyph_bytes.append(0x00)

    return glyph_bytes[:max_bytes]


def format_pixels(glyph_bytes, height):
    lines = []
    num_chars = len(glyph_bytes) // height

    for i in range(num_chars):
        offset = i * height
        chunk = glyph_bytes[offset:offset + height]
        hex_str = "".join(f"\\x{b:02x}" for b in chunk)
        lines.append(f'            "{hex_str}" \\')

    return "\r\n".join(lines)


def main():
    font_data = []
    for font in FONTS:
        glyph_bytes = load_font(font)
        font_data.append((font, glyph_bytes))

    lines = [
        "#include <gui.h>",
        "",
        f"font_st fonts[] = {{",
    ]

    for font, glyph_bytes in font_data:
        name = font["name"]
        width = font["width"]
        height = font["height"]
        lines += [
            f"    {{",
            f"        {{ {width}, {height} }},",
            f'        "{name}",',
            f"        (uint8_t *)",
            format_pixels(glyph_bytes, height),
            f"    }},",
        ]

    lines.append("};")
    lines.append("")

    with open("data/data_fnt.c", "w") as f:
        f.write("\r\n".join(lines))

if __name__ == "__main__":
    main()
