# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife.extensions.pychan.widgets import WIDGETS, registerWidget


class TestPychanWidgets:
    def test_widgets_dict_not_empty(self):
        assert len(WIDGETS) > 0

    def test_standard_widgets_registered(self):
        assert "Container" in WIDGETS
        assert "Label" in WIDGETS
        assert "Button" in WIDGETS
        assert "Window" in WIDGETS

    def test_register_widget(self):
        class CustomWidget:
            pass

        initial_count = len(WIDGETS)
        registerWidget(CustomWidget)
        assert "CustomWidget" in WIDGETS
        assert len(WIDGETS) == initial_count + 1
