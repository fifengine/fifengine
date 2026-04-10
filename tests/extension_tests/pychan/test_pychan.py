# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestPychanInit:
    def test_load_xml_exists(self):
        from fife.extensions.pychan import loadXML

        assert loadXML is not None

    def test_init_exists(self):
        from fife.extensions.pychan import init

        assert init is not None

    def test_manager_exists(self):
        from fife.extensions.pychan import manager

        assert manager is None


class TestPychanModule:
    def test_all_exports(self):
        from fife.extensions import pychan

        assert "loadXML" in pychan.__all__
        assert "loadFonts" in pychan.__all__
        assert "init" in pychan.__all__


class TestPychanExceptions:
    def test_all_exported(self):
        from fife.extensions.pychan.exceptions import (
            GuiXMLError,
            InitializationError,
            ParserError,
            PyChanException,
        )

        assert PyChanException is not None
        assert InitializationError is not None
        assert GuiXMLError is not None
        assert ParserError is not None


class TestPychanInternal:
    def test_get_manager_function(self):
        from fife.extensions.pychan.internal import get_manager

        assert callable(get_manager)

    def test_screen_width_function(self):
        from fife.extensions.pychan.internal import screen_width

        assert callable(screen_width)

    def test_screen_height_function(self):
        from fife.extensions.pychan.internal import screen_height

        assert callable(screen_height)

    def test_manager_class(self):
        from fife.extensions.pychan.internal import Manager

        assert Manager is not None


class TestPychanToolsFunctions:
    def test_apply_only_suitable(self):
        from fife.extensions.pychan.tools import applyOnlySuitable

        assert callable(applyOnlySuitable)

    def test_callback_with_arguments(self):
        from fife.extensions.pychan.tools import callbackWithArguments

        assert callable(callbackWithArguments)

    def test_attr_set_callback(self):
        from fife.extensions.pychan.tools import attrSetCallback

        assert callable(attrSetCallback)

    def test_chain_callbacks(self):
        from fife.extensions.pychan.tools import chainCallbacks

        assert callable(chainCallbacks)


class TestPychanAutoposition:
    def test_place_widget(self):
        from fife.extensions.pychan.autoposition import placeWidget

        assert callable(placeWidget)

    def test_constants(self):
        from fife.extensions.pychan.autoposition import (
            AUTOMATIC,
            BOTTOM,
            CENTER,
            EXPLICIT,
            LEFT,
            RIGHT,
            TOP,
        )

        assert EXPLICIT == "explicit"
        assert AUTOMATIC == "automatic"
        assert TOP == "top"
        assert LEFT == "left"
        assert RIGHT == "right"
        assert CENTER == "center"
        assert BOTTOM == "bottom"


class TestDialogsModule:
    def test_constants(self):
        from fife.extensions.pychan.dialogs import CANCEL, NO, OK, YES

        assert OK is True
        assert YES is True
        assert NO is False
        assert CANCEL is None

    def test_xml_dialog(self):
        from fife.extensions.pychan.dialogs import XMLDialog

        assert XMLDialog is not None


class TestPychanBasicApplication:
    def test_exit_event_listener(self):
        from fife.extensions.pychan.pychanbasicapplication import ExitEventListener

        assert ExitEventListener is not None

    def test_pychan_application_base(self):
        from fife.extensions.pychan.pychanbasicapplication import PychanApplicationBase

        assert PychanApplicationBase is not None


class TestFifePychanSettings:
    def test_class(self):
        from fife.extensions.pychan.fife_pychansettings import FifePychanSettings

        assert FifePychanSettings is not None


class TestPychanEvents:
    def test_event_mapper(self):
        from fife.extensions.pychan.events import EventMapper

        assert EventMapper is not None

    def test_event_listener_base(self):
        from fife.extensions.pychan.events import EventListenerBase

        assert EventListenerBase is not None


class TestPychanPropertiesModule:
    def test_module_docstring(self):
        from fife.extensions.pychan import properties

        assert "property" in properties.__doc__.lower()

    def test_wrapped_property_class(self):
        from fife.extensions.pychan.properties import WrappedProperty

        assert WrappedProperty is not None

    def test_color_property_class(self):
        from fife.extensions.pychan.properties import ColorProperty

        assert ColorProperty is not None

    def test_image_property_class(self):
        from fife.extensions.pychan.properties import ImageProperty

        assert ImageProperty is not None

    def test_get_manager_function(self):
        from fife.extensions.pychan.properties import get_manager

        assert callable(get_manager)


class TestPychanCompatModule:
    def test_module_docstring(self):
        from fife.extensions.pychan import compat

        assert "compatibility" in compat.__doc__.lower()

    def test_in_fife_variable(self):
        from fife.extensions.pychan.compat import in_fife

        assert in_fife is not None

    def test_fifechan_variable(self):
        from fife.extensions.pychan.compat import fifechan

        assert fifechan is not None


class TestPychanWidgetsCommonModule:
    def test_get_manager_function(self):
        from fife.extensions.pychan.widgets.common import get_manager

        assert callable(get_manager)

    def test_gui2str_function(self):
        from fife.extensions.pychan.widgets.common import gui2str

        assert callable(gui2str)


class TestPychanLayoutModule:
    def test_constants(self):
        from fife.extensions.pychan.widgets.layout import (
            AlignBottom,
            AlignCenter,
            AlignLeft,
            AlignRight,
            AlignTop,
        )

        assert AlignTop == 0
        assert AlignBottom == 1
        assert AlignLeft == 2
        assert AlignRight == 3
        assert AlignCenter == 4


class TestPychanExtModule:
    def test_module_docstring(self):
        from fife.extensions.pychan.widgets import ext

        assert "widget" in ext.__doc__.lower()
