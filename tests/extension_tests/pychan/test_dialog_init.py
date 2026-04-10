# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestDialogInit:
    def test_module_docstring(self):
        from fife.extensions.pychan import dialog

        assert "dialog" in dialog.__doc__.lower()
