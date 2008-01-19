#!/usr/bin/env python
from swig_test_utils import *

import sys

class TestVfs(unittest.TestCase):
	def setUp(self):
		self.engine = getEngine(True)
		self.vfs = self.engine.getVFS()

	def tearDown(self):
		del self.engine
	
	def testListFiles(self):
		self.vfs.listFilesNEW('.')
		self.assert_(self.vfs.listFilesNEW('.'))
	
	def testListDirs(self):
		print self.vfs.listDirectoriesNEW('.')
		self.assert_(self.vfs.listDirectoriesNEW('.'))
	
	def testReadLines(self):
		data = self.vfs.open('test_fife.py')
		self.assert_(data.getDataInLines())
		del data
	
	def testReadBytes(self):
		data = self.vfs.open('test_fife.py')
		self.assert_(data.getDataInBytes())
		del data

TEST_CLASSES = [TestVfs]

if __name__ == '__main__':
    unittest.main()
