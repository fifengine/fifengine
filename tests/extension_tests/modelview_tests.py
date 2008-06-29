#!/usr/bin/env python
from extension_test_utils import *
from loaders import *
import time

class TestModelView(unittest.TestCase):

	def setUp(self):
		self.engine = getEngine()
		self.model = self.engine.getModel()
		self.metamodel = self.model.getMetaModel()

		loadMapFile("content/maps/new_official_map.xml", self.engine)
	
		self.map = self.model.getMaps("id", "OfficialMap")[0]
		self.elevation = self.map.getElevations("id", "OfficialMapElevation")[0]
		self.layer = self.elevation.getLayers("id", "OfficialMapTileLayer")[0]

		imgid = self.layer.getInstances()[0].getObject().get2dGfxVisual().getStaticImageIndexByAngle(0)
		img = self.engine.getImagePool().getImage(imgid)
		self.screen_cell_w = img.getWidth()
		self.screen_cell_h = img.getHeight()

		self.camloc = fife.Location(self.layer)
		self.camloc.setLayerCoordinates(fife.ModelCoordinate(5,0))

	def tearDown(self):
		del self.engine

	def testModelView(self):

		cam = self.engine.getView().addCamera()
		cam.setCellImageDimensions(self.screen_cell_w, self.screen_cell_h)
		cam.setRotation(45)
		cam.setTilt(40)
		cam.setLocation(self.camloc)
		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight())
		cam.setViewPort(viewport)
		self.engine.getView().resetRenderers()
		self.engine.initializePumping()

		for count in range(10):
			self.engine.pump()
			#time.sleep(0.03)
			c = self.camloc.getExactLayerCoordinates()
			c.x += 0.50
			c = self.camloc.setExactLayerCoordinates(c)			
			cam.setLocation(self.camloc)

		self.engine.finalizePumping()

# removed from test set now due to switch to new directory structure -> content moved to clients
# to be considered if this should be taken into use again
TEST_CLASSES = []

if __name__ == '__main__':
	    unittest.main()
