# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestTextField:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.textfield import TextField

        assert TextField is not None


class TestTextBox:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.textbox import TextBox

        assert TextBox is not None


class TestPasswordField:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.passwordfield import PasswordField

        assert PasswordField is not None


class TestListBox:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.listbox import ListBox

        assert ListBox is not None


class TestDropDown:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.dropdown import DropDown

        assert DropDown is not None


class TestSlider:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.slider import Slider

        assert Slider is not None


class TestFlowContainer:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.flowcontainer import FlowContainer

        assert FlowContainer is not None


class TestAdjustingContainer:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.adjustingcontainer import AdjustingContainer

        assert AdjustingContainer is not None


class TestDockArea:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.dockarea import DockArea

        assert DockArea is not None


class TestResizableWindow:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.resizablewindow import ResizableWindow

        assert ResizableWindow is not None
