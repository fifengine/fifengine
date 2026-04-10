# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestPychanbasicapplication:
    def test_module_docstring(self):
        from fife.extensions.pychan import pychanbasicapplication

        assert "application" in pychanbasicapplication.__doc__.lower()

    def test_exit_event_listener_class_exists(self):
        from fife.extensions.pychan.pychanbasicapplication import ExitEventListener

        assert ExitEventListener is not None

    def test_pychan_application_base_class_exists(self):
        from fife.extensions.pychan.pychanbasicapplication import PychanApplicationBase

        assert PychanApplicationBase is not None
