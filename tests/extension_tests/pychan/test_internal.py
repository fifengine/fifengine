# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestInternal:
    def test_get_manager_exists(self):
        from fife.extensions.pychan.internal import get_manager

        assert get_manager is not None

    def test_screen_width_exists(self):
        from fife.extensions.pychan.internal import screen_width

        assert screen_width is not None

    def test_screen_height_exists(self):
        from fife.extensions.pychan.internal import screen_height

        assert screen_height is not None

    def test_manager_class_exists(self):
        from fife.extensions.pychan.internal import Manager

        assert Manager is not None
