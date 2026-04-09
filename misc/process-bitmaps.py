#!/usr/bin/env python3

import re
import os
import glob

FONTS = [
    {
        "path": "fonts/evxme58.pbm",
        "name": "Evx ME 5x8",
        "width": 5,
        "height": 8,
        "pitch": 8,
    },
    {
        "path": "fonts/atarism.pbm",
        "name": "Atari Small 4x8",
        "width": 4,
        "height": 8,
        "pitch": 4,
    },
]

FONT_MAX_CHARS = 128


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


def process_bitmap(path):
    print(f"Processing bitmap: {path}")

    name = os.path.splitext(os.path.basename(path))[0]
    pixels = load_pbm(path)
    width = len(pixels[0])
    height = len(pixels)
    pitch = (width + 7) // 8

    pixel_lines = []

    for row in pixels:
        bytez = []
        for i in range(0, len(row), 8):
            byte = 0
            for bit_pos in range(8):
                if i + bit_pos < len(row):
                    byte |= row[i + bit_pos] << (7 - bit_pos)
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


def process_bitmaps():
    bitmap_files = sorted(glob.glob("bitmaps/*"))
    bitmaps = (process_bitmap(x) for x in bitmap_files)

    lines = [
        '#include <gui.h>',
        '',
        *bitmaps,
    ]

    with open("data/data_bmp.c", "w") as f:
        f.write("\r\n".join(lines))


def load_font(font):
    path = font["path"]
    name = font["name"]
    width = font["width"]
    height = font["height"]
    pitch = font["pitch"]

    print(f"Processing font: {path}")
    pixels = load_pbm(path)
    img_height = len(pixels)
    img_width = len(pixels[0])

    cols = img_width // pitch
    rows = img_height // height
    num_chars = cols * rows

    print(f"  image: {img_width}x{img_height}, grid: {cols}x{rows} = {num_chars} chars")

    num_chars = min(num_chars, FONT_MAX_CHARS)
    max_bytes = FONT_MAX_CHARS * height

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
                p = pixels[y][x]
                byte |= p << (7 - i)
            glyph_bytes.append(byte)

    while len(glyph_bytes) < max_bytes:
        glyph_bytes.append(0x00)

    return glyph_bytes[:max_bytes]


def format_font_pixels(glyph_bytes, height):
    lines = []
    num_chars = len(glyph_bytes) // height

    for i in range(num_chars):
        offset = i * height
        chunk = glyph_bytes[offset:offset + height]
        hex_str = "".join(f"\\x{b:02x}" for b in chunk)
        lines.append(f'            "{hex_str}" \\')

    return "\r\n".join(lines)


def process_fonts():
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
            format_font_pixels(glyph_bytes, height),
            f"    }},",
        ]

    lines.append("};")
    lines.append("")

    with open("data/data_fnt.c", "w") as f:
        f.write("\r\n".join(lines))


def main():
    process_bitmaps()
    process_fonts()


if __name__ == "__main__":
    main()
