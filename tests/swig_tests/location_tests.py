#!/usr/bin/python
from swig_test_utils import *

class TestLocation(unittest.TestCase):
	def setUp(self):
		self.model = fife.Model()
		self.metamodel = self.model.getMetaModel()
	
	def print_pt(self, pt):
		print pt.x, pt.y
	
	def testLocation(self):
		grid1 = fife.SquareGrid()
		layer1 = fife.Layer("layer1", None, grid1)
		grid1.setRotation(10)
		loc1 = fife.Location()
		loc1.setLayer(layer1)
		loc1.setLayerCoordinates(fife.Point(1,1))
		self.print_pt(loc1.getElevationCoordinates())
		self.print_pt(loc1.getLayerCoordinates())
		

TEST_CLASSES = [TestLocation]

if __name__ == '__main__':
    unittest.main()
