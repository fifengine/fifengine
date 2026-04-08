# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest


def test_list_files(engine_minimized):
    vfs = engine_minimized.getVFS()
    assert vfs.listFiles(".")


def test_list_dirs(engine_minimized):
    vfs = engine_minimized.getVFS()
    assert vfs.listDirectories(".")


def test_read_lines(engine_minimized):
    vfs = engine_minimized.getVFS()
    data = vfs.open("run_tests.py")
    assert data.getDataInLines()


def test_read_bytes(engine_minimized):
    vfs = engine_minimized.getVFS()
    data = vfs.open("run_tests.py")
    assert data.getDataInBytes()
