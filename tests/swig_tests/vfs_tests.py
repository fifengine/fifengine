#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from swig_test_utils import *

import sys

class TestVfs(unittest.TestCase):
	def setUp(self):
		self.engine = getEngine(True)
		self.vfs = self.engine.getVFS()

	def tearDown(self):
		self.engine.destroy()
	
	def testListFiles(self):
		self.vfs.listFiles('.')
		self.assert_(self.vfs.listFiles('.'))
	
	def testListDirs(self):
		print self.vfs.listDirectories('.')
		self.assert_(self.vfs.listDirectories('.'))
	
	def testReadLines(self):
		data = self.vfs.open('../../test_fife.py')
		self.assert_(data.getDataInLines())
		del data
	
	def testReadBytes(self):
		data = self.vfs.open('../../test_fife.py')
		self.assert_(data.getDataInBytes())
		del data

TEST_CLASSES = [TestVfs]

if __name__ == '__main__':
    unittest.main()
