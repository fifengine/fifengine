# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestDialogs:
    def test_module_docstring(self):
        from fife.extensions.pychan import dialogs

        assert "dialog" in dialogs.__doc__.lower()

    def test_constants_defined(self):
        from fife.extensions.pychan import dialogs

        assert dialogs.OK is True
        assert dialogs.YES is True
        assert dialogs.NO is False
        assert dialogs.CANCEL is None

    def test_xml_dialog_class_exists(self):
        from fife.extensions.pychan.dialogs import XMLDialog

        assert XMLDialog is not None
