# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestSavers:
    def test_save_map_file_exists(self):
        from fife.extensions import savers

        assert savers.saveMapFile is not None
