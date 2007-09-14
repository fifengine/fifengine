#!/usr/bin/python
from swig_test_utils import *
import time

class TestView(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine()
		map = self.engine.getModel().addMap("map001")
		elev = map.addElevation("elevation001")
		
		dat = self.engine.getModel().getMetaModel().addDataset("dataset001")
		self.grid = fife.SquareGrid()
		pool = self.engine.getImagePool()
		
		self.obj1 = dat.addObject("0")
		imgid = pool.addResourceFromFile('content/gfx/objects/mushroom/mushroom_007.png')
		self.obj1.addStaticImage(0, imgid)
		
		self.obj2 = dat.addObject("1")
		imgid = pool.addResourceFromFile('content/gfx/tiles/ground/earth_1.png')
		self.obj2.addStaticImage(0, imgid)
		
		img = pool.getImage(imgid)
		self.screen_cell_w = img.getWidth()
		self.screen_cell_h = img.getHeight()
		
		self.layer = elev.addLayer("layer001", self.grid)
		
		self.camloc = fife.Location()
		self.camloc.setLayer(self.layer)
		self.camloc.setExactLayerCoordinates(fife.ExactModelCoordinate(2,0))

		
	def tearDown(self):
		del self.engine

	def testCamera(self):
		cam = fife.Camera()
		cam.setCellImageDimensions(self.screen_cell_w, self.screen_cell_h)
		cam.setRotation(45)
		cam.setTilt(40)
		cam.setLocation(self.camloc)
		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight())
		cam.setViewPort(viewport)
		self.engine.getView().addCamera(cam)
		self.engine.initializePumping()
		
		for y in xrange(4):
			for x in xrange(4):
				self.layer.addInstance(self.obj2, fife.ModelCoordinate(x,y))
				self.engine.pump()
				time.sleep(0.01)
		self.layer.addInstance(self.obj1, fife.ModelCoordinate(0,0))
		self.layer.addInstance(self.obj1, fife.ModelCoordinate(2,1))
		
		for i in xrange(120):
			if i > 20 and i < 30:
				cam.setRotation(cam.getRotation() + 1)
			elif i > 30 and i < 40:
				cam.setRotation(cam.getRotation() - 1)
			elif i > 40 and i < 50:
				if i % 2 == 0:
					c = self.camloc.getExactLayerCoordinates()
					c.x += 0.1
					c = self.camloc.setExactLayerCoordinates(c)
				cam.setLocation(self.camloc)
			elif i > 50 and i < 60:
				if i % 2 == 0:
					c = self.camloc.getExactLayerCoordinates()
					c.x -= 0.1
					c = self.camloc.setExactLayerCoordinates(c)
				cam.setLocation(self.camloc)
			elif i > 60 and i < 70:
				cam.setTilt(cam.getTilt() + 1)
			elif i > 70 and i < 80:
				cam.setTilt(cam.getTilt() - 1)
			elif i > 80 and i < 90:	
				cam.setZoom(cam.getZoom() + 0.010)
			elif i > 90 and i < 100:	
				cam.setZoom(cam.getZoom() - 0.010)
			self.engine.pump()
			time.sleep(0.05)
		self.engine.finalizePumping()
		self.engine.getView().removeCamera(cam)
	
		

TEST_CLASSES = [TestView]

if __name__ == '__main__':
    unittest.main()

