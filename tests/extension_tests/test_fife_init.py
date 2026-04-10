# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestFifeModuleImports:
    def test_fife_import(self):
        from fife import fife

        assert fife is not None

    def test_fifechan_import(self):
        from fife import fifechan

        assert fifechan is not None


class TestFifeExtensionsInit:
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


class TestFifeModule:
    def test_fife_import(self):
        import fife

        assert fife is not None

    def test_fifechan_import(self):
        import fife

        assert hasattr(fife, "fifechan")


class TestFifeInitModule:
    def test_module_exists(self):
        import fife

        assert hasattr(fife, "fife")
        assert hasattr(fife, "fifechan")
