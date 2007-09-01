#!/usr/bin/python
from swig_test_utils import *

class TestView(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine()
		map = self.engine.getModel().addMap()
		elev = map.addElevation()
		
		dat = self.engine.getModel().getMetaModel().addDataset()
		self.grid = fife.SquareGrid()
		pool = self.engine.getImagePool()
		
		obj1 = dat.addObject("0")
		imgid = pool.addResourceFromFile('content/gfx/objects/mushroom/mushroom_007.png')
		print imgid
		obj1.setStaticImageId(imgid)
		
		obj2 = dat.addObject("1")
		imgid = pool.addResourceFromFile('content/gfx/tiles/ground/earth_1.png')
		print imgid
		obj2.setStaticImageId(imgid)
		
		layer = elev.addLayer(self.grid)
		layer.addInstance(obj1, fife.Point(4,4))
		layer.addInstance(obj2, fife.Point(5,6))
		layer.addInstance(obj2, fife.Point(5,4))
		
		self.camloc = fife.Location()
		self.camloc.elevation = elev
		self.camloc.layer = layer
		self.camloc.position = fife.Point(5,5)

		
	def tearDown(self):
		del self.engine

	def testCamera(self):
		cam = fife.Camera()
		cam.setLocation(self.camloc)
		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight())
		cam.setViewPort(viewport)
		self.engine.getView().addCamera(cam)
		cam
		self.engine.initializePumping()
		for i in xrange(100):
			self.engine.pump()
		self.engine.finalizePumping()
		self.engine.getView().removeCamera(cam)
	

TEST_CLASSES = [TestView]

if __name__ == '__main__':
    unittest.main()

