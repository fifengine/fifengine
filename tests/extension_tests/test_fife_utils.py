# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import os


class TestFifeUtils:
    def test_getuserdatadirectory_creates_directory(self, tmp_path, monkeypatch):
        from fife.extensions import fife_utils

        home_dir = tmp_path / "home"
        home_dir.mkdir()
        monkeypatch.setenv("HOME", str(home_dir))

        result = fife_utils.getUserDataDirectory("testvendor", "testapp")

        assert os.path.isdir(result)
        assert str(home_dir) in result
        assert "testvendor" in result
        assert "testapp" in result

    def test_getuserdatadirectory_returns_existing(self, tmp_path, monkeypatch):
        from fife.extensions import fife_utils

        existing_dir = tmp_path / ".testvendor" / "testapp"
        existing_dir.mkdir(parents=True)
        monkeypatch.setenv("HOME", str(tmp_path))

        result = fife_utils.getUserDataDirectory("testvendor", "testapp")

        assert result == str(existing_dir)

    def test_is_fife_exc_returns_false_for_non_matching(self):
        from fife.extensions import fife_utils

        class FakeType:
            @staticmethod
            def getTypeStr():
                return "TestType"

        result = fife_utils.is_fife_exc(FakeType, RuntimeError("Some random error"))
        assert result is False


class TestFifeUtilsModule:
    def test_all_exports(self):
        from fife.extensions import fife_utils

        expected = ["is_fife_exc", "getUserDataDirectory"]
        for name in expected:
            assert name in fife_utils.__all__

    def test_module_docstring(self):
        from fife.extensions import fife_utils

        assert (
            "utility" in fife_utils.__doc__.lower()
            or "function" in fife_utils.__doc__.lower()
        )
