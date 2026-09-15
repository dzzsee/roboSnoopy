#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Drafts Snoopy profile pixel-art as ASCII files for spritegen.py.
Two walk frames are produced (legs alternate).  Run, then preview with
`python3 tools/spritegen.py tools/art/snoopy_frame1.txt` and tweak here.

Convention: '#' = white fur (drawn with MAINCOLOR), '.' = background / black
feature (stays off = BGCOLOR).  The ear and nose are black features, i.e.
holes in the white fur.
"""
import os

W, H = 48, 32
OUT = os.path.join(os.path.dirname(__file__), "art")


def new_canvas():
    return [["."] * W for _ in range(H)]


def rect(g, x0, y0, x1, y1, ch="#"):
    for y in range(max(0, y0), min(H, y1 + 1)):
        for x in range(max(0, x0), min(W, x1 + 1)):
            g[y][x] = ch


def ellipse(g, cx, cy, rx, ry, ch="#"):
    for y in range(max(0, cy - ry), min(H, cy + ry + 1)):
        for x in range(max(0, cx - rx), min(W, cx + rx + 1)):
            ex = (x - cx) / rx
            ey = (y - cy) / ry
            if ex * ex + ey * ey <= 1.0:
                g[y][x] = ch


def draw_base(g, legs):
    """legs = list of (grow, foot_dir) both legs at base columns [BODY]. """
    # TAIL - small curl rising at the rear (white)
    ellipse(g, 5, 15, 3, 3, "#")      # tail base
    ellipse(g, 5, 9, 2, 2, "#")       # tail tip curl
    # BODY - long, low horizontal mass
    ellipse(g, 15, 19, 12, 6, "#")    # rows13..25, cols3..27
    # LEGS - chunky vertical bars; a leg can be planted (7 tall) or lifted (5 tall)
    # legs spec: (rear_lift, rear_foot, front_lift, front_foot)
    (rear_lift, rear_foot, front_lift, front_foot) = legs
    # rear leg near cols 9..11, front leg near cols 18..20
    for lift, foot, base in ((rear_lift, rear_foot, 9), (front_lift, front_foot, 18)):
        x0, x1 = base, base + 2
        bottom = 29 if lift else 31
        rect(g, x0, 25, x1, bottom, "#")
        if foot:
            if foot < 0:
                rect(g, x0 + foot, bottom, x1, bottom, "#")   # foot back
            else:
                rect(g, x0, bottom, x1 + foot, bottom, "#")   # foot forward
    # HEAD - large round skull (white)
    ellipse(g, 33, 12, 10, 9, "#")    # rows3..21, cols23..43
    # MUZZLE - juts forward (white)
    ellipse(g, 44, 18, 4, 3, "#")     # rows15..21, cols40..47
    # EAR - black floppy lobe hanging over the rear of the head (hole)
    ellipse(g, 28, 13, 4, 9, ".")     # rows4..22, cols24..32
    # NOSE - black nose at the muzzle tip (hole)
    rect(g, 45, 16, 46, 18, ".")


def make_frame(legs, fname):
    g = new_canvas()
    draw_base(g, legs)
    with open(os.path.join(OUT, fname), "w") as f:
        for row in g:
            f.write("".join(row) + "\n")


# Frame 1: rear planted (foot back), front stepping forward (lifted, foot forward)
make_frame((False, -1, True, 2), "snoopy_frame1.txt")
# Frame 2: rear stepping forward (lifted, foot forward), front planted (foot forward)
make_frame((True, 2, False, 2), "snoopy_frame2.txt")

print("wrote tools/art/snoopy_frame{1,2}.txt")