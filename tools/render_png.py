#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Render ASCII art files to upscaled PNG for visual inspection."""
import sys, zlib, struct

SCALE = 10


def parse(fname):
    rows = []
    for line in open(fname):
        line = line.rstrip("\r\n")
        if not line:
            continue
        rows.append([1 if c == "#" else 0 for c in line])
    return rows


def chunk(tag, data):
    c = struct.pack(">I", len(data)) + tag + data
    c += struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)
    return c


def to_png(rows, scale, path):
    h, w = len(rows), len(rows[0])
    W, H = w * scale, h * scale
    raw = bytearray()
    for y in range(H):
        raw.append(0)
        for x in range(W):
            v = 255 if rows[y // scale][x // scale] else 0
            raw += bytes((v, v, v))
    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", W, H, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(raw), 9))
    png += chunk(b"IEND", b"")
    with open(path, "wb") as f:
        f.write(png)
    print("wrote", path, W, "x", H)


for path in sys.argv[1:]:
    rows = parse(path)
    out = path.rsplit(".", 1)[0] + ".png"
    to_png(rows, SCALE, out)