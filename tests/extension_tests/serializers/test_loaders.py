# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestLoaders:
    def test_load_map_file_exists(self):
        from fife.extensions import loaders

        assert loaders.loadMapFile is not None

    def test_add_map_loader_exists(self):
        from fife.extensions import loaders

        assert loaders.addMapLoader is not None

    def test_map_file_mapping_defined(self):
        from fife.extensions import loaders

        assert hasattr(loaders, "mapFileMapping")
        assert "xml" in loaders.mapFileMapping
