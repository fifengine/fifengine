#!/usr/bin/python
from extension_test_utils import *
from loaders import *
import time

class TestModelView(unittest.TestCase):

	def setUp(self):
		self.engine = fife.Engine()
		self.model = self.engine.getModel()
		self.metamodel = self.model.getMetaModel()

		loadMapFile("content/maps/new_official_map.xml", self.engine)
	
		self.map = self.model.getMapsByString("id", "OfficialMap")[0]
		self.elevation = self.map.getElevationsByString("id", "OfficialMapElevation")[0]
		self.layer = self.elevation.getLayersByString("id", "OfficialMapTileLayer")[0]

		img = self.engine.getImagePool().getImage(self.layer.getInstances()[0].getObject().getStaticImageIndexByAngle(0))
		self.screen_cell_w = img.getWidth()
		self.screen_cell_h = img.getHeight()

		self.camloc = fife.Location()
		self.camloc.setLayer(self.layer)
		self.camloc.setLayerCoordinates(fife.Point(5,0))

	def tearDown(self):
		del self.engine

	def testModelView(self):

		cam = fife.Camera()
		cam.setCellImageDimensions(self.screen_cell_w, self.screen_cell_h)
		cam.setRotation(45)
		cam.setTilt(40)
		cam.setLocation(self.camloc)
		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight())
		cam.setViewPort(viewport)
		self.engine.getView().addCamera(cam)
		cam.thisown = 0
		self.engine.initializePumping()

		for count in range(20):
			self.engine.pump()
			time.sleep(0.3)
			c = self.camloc.getExactLayerCoordinates()
			c.x += 0.5
			c = self.camloc.setExactLayerCoordinates(c)			
			cam.setLocation(self.camloc)
			
		self.engine.finalizePumping()

TEST_CLASSES = [TestModelView]

if __name__ == '__main__':
	    unittest.main()
