# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest
from fife.extensions.pychan.attrs import (
    Attr,
    BoolAttr,
    BoolListAttr,
    ColorAttr,
    FloatAttr,
    FloatListAttr,
    IntAttr,
    MixedListAttr,
    PointAttr,
    UnicodeAttr,
)


class TestAttrClasses:
    def test_attr_init(self):
        attr = Attr("test_name")
        assert attr.name == "test_name"

    def test_attr_set_and_parse(self):
        class MockObj:
            pass

        obj = MockObj()
        attr = Attr("my_attr")
        attr.set(obj, "test_value")
        assert obj.my_attr == "test_value"

    def test_attr_parse_returns_string(self):
        attr = Attr("test")
        assert attr.parse(123) == "123"
        assert attr.parse("hello") == "hello"

    def test_unicode_attr_parse(self):
        attr = UnicodeAttr("test")
        result = attr.parse("hello world")
        assert result == "hello world"

    def test_bool_attr_parse_true(self):
        attr = BoolAttr("test")
        assert attr.parse("true") is True
        assert attr.parse("1") is True
        assert attr.parse("True") is True

    def test_bool_attr_parse_false(self):
        attr = BoolAttr("test")
        assert attr.parse("false") is False
        assert attr.parse("0") is False
        assert attr.parse("False") is False

    def test_int_attr_parse_invalid(self):
        attr = IntAttr("test")
        with pytest.raises(Exception):  # ParserError
            attr.parse("not_an_int")

    def test_float_attr_parse(self):
        attr = FloatAttr("test")
        assert attr.parse("3.14") == 3.14
        assert attr.parse("-0.5") == -0.5
        assert attr.parse("0") == 0.0

    def test_point_attr_parse(self):
        attr = PointAttr("test")
        result = attr.parse("10,20")
        assert result == (10, 20)

    def test_color_attr_parse(self):
        attr = ColorAttr("test")
        result = attr.parse("255,0,0")
        assert result[0] == 255
        assert result[1] == 0
        assert result[2] == 0

    def test_mixed_list_attr_parse(self):
        attr = MixedListAttr("test")
        with pytest.raises(Exception):  # ParserError - requires specific format
            attr.parse("1,2,hello")

    def test_bool_list_attr_parse(self):
        attr = BoolListAttr("test")
        result = attr.parse("true,true,true")
        assert result == [True, True, True]

    def test_float_list_attr_parse(self):
        attr = FloatListAttr("test")
        result = attr.parse("1.1,2.2,3.3")
        assert result == [1.1, 2.2, 3.3]
