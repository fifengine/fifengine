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
		layer.addInstance(obj1, fife.Point(0,0))
		layer.addInstance(obj2, fife.Point(1,1))
		layer.addInstance(obj2, fife.Point(2,2))
		
		self.camloc = fife.Location()
		self.camloc.elevation = elev
		self.camloc.layer = layer
		self.camloc.position = fife.Point(0,0)

		
	def tearDown(self):
		del self.engine

	def testCamera(self):
		cam = fife.Camera()
		cam.setScreenCellWidth(60)
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
		outpt = cam.toScreenCoords(fife.DoublePoint(1,1))
		print "1,1 = %d, %d" % (outpt.x, outpt.y)
		outpt = cam.toScreenCoords(fife.DoublePoint(1,200))
		print "1,2 = %d, %d" % (outpt.x, outpt.y)
		
		for i in xrange(400):
			cam.setRotation(cam.getRotation() + 80)
			cam.setZoom(cam.getZoom() + 0.004)
			self.camloc.position.x += 1
			cam.setLocation(self.camloc)
			#cam.setTilt(cam.getTilt() + 1)
			#print "current rotation " + str(cam.getRotation())
			
			self.engine.pump()
		self.engine.finalizePumping()
		self.engine.getView().removeCamera(cam)
	
		

TEST_CLASSES = [TestView]

if __name__ == '__main__':
    unittest.main()

