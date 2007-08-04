#!/usr/bin/python
from __init__ import *

class TestSettingsManager(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine(True)
		self.settings = self.engine.getSettingsManager()
		
	def tearDown(self):
		del self.engine
		
	def testSettings(self):
		self.assert_(self.settings.read_int("ScreenWidth", 800) > 0)
		self.assert_(self.settings.read_int("ScreenHeight", 600) > 0)

TEST_CLASSES = [TestSettingsManager]

if __name__ == '__main__':
	unittest.main()
