#!/usr/bin/env python
from swig_test_utils import *

class TestVideo(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine()
		
	def tearDown(self):
		del self.engine

	def testSimpleAnimation(self):
		pool = self.engine.getAnimationPool()
		id = pool.addResourceFromFile('content/animations/critters/animals/wolf_walk_sw.xml')
		animation = pool.getAnimation(id)
		self.engine.initializePumping()
		backend = self.engine.getRenderBackend()
		gettime = self.engine.getTimeManager().getTime
		starttime = gettime()
		for i in xrange(100):
			image = animation.getFrameByTimestamp(gettime() - starttime)
			if not image:
				image = animation.getFrame(0)
				starttime = gettime()
			image.render(fife.Rect(0,0,image.getWidth(),image.getHeight()),255)
			self.engine.pump()
		self.engine.finalizePumping()


	def testDrawLine(self):
		points = (
			(1,1), (50,20), (20,50), (200,500), (500,200), (100,200),
			(10,10), (70,40), (80,30), (300,520), (340,220), (170,600),
		)
		self.engine.initializePumping()
		
		be = self.engine.getRenderBackend()
		for i in xrange(100):
			prevPt = fife.Point(*points[1])
			for pt in points[1:]:
				curPt = fife.Point(*pt)
				be.drawLine(prevPt, curPt, 100, 100, 100)
				prevPt = curPt
			self.engine.pump()
		self.engine.finalizePumping()
	

TEST_CLASSES = [TestVideo]

if __name__ == '__main__':
    unittest.main()

