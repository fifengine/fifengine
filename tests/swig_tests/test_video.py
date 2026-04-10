# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife


def test_draw_line(engine):
    box = [
        (150, 300),
        (150, 450),
        (450, 450),
        (450, 300),
        (150, 300),
    ]

    roof = [
        (150, 300),
        (300, 150),
        (450, 300),
        (150, 300),
    ]

    # choose chimney x-range
    x1, x2 = 340, 380

    # compute bottom points ON the roof slope
    y1 = x1 - 150
    y2 = x2 - 150

    chimney = [
        (x1, y1),       # bottom-left (on roof)
        (x1, y1 - 60),  # top-left (vertical)
        (x2, y1 - 60),  # top-right (horizontal top)
        (x2, y2),       # bottom-right (on roof)
        (x1, y1),
    ]

    engine.initializePumping()

    renderer = engine.getOffRenderer()
    renderer.setEnabled(True)

    def draw_shape(points):
        prev = fife.Point(*points[0])
        for pt in points[1:]:
            cur = fife.Point(*pt)
            renderer.addLine("lines", prev, cur, 255, 255, 255)
            prev = cur

    draw_shape(box)
    draw_shape(roof)
    draw_shape(chimney)

    for _ in range(200):
        engine.pump()

    renderer.removeAll("lines")
    engine.finalizePumping()
