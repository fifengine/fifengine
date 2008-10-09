#!/usr/bin/env python
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
