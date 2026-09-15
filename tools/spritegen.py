#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
RoboSnoopy sprite generator
===========================
Converts ASCII pixel-art files ('.' or ' ' = transparent, '#' = lit) into
bit-packed PROGMEM byte arrays suitable for Adafruit_GFX drawBitmap(), and
prints a composited preview of all frames so the art can be eyeballed.

Usage:
  python3 spritegen.py <artfile.txt ...>            # one or more art files
  python3 spritegen.py --compose <body> <face> <fx> <fy>  # preview face overlay

Art file format:
  - Plain text, every line must have identical length (the width).
  - '#'   -> pixel is drawn (white fur / face feature)
  - '.'   -> transparent / background
  - ' '   -> same as '.'
  - Lines starting with '# ' or '//' are comments and are ignored.

Output:
  stdout C array snippet.  Use  > out.h  to capture.
"""

import sys
import struct


def parse(fname):
    rows = []
    width = None
    for line in open(fname, "r", encoding="utf-8"):
        line = line.rstrip("\r\n")
        if not line:
            continue
        stripped = line.lstrip()
        if stripped.startswith("#") and not stripped.startswith("#" + ".") and not stripped.startswith("#" + " "):
            # comment only if the very first cell is '#' AND it's not pixel art
            if stripped.startswith("##") or stripped.startswith("# "):
                continue
        if width is None:
            width = len(line)
        else:
            assert len(line) == width, "Row length mismatch in %s: %d != %d\n%s" % (fname, len(line), width, line)
        rows.append([1 if c == "#" else 0 for c in line])
    assert rows, "empty art file: %s" % fname
    return rows, width


def pack(rows, width, height):
    nbytes_row = (width + 7) // 8
    out = bytearray()
    for r in range(height):
        row = rows[r]
        for b in range(nbytes_row):
            byte = 0
            for bit in range(8):
                col = b * 8 + bit
                if col < width and row[col]:
                    byte |= (0x80 >> bit)
            out.append(byte)
    return out


def roundtrip_check(rows, width, height, packed):
    nw = nbytes_row = (width + 7) // 8
    ok = True
    for r in range(height):
        for c in range(width):
            byte = packed[r * nw + (c // 8)]
            bit = (byte >> (7 - (c % 8))) & 1
            if bit != rows[r][c]:
                ok = False
                print("round-trip mismatch at (%d,%d)" % (c, r))
    return ok


def mirror_pack(rows, width, height):
    """Horizontal mirror of a packed bitmap (byte order + bit order reversed)."""
    nw = (width + 7) // 8
    src = pack(rows, width, height)
    REV = [int("{0:08b}".format(b)[::-1], 2) for b in range(256)]
    out = bytearray()
    for r in range(height):
        for b in range(nw):
            out.append(REV[src[r * nw + (nw - 1 - b)]])
    return out


def render(rows, width, height):
    out = ""
    for r in range(height):
        out += "".join("#" if p else "." for p in rows[r]) + "\n"
    return out


def c_array(name, rows, width, height):
    nw = (width + 7) // 8
    packed = pack(rows, width, height)
    lines = []
    for i in range(0, len(packed), 12):
        chunk = packed[i:i + 12]
        lines.append("  " + ", ".join("0x%02X" % b for b in chunk) + ",")
    arr = "\n".join(lines)
    return (
        "// %s - %dx%d, %d bytes\n"
        "static const uint8_t %s[%d] PROGMEM = {\n%s\n};\n\n"
        % (name, width, height, len(packed), name, len(packed), arr)
    )


def c_array_mirror(name, rows, width, height):
    nw = (width + 7) // 8
    packed = mirror_pack(rows, width, height)
    lines = []
    for i in range(0, len(packed), 12):
        chunk = packed[i:i + 12]
        lines.append("  " + ", ".join("0x%02X" % b for b in chunk) + ",")
    arr = "\n".join(lines)
    return (
        "// %s_mirror - %dx%d, %d bytes (horizontally flipped)\n"
        "static const uint8_t %s_mirror[%d] PROGMEM = {\n%s\n};\n\n"
        % (name, width, height, len(packed), name, len(packed), arr)
    )


def compose_preview(body_rows, face_rows, fx, fy, body_w, body_h, face_w, face_h):
    """Composite a black-feature face bitmap (1=punch) onto a white body."""
    out = [list(row) for row in body_rows]
    for y in range(face_h):
        by = fy + y
        if by < 0 or by >= body_h:
            continue
        for x in range(face_w):
            bx = fx + x
            if bx < 0 or bx >= body_w:
                continue
            if face_rows[y][x]:
                out[by][bx] = 0  # punch a black feature into the white body
    return out


def main(argv):
    if not argv or argv[0] == "--help":
        print(__doc__)
        return 0

    if argv[0] == "--compose":
        body_f, face_f, fx, fy = argv[1], argv[2], int(argv[3]), int(argv[4])
        body, bw = parse(body_f)
        face, fw = parse(face_f)
        bh, fh = len(body), len(face)
        comp = compose_preview(body, face, fx, fy, bw, bh, fw, fh)
        print(render(comp, bw, bh))
        return 0

    ok = True
    emit_mirror = "--mirror" in argv
    for fname in argv:
        if fname == "--mirror":
            continue
        rows, width = parse(fname)
        height = len(rows)
        packed = pack(rows, width, height)
        if not roundtrip_check(rows, width, height, packed):
            ok = False
        name = fname.split("/")[-1].rsplit(".", 1)[0]
        print(c_array(name, rows, width, height))
        if emit_mirror:
            print(c_array_mirror(name, rows, width, height))
    print("// round-trip: %s" % ("OK" if ok else "FAILED"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))