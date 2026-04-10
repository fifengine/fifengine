# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife.extensions.serializers.xml_loader_tools import (
    frange,
    norm_path,
    reverse_root_subfile,
    root_subfile,
)


class TestXmlLoaderTools:
    def test_norm_path_on_unix(self):
        result = norm_path("/some/path/file.txt")
        assert result == "/some/path/file.txt"

    def test_frange_with_single_limit(self):
        result = list(frange(5))
        assert result == [0.0, 1.0, 2.0, 3.0, 4.0]

    def test_frange_with_two_limits(self):
        result = list(frange(1, 4))
        assert result == [1.0, 2.0, 3.0]

    def test_frange_with_increment(self):
        result = list(frange(0, 1, 0.25))
        assert result == [0.0, 0.25, 0.5, 0.75]

    def test_frange_negative(self):
        result = list(frange(-2, 2, 1))
        assert result == [-2.0, -1.0, 0.0, 1.0]

    def test_frange_float_values(self):
        result = list(frange(0, 0.5, 0.1))
        assert len(result) == 5
        assert result[0] == 0.0
        assert abs(result[-1] - 0.4) < 0.01

    def test_root_subfile_basic(self, tmp_path):
        file1 = tmp_path / "file1.xml"
        file2 = tmp_path / "subdir" / "file2.xml"
        file2.parent.mkdir()
        file1.touch()
        file2.touch()

        result = root_subfile(str(file1), str(file2))
        assert isinstance(result, str)

    def test_root_subfile_same_directory(self, tmp_path):
        file1 = tmp_path / "dir" / "file1.xml"
        file2 = tmp_path / "dir" / "file2.xml"
        file1.parent.mkdir()
        file1.touch()
        file2.touch()

        result = root_subfile(str(file1), str(file2))
        assert isinstance(result, str)

    def test_reverse_root_subfile(self, tmp_path):
        file1 = tmp_path / "test.xml"
        file1.touch()

        result = reverse_root_subfile(str(file1), "test.xml")
        assert isinstance(result, str)

    def test_frange_count_calculation(self):
        result = frange(0, 10, 3)
        count = sum(1 for _ in result)
        assert count == 4
