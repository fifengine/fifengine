# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestFifelog:
    def test_log_manager_exists(self):
        from fife.extensions.fifelog import LogManager

        assert LogManager is not None
