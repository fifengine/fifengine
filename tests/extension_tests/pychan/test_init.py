# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


import pytest


class TestPychanInit:
    def test_all_exports(self):
        from fife.extensions import pychan

        assert "loadXML" in pychan.__all__
        assert "loadFonts" in pychan.__all__
        assert "init" in pychan.__all__
        assert "manager" in pychan.__all__

    def test_exceptions_import(self):
        from fife.extensions.pychan import (
            GuiXMLError,
            InitializationError,
            PyChanException,
        )

        with pytest.raises(PyChanException):
            raise PyChanException("test")

        with pytest.raises(InitializationError):
            raise InitializationError("test")

        with pytest.raises(GuiXMLError):
            raise GuiXMLError("test")

    def test_manager_is_none_initially(self):
        from fife.extensions import pychan

        assert pychan.manager is None

    def test_traced_decorator(self):
        from fife.extensions.pychan import traced

        @traced
        def func_that_works():
            return "result"

        result = func_that_works()
        assert result == "result"

    def test_traced_decorator_raises(self):
        from fife.extensions.pychan import traced

        @traced
        def func_that_raises():
            raise ValueError("test error")

        with pytest.raises(ValueError):
            func_that_raises()

    def test_widgets_exported(self):
        from fife.extensions.pychan import WIDGETS, Widget

        assert Widget is not None
        assert isinstance(WIDGETS, dict)

    def test_tab_exported(self):
        from fife.extensions.pychan import Tab

        assert Tab is not None
