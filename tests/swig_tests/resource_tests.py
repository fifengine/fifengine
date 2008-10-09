#!/usr/bin/env python
import sys
from swig_test_utils import *
from serializers import *
from serializers.xmlanimation import XMLAnimationLoader

class TestPool(unittest.TestCase):
	
	def setUp(self):
		self.engine = getEngine()
		
	def tearDown(self):
		self.engine.destroy()
			
	def testImagePool(self):
		pool = self.engine.getImagePool()
		self.assert_(pool)
		self.assert_(pool.getResourceCount(fife.RES_LOADED) == 0)
		id = pool.addResourceFromFile('tests/data/beach_e1.png')
		self.assertEqual(pool.getResourceCount(fife.RES_LOADED), 0)
		self.assertEqual(pool.getResourceCount(fife.RES_NON_LOADED), 1)
		img = pool.getImage(id)
		self.assertEqual(pool.getResourceCount(fife.RES_LOADED), 1)
		self.assertEqual(pool.getResourceCount(fife.RES_NON_LOADED), 0)

	def testImagePoolFail(self):
		pool = self.engine.getImagePool()
		id = pool.addResourceFromFile('bogus_image.png')
		self.assertRaises(RuntimeError,pool.getImage,id)

	def testAnimationPool(self):
		pool = self.engine.getAnimationPool()
		animationloader = XMLAnimationLoader(self.engine.getImagePool(), self.engine.getVFS())
		pool.addResourceLoader(animationloader)
		
		self.assert_(pool)
		self.assert_(pool.getResourceCount(fife.RES_LOADED) == 0)
		id = pool.addResourceFromFile('tests/data/wolf_walk/wolf_walk_sw.xml')
		self.assertEqual(pool.getResourceCount(fife.RES_LOADED), 0)
		self.assertEqual(pool.getResourceCount(fife.RES_NON_LOADED), 1)
		animation = pool.getAnimation(id)
		self.assertEqual(pool.getResourceCount(fife.RES_LOADED), 1)
		self.assertEqual(pool.getResourceCount(fife.RES_NON_LOADED), 0)

	def testAnimationPoolFail(self):
		pool = self.engine.getAnimationPool()
		animationloader = XMLAnimationLoader(self.engine.getImagePool(), self.engine.getVFS())
		pool.addResourceLoader(animationloader)
		
		id = pool.addResourceFromFile('bogus_animation.xml')
		#self.assertRaises(RuntimeError, pool.getAnimation, id) #<- this test still passes, but crashes python on exit

TEST_CLASSES = [TestPool]

if __name__ == '__main__':
    unittest.main()
