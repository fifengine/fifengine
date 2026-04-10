# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestSerializersExceptions:
    def test_serializer_error_exists(self):
        from fife.extensions.serializers import SerializerError

        assert SerializerError is not None

    def test_invalid_format_exists(self):
        from fife.extensions.serializers import InvalidFormat

        assert InvalidFormat is not None

    def test_wrong_file_type_exists(self):
        from fife.extensions.serializers import WrongFileType

        assert WrongFileType is not None

    def test_name_clash_exists(self):
        from fife.extensions.serializers import NameClash

        assert NameClash is not None

    def test_not_found_exists(self):
        from fife.extensions.serializers import NotFound

        assert NotFound is not None


class TestSerializersFunctions:
    def test_root_subfile_exists(self):
        from fife.extensions.serializers import root_subfile

        assert callable(root_subfile)

    def test_reverse_root_subfile_exists(self):
        from fife.extensions.serializers import reverse_root_subfile

        assert callable(reverse_root_subfile)

    def test_et_import(self):
        from fife.extensions.serializers import ET

        assert ET is not None
