# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from unittest.mock import MagicMock, patch


class TestFifeSettings:
    def test_setting_creates_in_user_directory(self, tmp_path, monkeypatch):
        settings_dir = tmp_path / ".fife" / "testapp"
        settings_dir.mkdir(parents=True)
        monkeypatch.setenv("HOME", str(tmp_path))

        from fife.extensions.fife_settings import Setting

        with patch(
            "fife.extensions.fife_settings.getUserDataDirectory",
            return_value=str(settings_dir),
        ):
            with patch("fife.extensions.fife_settings.SimpleXMLSerializer"):
                with patch("fife.extensions.fife_settings.shutil.copyfile"):
                    s = Setting(app_name="testapp")
                    assert s._app_name == "testapp"
                    assert s._appdata == str(settings_dir)

    def test_setting_with_explicit_path(self, tmp_path):
        from fife.extensions.fife_settings import Setting

        with patch("fife.extensions.fife_settings.SimpleXMLSerializer"):
            with patch(
                "fife.extensions.fife_settings.os.path.exists", return_value=False
            ):
                with patch("fife.extensions.fife_settings.shutil.copyfile"):
                    s = Setting(settings_file=str(tmp_path / "settings.xml"))
                    assert s._settings_file == "settings.xml"
                    assert s._appdata == str(tmp_path)

    def test_setting_default_values(self, tmp_path, monkeypatch):
        settings_dir = tmp_path / ".fife" / "testapp"
        settings_dir.mkdir(parents=True)
        monkeypatch.setenv("HOME", str(tmp_path))

        mock_serializer = MagicMock()
        mock_serializer.getModuleNameList.return_value = ["FIFE"]

        from fife.extensions.fife_settings import Setting

        with patch(
            "fife.extensions.fife_settings.getUserDataDirectory",
            return_value=str(settings_dir),
        ):
            with patch(
                "fife.extensions.fife_settings.SimpleXMLSerializer",
                return_value=mock_serializer,
            ):
                s = Setting(app_name="testapp", copy_dist=False)
                assert s._app_name == "testapp"
                assert s._settings_file == "settings.xml"

    def test_get_all_settings(self, tmp_path, monkeypatch):
        settings_dir = tmp_path / ".fife" / "testapp"
        settings_dir.mkdir(parents=True)
        monkeypatch.setenv("HOME", str(tmp_path))

        mock_serializer = MagicMock()
        mock_serializer.getModuleNameList.return_value = ["FIFE"]

        from fife.extensions.fife_settings import Setting

        with patch(
            "fife.extensions.fife_settings.getUserDataDirectory",
            return_value=str(settings_dir),
        ):
            with patch(
                "fife.extensions.fife_settings.SimpleXMLSerializer",
                return_value=mock_serializer,
            ):
                s = Setting(app_name="testapp", copy_dist=False)
                all_settings = s.getAllSettings("FIFE")
                assert isinstance(all_settings, dict)
                assert "FullScreen" in all_settings

    def test_get_one_setting(self, tmp_path, monkeypatch):
        settings_dir = tmp_path / ".fife" / "testapp"
        settings_dir.mkdir(parents=True)
        monkeypatch.setenv("HOME", str(tmp_path))

        mock_serializer = MagicMock()
        mock_serializer.getModuleNameList.return_value = ["FIFE"]

        from fife.extensions.fife_settings import Setting

        with patch(
            "fife.extensions.fife_settings.getUserDataDirectory",
            return_value=str(settings_dir),
        ):
            with patch(
                "fife.extensions.fife_settings.SimpleXMLSerializer",
                return_value=mock_serializer,
            ):
                s = Setting(app_name="testapp", copy_dist=False)
                value = s.getOneSetting("FIFE", "FullScreen", validSetting=False)
                assert not value

    def test_set_one_setting(self, tmp_path, monkeypatch):
        settings_dir = tmp_path / ".fife" / "testapp"
        settings_dir.mkdir(parents=True)
        monkeypatch.setenv("HOME", str(tmp_path))

        mock_serializer = MagicMock()
        mock_serializer.getModuleNameList.return_value = ["FIFE"]

        from fife.extensions.fife_settings import Setting

        with patch(
            "fife.extensions.fife_settings.getUserDataDirectory",
            return_value=str(settings_dir),
        ):
            with patch(
                "fife.extensions.fife_settings.SimpleXMLSerializer",
                return_value=mock_serializer,
            ):
                s = Setting(app_name="testapp", copy_dist=False)
                s.setOneSetting("FIFE", "FullScreen", True)
                value = s.getOneSetting("FIFE", "FullScreen")
                assert value is True

    def test_set_valid_resolutions(self, tmp_path, monkeypatch):
        settings_dir = tmp_path / ".fife" / "testapp"
        settings_dir.mkdir(parents=True)
        monkeypatch.setenv("HOME", str(tmp_path))

        mock_serializer = MagicMock()
        mock_serializer.getModuleNameList.return_value = ["FIFE"]

        from fife.extensions.fife_settings import Setting

        with patch(
            "fife.extensions.fife_settings.getUserDataDirectory",
            return_value=str(settings_dir),
        ):
            with patch(
                "fife.extensions.fife_settings.SimpleXMLSerializer",
                return_value=mock_serializer,
            ):
                s = Setting(app_name="testapp", copy_dist=False)
                s.setValidResolutions(["640x480", "800x600"])
                assert "640x480" in s._resolutions
                assert "800x600" in s._resolutions

    def test_set_valid_displays(self, tmp_path, monkeypatch):
        settings_dir = tmp_path / ".fife" / "testapp"
        settings_dir.mkdir(parents=True)
        monkeypatch.setenv("HOME", str(tmp_path))

        mock_serializer = MagicMock()
        mock_serializer.getModuleNameList.return_value = ["FIFE"]

        from fife.extensions.fife_settings import Setting

        with patch(
            "fife.extensions.fife_settings.getUserDataDirectory",
            return_value=str(settings_dir),
        ):
            with patch(
                "fife.extensions.fife_settings.SimpleXMLSerializer",
                return_value=mock_serializer,
            ):
                s = Setting(app_name="testapp", copy_dist=False)
                s.setValidDisplays([0, 1, 2])
                assert 0 in s._displays
                assert 1 in s._displays
                assert 2 in s._displays


class TestFifeSettingsAdvanced:
    def test_fife_module_constant(self):
        from fife.extensions.fife_settings import FIFE_MODULE

        assert FIFE_MODULE == "FIFE"

    def test_create_and_add_entry(self):
        from fife.extensions.fife_settings import Setting

        assert hasattr(Setting, "createAndAddEntry")

    def test_get_all_settings_method(self):
        from fife.extensions.fife_settings import Setting

        assert hasattr(Setting, "getAllSettings")

    def test_get_one_setting_method(self):
        from fife.extensions.fife_settings import Setting

        assert hasattr(Setting, "getOneSetting")

    def test_set_one_setting_method(self):
        from fife.extensions.fife_settings import Setting

        assert hasattr(Setting, "setOneSetting")

    def test_set_valid_resolutions_method(self):
        from fife.extensions.fife_settings import Setting

        assert hasattr(Setting, "setValidResolutions")

    def test_set_valid_displays_method(self):
        from fife.extensions.fife_settings import Setting

        assert hasattr(Setting, "setValidDisplays")


class TestFifeSettingsEntry:
    def test_entry_class(self):
        from fife.extensions.fife_settings import SettingEntry

        assert SettingEntry is not None
