# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest
from fife import fife


def test_draw_line(engine):
    points = (
        (1, 1),
        (50, 20),
        (20, 50),
        (200, 500),
        (500, 200),
        (100, 200),
        (10, 10),
        (70, 40),
        (80, 30),
        (300, 520),
        (340, 220),
        (170, 600),
    )

    engine.initializePumping()

    renderer = engine.getOffRenderer()
    renderer.setEnabled(True)

    prev_pt = fife.Point(*points[1])
    for pt in points[1:]:
        cur_pt = fife.Point(*pt)
        renderer.addLine("lines", prev_pt, cur_pt, 255, 255, 255)
        prev_pt = cur_pt

    for _ in range(200):
        engine.pump()

    renderer.removeAll("lines")

    engine.finalizePumping()
