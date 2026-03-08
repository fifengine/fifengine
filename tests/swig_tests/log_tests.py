#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import

from .swig_test_utils import unittest


class TestLog(unittest.TestCase):
    def testLog(self):
        pass


TEST_CLASSES = [TestLog]

if __name__ == "__main__":
    unittest.main()