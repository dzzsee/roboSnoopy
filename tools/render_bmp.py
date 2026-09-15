#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Render ASCII art files to upscaled BMP for visual inspection."""
import sys, struct

SCALE = 10


def parse(fname):
    rows = []
    for line in open(fname):
        line = line.rstrip("\r\n")
        if not line:
            continue
        rows.append([1 if c == "#" else 0 for c in line])
    return rows


def to_bmp(rows, scale, path):
    h, w = len(rows), len(rows[0])
    W, H = w * scale, h * scale
    row_size = ((32 * W + 31) // 32) * 4
    img_size = row_size * H
    file_size = 14 + 40 + img_size
    data = bytearray()
    # 32-bit BGRA pixel data
    px = [[(255, 255, 255) if rows[y][x] else (0, 0, 0) for x in range(w)] for y in range(h)]
    padded = b"".join(
        b"".join(struct.pack("<BBBx", px[y // scale][x // scale][2],
                             px[y // scale][x // scale][1], px[y // scale][x // scale][0])
                 for x in range(W)) + b"\x00" * (row_size - W * 4)
        for y in range(H)
    )
    header = struct.pack("<2sIHHI", b"BM", file_size, 0, 0, 54)
    dib = struct.pack("<IiiHHIIiiII", 40, W, H, 1, 32, 0, img_size, 0, 0, 0, 0)
    with open(path, "wb") as f:
        f.write(header + dib + padded)
    print("wrote", path, W, "x", H)


for path in sys.argv[1:]:
    rows = parse(path)
    out = path.rsplit(".", 1)[0] + ".bmp"
    to_bmp(rows, SCALE, out)