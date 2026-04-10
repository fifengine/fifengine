# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestExtWidgets:
    def test_module_docstring(self):
        from fife.extensions.pychan.widgets import ext

        assert "widget" in ext.__doc__.lower()
