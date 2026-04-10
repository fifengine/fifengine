# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestContainer:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.containers import Container

        assert Container is not None

    def test_attributes_defined(self):
        from fife.extensions.pychan.widgets.containers import Container

        assert hasattr(Container, "ATTRIBUTES")


class TestVBox:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.containers import VBox

        assert VBox is not None


class TestHBox:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.containers import HBox

        assert HBox is not None


class TestWindow:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.containers import Window

        assert Window is not None


class TestABox:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.containers import ABox

        assert ABox is not None


class TestCBox:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.containers import CBox

        assert CBox is not None


class TestPanel:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.panel import Panel

        assert Panel is not None


class TestScrollArea:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.scrollarea import ScrollArea

        assert ScrollArea is not None


class TestTabbedArea:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.tabbedarea import TabbedArea

        assert TabbedArea is not None


class TestTab:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.tabbedarea import Tab

        assert Tab is not None
