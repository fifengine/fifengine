#!/usr/bin/python
from __init__ import *

class TestVfs(unittest.TestCase):
	def setUp(self):
		self.engine = fife.Engine()
		self.vfs = fife.VFSUtility()

	def tearDown(self):
		del self.vfs
		del self.engine
	
	def testListFiles(self):
		self.assert_(self.vfs.listFiles('.'))
	
	def testListDirs(self):
		self.assert_(self.vfs.listDirectories('.'))
	
	def testReadLines(self):
		self.assert_(self.vfs.readLines('fife.config'))
	
	def testReadBytes(self):
		self.assert_(self.vfs.readBytes('fife.config'))

TEST_CLASSES = [TestVfs]

if __name__ == '__main__':
    unittest.main()