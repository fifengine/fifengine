#!/usr/bin/python
from __init__ import *

class TestVideo(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine()
		
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

TEST_CLASSES = [TestVideo]

if __name__ == '__main__':
    unittest.main()

