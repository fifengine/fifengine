#!/usr/bin/env python
from swig_test_utils import *

class TestPool(unittest.TestCase):
	
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

	def testImagePoolFail(self):
		pool = self.engine.getImagePool()
		id = pool.addResourceFromFile('bogus_image.png')
		self.assertRaises(fife.NotFound,pool.getImage,id)

	def testAnimationPool(self):
		pool = self.engine.getAnimationPool()
		self.assert_(pool)
		self.assert_(pool.getResourceCount(fife.RES_LOADED) == 0)
		id = pool.addResourceFromFile('content/animations/critters/animals/wolf_walk_sw.xml')
		self.assertEqual(pool.getResourceCount(fife.RES_LOADED), 0)
		self.assertEqual(pool.getResourceCount(fife.RES_NON_LOADED), 1)
		animation = pool.getAnimation(id)
		self.assertEqual(pool.getResourceCount(fife.RES_LOADED), 1)
		self.assertEqual(pool.getResourceCount(fife.RES_NON_LOADED), 0)

	def testAnimationPoolFail(self):
		pool = self.engine.getAnimationPool()
		id = pool.addResourceFromFile('bogus_animation.xml')
		self.assertRaises(fife.NotFound,pool.getAnimation,id)

TEST_CLASSES = [TestPool]

if __name__ == '__main__':
    unittest.main()
