#!/usr/bin/env python
from swig_test_utils import *

class TestController(unittest.TestCase):
	
	def setUp(self):
		self.engine = getEngine(True)
		
	def tearDown(self):
		self.engine.destroy()
		
	def testInstances(self):
		print "\nTest SoundManager"
		self.assert_(self.engine.getSoundManager())
		print "Test EventManager"
		self.assert_(self.engine.getEventManager())
		print "Test TimeManager"
		self.assert_(self.engine.getTimeManager())
		print "Test GuiManager"
		self.assert_(self.engine.getGuiManager())
		print "Test ImagePool"		
		self.assert_(self.engine.getImagePool())
		print "Test AnimationPool"
		self.assert_(self.engine.getAnimationPool())
		pass

	def testPumping(self):
		self.engine.initializePumping()
		for i in xrange(10):
			self.engine.pump()
		self.engine.finalizePumping()

TEST_CLASSES = [TestController]

if __name__ == '__main__':
	unittest.main()
