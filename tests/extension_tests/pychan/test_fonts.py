# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestFontsModule:
    def test_font_class_exists(self):
        from fife.extensions.pychan.fonts import Font

        assert Font is not None

    def test_load_fonts_exists(self):
        from fife.extensions.pychan.fonts import loadFonts

        assert loadFonts is not None
