#!/usr/bin/python
from swig_test_utils import *

class TestView(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine()
		
	def tearDown(self):
		del self.engine

	def testCamera(self):
		cam = fife.Camera()
		self.engine.getView().addCamera(cam)
		self.engine.initializePumping()
		#for i in xrange(100):
		#	self.engine.pump()
		self.engine.finalizePumping()
		self.engine.getView().removeCamera(cam)
	

TEST_CLASSES = [TestView]

if __name__ == '__main__':
    unittest.main()

