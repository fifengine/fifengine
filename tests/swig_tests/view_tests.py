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
		obj1.setStaticImageId(imgid)
		
		obj2 = dat.addObject("1")
		imgid = pool.addResourceFromFile('content/gfx/tiles/ground/earth_1.png')
		obj2.setStaticImageId(imgid)
		
		img = pool.getImage(imgid)
		self.screen_cell_w = img.getWidth()
		
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
		cam.setScreenCellWidth(self.screen_cell_w)
		cam.setRotation(45)
		cam.setTilt(45)
		cam.setLocation(self.camloc)
		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight())
		cam.setViewPort(viewport)
		self.engine.getView().addCamera(cam)
		self.engine.initializePumping()
		
		self.camloc.position.x -= 300
		self.camloc.position.y -= 300
		cam.setLocation(self.camloc)
		
		for i in xrange(200):
			cam.setTilt(cam.getTilt() + 1)
			print "------------------------------------------> " + str(cam.getTilt())
			
			self.engine.pump()
		self.engine.finalizePumping()
		self.engine.getView().removeCamera(cam)
	

TEST_CLASSES = [TestView]

if __name__ == '__main__':
    unittest.main()

