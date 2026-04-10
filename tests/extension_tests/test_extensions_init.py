# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestExtensionsInit:
    def test_module_docstring(self):
        from fife import extensions

        assert "extension" in extensions.__doc__.lower()

    def test_all_exports(self):
        from fife import extensions

        expected = [
            "basicapplication",
            "fife_compat",
            "fife_timer",
            "fife_utils",
            "fife_settings",
            "fifelog",
            "pythonize",
            "savers",
            "loaders",
            "pychan",
        ]
        for name in expected:
            assert name in extensions.__all__
