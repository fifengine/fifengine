# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestWidget:
    def test_widget_class_exists(self):
        from fife.extensions.pychan.widgets.widget import Widget

        assert Widget is not None


class TestWidgetRegistry:
    def test_widgets_registered(self):
        from fife.extensions.pychan.widgets import WIDGETS

        assert isinstance(WIDGETS, dict)
        assert len(WIDGETS) > 0
