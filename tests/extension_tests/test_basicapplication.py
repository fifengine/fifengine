# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestExitEventListener:
    def test_class_exists(self):
        from fife.extensions.basicapplication import ExitEventListener

        assert ExitEventListener is not None


class TestApplicationBase:
    def test_class_exists(self):
        from fife.extensions.basicapplication import ApplicationBase

        assert ApplicationBase is not None

    def test_module_docstring(self):
        from fife.extensions import basicapplication

        assert (
            "application" in basicapplication.__doc__.lower()
            or "main loop" in basicapplication.__doc__.lower()
        )
