#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import, print_function

from .swig_test_utils import getEngine, unittest


class TestVfs(unittest.TestCase):
    def setUp(self):
        self.engine = getEngine(True)
        self.vfs = self.engine.getVFS()

    def tearDown(self):
        self.engine.destroy()

    def testListFiles(self):
        self.vfs.listFiles(".")
        self.assert_(self.vfs.listFiles("."))

    def testListDirs(self):
        print(self.vfs.listDirectories("."))
        self.assert_(self.vfs.listDirectories("."))

    def testReadLines(self):
        data = self.vfs.open("run_tests.py")
        self.assert_(data.getDataInLines())
        del data

    def testReadBytes(self):
        data = self.vfs.open("run_tests.py")
        self.assert_(data.getDataInBytes())
        del data


TEST_CLASSES = [TestVfs]

if __name__ == "__main__":
    unittest.main()