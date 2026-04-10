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

    def test_all_exported(self):
        from fife.extensions import serializers

        expected = [
            "SerializerError",
            "InvalidFormat",
            "WrongFileType",
            "NameClash",
            "NotFound",
            "ET",
            "root_subfile",
            "reverse_root_subfile",
        ]
        for name in expected:
            assert name in serializers.__all__

    def test_et_module(self):
        from fife.extensions.serializers import ET

        assert ET is not None

    def test_root_subfile(self):
        from fife.extensions.serializers import root_subfile

        assert callable(root_subfile)

    def test_reverse_root_subfile(self):
        from fife.extensions.serializers import reverse_root_subfile

        assert callable(reverse_root_subfile)


class TestSerializersExceptions:
    def test_hierarchy(self):
        from fife.extensions.serializers import (
            InvalidFormat,
            NameClash,
            NotFound,
            SerializerError,
            WrongFileType,
        )

        assert issubclass(InvalidFormat, SerializerError)
        assert issubclass(WrongFileType, SerializerError)
        assert issubclass(NameClash, SerializerError)
        assert issubclass(NotFound, SerializerError)


class TestXmlLoaderTools:
    def test_norm_path(self):
        from fife.extensions.serializers.xml_loader_tools import norm_path

        assert callable(norm_path)

    def test_root_subfile(self):
        from fife.extensions.serializers.xml_loader_tools import root_subfile

        assert callable(root_subfile)

    def test_reverse_root_subfile(self):
        from fife.extensions.serializers.xml_loader_tools import reverse_root_subfile

        assert callable(reverse_root_subfile)

    def test_frange(self):
        from fife.extensions.serializers.xml_loader_tools import frange

        assert callable(frange)


class TestSimpleXMLSerializer:
    def test_class_exists(self):
        from fife.extensions.serializers.simplexml import SimpleXMLSerializer

        assert SimpleXMLSerializer is not None

    def test_simple_serializer_exists(self):
        from fife.extensions.serializers.simplexml import SimpleSerializer

        assert SimpleSerializer is not None
