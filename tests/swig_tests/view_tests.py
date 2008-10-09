#!/usr/bin/env python
from swig_test_utils import *
import time

class TestView(unittest.TestCase):
	
	def setUp(self):
		self.engine = getEngine()
		model = self.engine.getModel()
		map = model.createMap("map001")
		
		self.grid = fife.SquareGrid()
		pool = self.engine.getImagePool()

		self.obj1 = model.createObject('0','test_nspace')
		fife.ObjectVisual.create(self.obj1)
		imgid = pool.addResourceFromFile('tests/data/mushroom_007.png')
		self.obj1.get2dGfxVisual().addStaticImage(0, imgid)
		
		self.obj2 = model.createObject('1','test_nspace')
		fife.ObjectVisual.create(self.obj2)
		imgid = pool.addResourceFromFile('tests/data/earth_1.png')
		self.obj2.get2dGfxVisual().addStaticImage(0, imgid)

		img = pool.getImage(imgid)
		self.screen_cell_w = img.getWidth()
		self.screen_cell_h = img.getHeight()
		
		self.layer = map.createLayer("layer001", self.grid)

		self.camcoord = fife.ExactModelCoordinate(2,0)
		

		
	def tearDown(self):
		self.engine.destroy()

	def testCamera(self):
		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getWidth(), rb.getHeight())

		cam = self.engine.getView().addCamera("foo", self.layer, viewport, self.camcoord )
		cam.setCellImageDimensions(self.screen_cell_w, self.screen_cell_h)
		cam.setRotation(45)
		cam.setTilt(40)
		
		cam.setViewPort(viewport)
		self.engine.getView().resetRenderers()
		
		self.engine.initializePumping()
		
		for y in xrange(4):
			for x in xrange(4):
				i = self.layer.createInstance(self.obj2, fife.ModelCoordinate(x,y))
				fife.InstanceVisual.create(i)
				self.engine.pump()
				time.sleep(0.01)
		i = self.layer.createInstance(self.obj1, fife.ModelCoordinate(0,0))
		fife.InstanceVisual.create(i)
		i = self.layer.createInstance(self.obj1, fife.ModelCoordinate(2,1))
		fife.InstanceVisual.create(i)
		
		for i in xrange(120):
			if i > 20 and i < 30:
				cam.setRotation(cam.getRotation() + 1)
			elif i > 30 and i < 40:
				cam.setRotation(cam.getRotation() - 1)
			elif i > 40 and i < 50:
				if i % 2 == 0:
					c = cam.getLocation().getExactLayerCoordinates()
					c.x += 0.1
					c = cam.getLocation().setExactLayerCoordinates(c)
			elif i > 50 and i < 60:
				if i % 2 == 0:
					c = cam.getLocation().getExactLayerCoordinates()
					c.x -= 0.1
					c = cam.getLocation().setExactLayerCoordinates(c)
			elif i > 60 and i < 70:
				cam.setTilt(cam.getTilt() + 1)
			elif i > 70 and i < 80:
				cam.setTilt(cam.getTilt() - 1)
			elif i > 80 and i < 90:	
				cam.setZoom(cam.getZoom() + 0.010)
			elif i > 90 and i < 100:	
				cam.setZoom(cam.getZoom() - 0.010)
			self.engine.pump()
		self.engine.finalizePumping()
		self.engine.getView().removeCamera(cam)
	
		

TEST_CLASSES = [TestView]

if __name__ == '__main__':
    unittest.main()

