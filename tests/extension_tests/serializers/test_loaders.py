# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestLoaders:
    def test_load_map_file_exists(self):
        from fife.extensions import loaders

        assert loaders.loadMapFile is not None
