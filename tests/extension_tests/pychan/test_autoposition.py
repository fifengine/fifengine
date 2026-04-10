# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest
from fife.extensions.pychan.autoposition import (
    AUTOMATIC,
    BOTTOM,
    CENTER,
    EXPLICIT,
    LEFT,
    RIGHT,
    TOP,
    _parsePosition,
    _splicePosition,
)
from fife.extensions.pychan.exceptions import PyChanException


class TestAutoposition:
    def test_constants_defined(self):
        assert EXPLICIT == "explicit"
        assert AUTOMATIC == "automatic"
        assert TOP == "top"
        assert LEFT == "left"
        assert RIGHT == "right"
        assert CENTER == "center"
        assert BOTTOM == "bottom"

    def test_splice_position_with_plus(self):
        technique, delta = _splicePosition("center+10")
        assert technique == "center"
        assert delta == 10

    def test_splice_position_with_minus(self):
        technique, delta = _splicePosition("left-20")
        assert technique == "left"
        assert delta == -20

    def test_splice_position_no_delta(self):
        technique, delta = _splicePosition("center")
        assert technique == "center"
        assert delta == 0

    def test_parse_position_automatic(self):
        result = _parsePosition(AUTOMATIC)
        assert result == (CENTER, 0, CENTER, 0)

    def test_parse_position_explicit(self):
        with pytest.raises(PyChanException):
            _parsePosition(EXPLICIT)

    def test_parse_position_center_top(self):
        result = _parsePosition("center:top")
        assert result == (CENTER, 0, TOP, 0)

    def test_parse_position_left_bottom(self):
        result = _parsePosition("left:bottom")
        assert result == (LEFT, 0, BOTTOM, 0)

    def test_parse_position_with_deltas(self):
        result = _parsePosition("center+10:top-5")
        assert result == (CENTER, 10, TOP, -5)

    def test_parse_position_invalid_x_raises(self):
        with pytest.raises(PyChanException):
            _parsePosition("invalid:top")

    def test_parse_position_invalid_y_raises(self):
        with pytest.raises(PyChanException):
            _parsePosition("center:invalid")

    def test_parse_position_invalid_format_raises(self):
        with pytest.raises(PyChanException):
            _parsePosition("not_valid")
