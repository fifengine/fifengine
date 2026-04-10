# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

class TestPychanWidgetsCommon:
    def test_get_manager_returns_none_initially(self):
        from fife.extensions.pychan.widgets.common import get_manager

        assert get_manager() is None

    def test_gui2str(self):
        from fife.extensions.pychan.widgets.common import gui2str

        result = gui2str("hello")
        assert result == "hello"
