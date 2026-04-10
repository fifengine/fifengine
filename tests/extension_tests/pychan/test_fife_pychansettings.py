# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestFifePychansettings:
    def test_module_docstring(self):
        from fife.extensions.pychan import fife_pychansettings

        assert "settings" in fife_pychansettings.__doc__.lower()

    def test_settings_gui_xml_defined(self):
        from fife.extensions.pychan import fife_pychansettings

        assert hasattr(fife_pychansettings, "SETTINGS_GUI_XML")
        assert "Window" in fife_pychansettings.SETTINGS_GUI_XML

    def test_pychansettings_class_exists(self):
        from fife.extensions.pychan.fife_pychansettings import FifePychanSettings

        assert FifePychanSettings is not None
