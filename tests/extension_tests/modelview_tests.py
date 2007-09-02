#!/usr/bin/python
from extension_test_utils import *
from loaders import *
import time

class TestLoaders(unittest.TestCase):

	def setUp(self):
		self.engine = fife.Engine()
		self.model = self.engine.getModel()
		self.model.thisown = 0
		self.metamodel = self.model.getMetaModel()
		self.metamodel.thisown = 0

		loadMapFile("content/maps/new_official_map.xml", self.engine)
	
		self.map = self.model.getMapsByString("id", "OfficialMap")[0]
		self.map.thisown = 0

		self.elevation = self.map.getElevationsByString("id", "OfficialMapElevation")[0]
		self.elevation.thisown = 0

		self.layer = self.elevation.getLayersByString("id", "OfficialMapTileLayer")[0]
		self.layer.thisown = 0

		img = self.engine.getImagePool().getImage(self.layer.getInstances()[0].getObject().getStaticImageId())
		self.screen_cell_w = img.getWidth()

		self.camloc = fife.Location()
		self.camloc.elevation = self.elevation
		self.camloc.layer = self.layer
		self.camloc.position = fife.Point(-2,-2)

	def tearDown(self):
		del self.engine

	def testModelView(self):

		cam = fife.Camera()
		cam.setScreenCellWidth(int(self.screen_cell_w/1.41))
		cam.setRotation(45)
		cam.setTilt(40)
		cam.setLocation(self.camloc)
		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight())
		cam.setViewPort(viewport)
		self.engine.getView().addCamera(cam)
		self.engine.initializePumping()

		for count in range(25):
			self.engine.pump()
			time.sleep(0.01)

TEST_CLASSES = [TestLoaders]

if __name__ == '__main__':
	    unittest.main()
