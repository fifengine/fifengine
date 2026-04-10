# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestSavers:
    def test_save_map_file_exists(self):
        from fife.extensions import savers

        assert savers.saveMapFile is not None

    def test_add_map_saver_exists(self):
        from fife.extensions import savers

        assert savers.addMapSaver is not None

    def test_map_file_mapping_defined(self):
        from fife.extensions import savers

        assert hasattr(savers, "mapFileMapping")
        assert "xml" in savers.mapFileMapping
