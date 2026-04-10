# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from unittest.mock import MagicMock, patch

import pytest


class TestLoaders:
    def test_add_map_loader(self):
        from fife.extensions import loaders

        class MockLoader:
            pass

        loaders.addMapLoader("custom", MockLoader)
        assert "custom" in loaders.mapFileMapping
        assert loaders.mapFileMapping["custom"] == MockLoader
        assert "custom" in loaders.fileExtensions

    def test_update_map_file_extensions(self):
        from fife.extensions import loaders

        loaders.mapFileMapping["test"] = object
        loaders._updateMapFileExtensions()
        assert "test" in loaders.fileExtensions

    def test_load_map_file_with_unknown_extension_raises(self):
        from fife.extensions import loaders

        mock_engine = MagicMock()
        with pytest.raises(KeyError):
            loaders.loadMapFile("test.unknown", mock_engine)

    def test_file_extensions_is_set(self):
        from fife.extensions import loaders

        assert "xml" in loaders.fileExtensions
        assert isinstance(loaders.fileExtensions, set)


class TestSavers:
    def test_add_map_saver(self):
        from fife.extensions import savers

        class MockSaver:
            pass

        savers.addMapSaver("custom", MockSaver)
        assert "custom" in savers.mapFileMapping
        assert savers.mapFileMapping["custom"] == MockSaver
        assert "custom" in savers.fileExtensions

    def test_update_map_file_extensions(self):
        from fife.extensions import savers

        savers.mapFileMapping["test"] = object
        savers._updateMapFileExtensions()
        assert "test" in savers.fileExtensions

    def test_save_map_file_with_unknown_extension_raises(self):
        from fife.extensions import savers

        mock_engine = MagicMock()
        mock_map = MagicMock()
        with pytest.raises(KeyError):
            savers.saveMapFile("test.unknown", mock_engine, mock_map)

    def test_file_extensions_is_set(self):
        from fife.extensions import savers

        assert "xml" in savers.fileExtensions
        assert isinstance(savers.fileExtensions, set)

    def test_save_map_file_calls_saver(self):
        from fife.extensions import savers

        mock_engine = MagicMock()
        mock_map = MagicMock()
        mock_saver = MagicMock()

        with patch.object(savers, "mapFileMapping", {"xml": lambda *args: mock_saver}):
            with patch.object(savers, "_updateMapFileExtensions"):
                with patch("fife.extensions.savers.XMLMapSaver", return_value=mock_saver):
                    savers.saveMapFile("test.xml", mock_engine, mock_map)
                    mock_saver.saveResource.assert_called_once()
