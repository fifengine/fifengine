#!/usr/bin/python
from __init__ import *

class TestVideo(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine()
		
	def tearDown(self):
		del self.engine
			
	def testImagePool(self):
		pool = self.engine.getImagePool()
		self.assert_(pool)
		self.assert_(pool.getResourceCount(fife.RES_LOADED) == 0)
		id = pool.addResourceFromFile('content/gfx/tiles/beach/beach_e1.png')
		self.assertEqual(pool.getResourceCount(fife.RES_LOADED), 0)
		self.assertEqual(pool.getResourceCount(fife.RES_NON_LOADED), 1)
		img = pool.getImage(id)
		self.assertEqual(pool.getResourceCount(fife.RES_LOADED), 1)
		self.assertEqual(pool.getResourceCount(fife.RES_NON_LOADED), 0)

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

