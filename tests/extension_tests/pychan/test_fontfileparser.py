# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest
from fife.extensions.pychan.fontfileparser import FontFileParser


class TestFontFileParser:
    def test_parser_init(self):
        parser = FontFileParser()
        assert parser._fonts == {}

    def test_start_element_font_with_name(self):
        parser = FontFileParser()
        mock_attrs = MockAttributes({"name": "test_font", "size": "12"})
        parser.startElement("font", mock_attrs)
        assert "test_font" in parser._fonts

    def test_start_element_font_without_name_raises(self):
        parser = FontFileParser()
        mock_attrs = MockAttributes({"size": "12"})
        with pytest.raises(ValueError):
            parser.startElement("font", mock_attrs)

    def test_start_element_other_tag(self):
        parser = FontFileParser()
        mock_attrs = MockAttributes({})
        parser.startElement("other", mock_attrs)
        assert parser._fonts == {}

    def test_get_existing_font(self):
        parser = FontFileParser()
        parser._fonts = {"test_font": {"size": "12"}}
        result = parser.get("test_font", "size")
        assert result == "12"

    def test_get_missing_font(self):
        parser = FontFileParser()
        result = parser.get("missing_font", "size", "default")
        assert result == "default"

    def test_get_missing_attribute(self):
        parser = FontFileParser()
        parser._fonts = {"test_font": {}}
        result = parser.get("test_font", "size", "default")
        assert result == "default"

    def test_fonts_iterator(self):
        parser = FontFileParser()
        parser._fonts = {"font1": {}, "font2": {}}
        result = list(parser.fonts())
        assert "font1" in result
        assert "font2" in result

    def test_parse_from_string(self):
        xml = b"""<?xml version="1.0"?>
        <fonts>
            <font name="test">size=12</font>
        </fonts>
        """
        parser = FontFileParser()
        parser.parseString(xml)
        assert "test" in parser._fonts


class MockAttributes:
    def __init__(self, attrs):
        self._attrs = attrs

    def getValue(self, name):
        return self._attrs[name]

    def getNames(self):
        return self._attrs.keys()
