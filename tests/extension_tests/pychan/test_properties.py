# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestProperties:
    def test_module_docstring(self):
        from fife.extensions.pychan import properties

        assert "property" in properties.__doc__.lower()

    def test_wrapped_property_exists(self):
        from fife.extensions.pychan.properties import WrappedProperty

        assert WrappedProperty is not None

    def test_color_property_exists(self):
        from fife.extensions.pychan.properties import ColorProperty

        assert ColorProperty is not None

    def test_image_property_exists(self):
        from fife.extensions.pychan.properties import ImageProperty

        assert ImageProperty is not None


class TestFonts:
    def test_font_class_exists(self):
        from fife.extensions.pychan.fonts import Font

        assert Font is not None

    def test_load_fonts_exists(self):
        from fife.extensions.pychan.fonts import loadFonts

        assert loadFonts is not None
