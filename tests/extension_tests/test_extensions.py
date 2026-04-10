# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestFifeTimerModule:
    def test_module_docstring(self):
        from fife.extensions import fife_timer

        assert "timer" in fife_timer.__doc__.lower()

    def test_timer_class_exists(self):
        from fife.extensions.fife_timer import Timer

        assert Timer is not None

    def test_timer_init(self):
        from fife.extensions.fife_timer import Timer

        assert hasattr(Timer, "__init__")

    def test_manager_init(self):
        from fife.extensions.fife_timer import _manager

        assert _manager is None


class TestFifelogModule:
    def test_module_docstring(self):
        from fife.extensions import fifelog

        assert "log" in fifelog.__doc__.lower()

    def test_log_manager_exists(self):
        from fife.extensions.fifelog import LogManager

        assert LogManager is not None

    def test_log_manager_init(self):
        from fife.extensions.fifelog import LogManager

        assert hasattr(LogManager, "__init__")


class TestPythonizeModule:
    def test_module_docstring(self):
        from fife.extensions import pythonize

        assert "pythonic" in pythonize.__doc__.lower()

    def test_all_empty(self):
        from fife.extensions import pythonize

        assert pythonize.__all__ == ()


class TestLoadersModule:
    def test_all_exported(self):
        from fife.extensions import loaders

        expected = ["loadMapFile", "addMapLoader", "mapFileMapping", "fileExtensions"]
        for name in expected:
            assert hasattr(loaders, name)

    def test_add_map_loader(self):
        from fife.extensions.loaders import addMapLoader

        assert callable(addMapLoader)

    def test_load_map_file(self):
        from fife.extensions.loaders import loadMapFile

        assert callable(loadMapFile)


class TestSaversModule:
    def test_all_exported(self):
        from fife.extensions import savers

        expected = ["saveMapFile", "addMapSaver", "mapFileMapping", "fileExtensions"]
        for name in expected:
            assert hasattr(savers, name)

    def test_add_map_saver(self):
        from fife.extensions.savers import addMapSaver

        assert callable(addMapSaver)

    def test_save_map_file(self):
        from fife.extensions.savers import saveMapFile

        assert callable(saveMapFile)


class TestFifeCompatAdvanced:
    def test_deprecated_function(self):
        from fife.extensions.fife_compat import deprecated

        assert callable(deprecated)

    def test_this_is_deprecated_class(self):
        from fife.extensions.fife_compat import this_is_deprecated

        assert callable(this_is_deprecated)


class TestFifeUtilsAdvanced:
    def test_all_exports(self):
        from fife.extensions.fife_utils import __all__

        assert "is_fife_exc" in __all__
        assert "getUserDataDirectory" in __all__
