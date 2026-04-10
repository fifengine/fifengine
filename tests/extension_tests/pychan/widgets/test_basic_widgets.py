# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestSpacer:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.spacer import Spacer

        assert Spacer is not None

    def test_default_hexpand(self):
        from fife.extensions.pychan.widgets.spacer import Spacer

        assert Spacer.DEFAULT_HEXPAND is True

    def test_default_vexpand(self):
        from fife.extensions.pychan.widgets.spacer import Spacer

        assert Spacer.DEFAULT_VEXPAND is False


class TestLabel:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.label import Label

        assert Label is not None


class TestIcon:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.icon import Icon

        assert Icon is not None


class TestButton:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.buttons import Button

        assert Button is not None


class TestCheckBox:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.checkbox import CheckBox

        assert CheckBox is not None


class TestRadioButton:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.radiobutton import RadioButton

        assert RadioButton is not None
