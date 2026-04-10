# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestBasicTextWidget:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.basictextwidget import BasicTextWidget

        assert BasicTextWidget is not None

    def test_attributes_inherited(self):
        from fife.extensions.pychan.widgets.basictextwidget import BasicTextWidget

        assert hasattr(BasicTextWidget, "ATTRIBUTES")

    def test_default_hexpand(self):
        from fife.extensions.pychan.widgets.basictextwidget import BasicTextWidget

        assert BasicTextWidget.DEFAULT_HEXPAND is True

    def test_default_vexpand(self):
        from fife.extensions.pychan.widgets.basictextwidget import BasicTextWidget

        assert BasicTextWidget.DEFAULT_VEXPAND is False
