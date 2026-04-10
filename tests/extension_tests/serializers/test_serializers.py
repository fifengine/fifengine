# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import os
from unittest.mock import MagicMock

import pytest


class TestSerializersInit:
    def test_norm_path_on_unix(self):
        from fife.extensions.serializers import norm_path

        result = norm_path("/some/path/file.txt")
        assert result == "/some/path/file.txt"

    def test_norm_path_on_windows(self, monkeypatch):
        monkeypatch.setattr(os, "path", MagicMock())
        monkeypatch.setattr(os.path, "sep", "\\")

        from fife.extensions.serializers import norm_path

        result = norm_path("C:\\some\\path\\file.txt")
        assert "/" in result

    def test_reverse_root_subfile_basic(self):
        from fife.extensions.serializers import reverse_root_subfile

        result = reverse_root_subfile("./../foo/bar.xml", "../foo2/subfoo.xml")
        assert isinstance(result, str)

    def test_reverse_root_subfile_simple(self, tmp_path, monkeypatch):
        monkeypatch.chdir(tmp_path)
        from fife.extensions.serializers import reverse_root_subfile

        result = reverse_root_subfile("test.xml", "other.xml")
        assert isinstance(result, str)

    def test_serializer_exceptions_exist(self):
        from fife.extensions.serializers import (
            InvalidFormat,
            NameClash,
            NotFound,
            SerializerError,
            WrongFileType,
        )

        with pytest.raises(SerializerError):
            raise SerializerError("test")

        with pytest.raises(InvalidFormat):
            raise InvalidFormat("test")

        with pytest.raises(WrongFileType):
            raise WrongFileType("test")

        with pytest.raises(NameClash):
            raise NameClash("test")

        with pytest.raises(NotFound):
            raise NotFound("test")

    def test_warn_function(self, capsys, tmp_path):
        from fife.extensions.serializers import warn

        class FakeSelf:
            filename = tmp_path / "test.xml"

        warn(FakeSelf(), "test warning")
        captured = capsys.readouterr()
        assert "Warning" in captured.out
        assert "test warning" in captured.out
