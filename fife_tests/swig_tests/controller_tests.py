#!/usr/bin/python#!/usr/bin/python
from __init__ import *

class TestController(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine(True)
		
	def tearDown(self):
		del self.engine
		
	def testInstances(self):
		self.assert_(self.engine.getAudioManager())
		self.assert_(self.engine.getEventManager())
		self.assert_(self.engine.getTimeManager())
		self.assert_(self.engine.getSettingsManager())
		self.assert_(self.engine.getGuiManager())
		self.assert_(self.engine.getImagePool())
		self.assert_(self.engine.getAnimationPool())
		pass

	def testPumping(self):
		return
		self.engine.initializePumping()
		for i in xrange(10):
			self.engine.pump()
		self.engine.finalizePumping()

TEST_CLASSES = [TestController]

if __name__ == '__main__':
	unittest.main()
