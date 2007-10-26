#!/usr/bin/env python
from swig_test_utils import *

class TestVfs(unittest.TestCase):
	def setUp(self):
		self.engine = getEngine(True)
		self.vfs = fife.VFSUtility()

	def tearDown(self):
		del self.vfs
		del self.engine
	
	def testListFiles(self):
		self.assert_(self.vfs.listFiles('.'))
	
	def testListDirs(self):
		self.assert_(self.vfs.listDirectories('.'))
	
	def testReadLines(self):
		self.assert_(self.vfs.readLines('test_fife.py'))
	
	def testReadBytes(self):
		self.assert_(self.vfs.readBytes('test_fife.py'))

TEST_CLASSES = [TestVfs]

if __name__ == '__main__':
    unittest.main()
