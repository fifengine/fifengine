# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestImageButton:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.buttons import ImageButton

        assert ImageButton is not None


class TestToggleButton:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.buttons import ToggleButton

        assert ToggleButton is not None
