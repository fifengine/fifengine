# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestFifeSettingsClasses:
    def test_setting_class(self):
        from fife.extensions.fife_settings import Setting

        assert Setting is not None

    def test_setting_entry_class(self):
        from fife.extensions.fife_settings import SettingEntry

        assert SettingEntry is not None


class TestFifeUtilsFunctions:
    def test_get_user_data_directory(self):
        from fife.extensions.fife_utils import getUserDataDirectory

        assert callable(getUserDataDirectory)

    def test_is_fife_exc(self):
        from fife.extensions.fife_utils import is_fife_exc

        assert callable(is_fife_exc)


class TestFifeCompatFunctions:
    def test_deprecated(self):
        from fife.extensions.fife_compat import deprecated

        assert callable(deprecated)

    def test_this_is_deprecated(self):
        from fife.extensions.fife_compat import this_is_deprecated

        assert callable(this_is_deprecated)
