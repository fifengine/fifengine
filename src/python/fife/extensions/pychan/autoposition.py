# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
Automatic widget positioning.

You can use the C{position_technique} attribute
on top level widgets which can also be set from xml.

For direct use call L{placeWidget}.
"""

from .exceptions import PyChanException
from .internal import screen_height, screen_width

EXPLICIT = "explicit"
AUTOMATIC = "automatic"

TOP = "top"
LEFT = "left"
RIGHT = "right"
CENTER = "center"
BOTTOM = "bottom"


def _splicePosition(p):
    if "+" in p:
        technique, delta = p.split("+")
    elif "-" in p:
        technique, delta = p.split("-")
        delta = "-" + delta
    else:
        technique, delta = p, 0
    delta = int(delta)
    return technique, delta


def _parsePosition(position):
    try:
        if position == AUTOMATIC:
            position = "center+0:center+0"

        x_pos, y_pos = position.split(":")
        x_pos, x_delta = _splicePosition(x_pos)
        y_pos, y_delta = _splicePosition(y_pos)

        if x_pos not in [EXPLICIT, LEFT, CENTER, RIGHT]:
            raise ValueError("invalid x position")
        if y_pos not in [EXPLICIT, TOP, CENTER, BOTTOM]:
            raise ValueError("invalid y position")
    except (AttributeError, TypeError, ValueError):
        raise PyChanException("Malformed position definition: " + repr(position))
    return x_pos, x_delta, y_pos, y_delta


def placeWidget(widget, position):
    """
    Place a widget according to a string defining relative coordinates to screen borders.

    The position definition has to be of the form: C{"<x_pos><x_delta>:<y_pos><y_delta>"}

    C{<x_pos>} may be one of:
      - left
      - right
      - center
      - explicit

    C{<y_pos>} may be one of:
      - top
      - bottom
      - center
      - explicit

    C{explicit} means that the widgets x or y position will not be touched. The rest should be
    self explanatory.

    C{<x_delta>} and C{<y_delta>} must be of the form: +pixel_number or -pixel_number. Or completely
    omitted. Note that the sign has to be there for for positive deltas, too.

    For brevity two shortcuts exist:
      - "explicit" -> "explicit:explicit"
      - "automatic" -> "center:center"

    A few examples::
      "right-20:top"
      "center:top+10"
      "center:center"

    Parameters
    ----------
    widget : object
        The PyChan widget.
    position : str
        A position definition.

    Notes
    -----
    If the position cannot be parsed a L{PyChanException} is thrown.

    """
    if position == EXPLICIT:
        return
    x_pos, x_delta, y_pos, y_delta = _parsePosition(position)

    x, y = widget.position
    w, h = widget.size

    if x_pos == CENTER:
        x = (screen_width() - w) // 2 + x_delta

    if y_pos == CENTER:
        y = (screen_height() - h) // 2 + y_delta

    if x_pos == LEFT:
        x = x_delta

    if y_pos == TOP:
        y = y_delta

    if x_pos == RIGHT:
        x = screen_width() - w + x_delta

    if y_pos == BOTTOM:
        y = screen_height() - h + y_delta

    widget.position = x, y
