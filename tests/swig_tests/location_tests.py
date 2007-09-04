#!/usr/bin/python
from swig_test_utils import *

class TestLocation(unittest.TestCase):
	def setUp(self):
		self.squaregrid1 = fife.SquareGrid()
		self.squaregrid2 = fife.SquareGrid()
		self.layer1 = fife.Layer("layer1", None, self.squaregrid1)
		self.layer2 = fife.Layer("layer2", None, self.squaregrid2)
		self.loc1 = fife.Location()
		self.loc1.setLayer(self.layer1)
		self.loc2 = fife.Location()
		self.loc2.setLayer(self.layer2)
	
	def print_pt(self, pt):
		print pt.x, pt.y
	
	def testSquareGrid(self):
		self.squaregrid2.setScale(0.25)
		self.loc1.setLayerCoordinates(fife.Point(1,1))
		
		loc1_in_elev = self.loc1.getElevationCoordinates()
		loc1_in_layer2 = self.squaregrid2.toLayerCoordinates(loc1_in_elev)
		self.assertEqual(loc1_in_layer2.x, 4)
		self.assertEqual(loc1_in_layer2.y, 4)
		
		self.squaregrid2.setRotation(90)
		loc1_in_layer2 = self.squaregrid2.toLayerCoordinates(loc1_in_elev)
		self.assertEqual(loc1_in_layer2.x, 4)
		self.assertEqual(loc1_in_layer2.y, -4)
		
		self.squaregrid2.setRotation(180)
		loc1_in_layer2 = self.squaregrid2.toLayerCoordinates(loc1_in_elev)
		self.assertEqual(loc1_in_layer2.x, -4)
		self.assertEqual(loc1_in_layer2.y, -4)

		self.squaregrid2.setXShift(2)
		loc1_in_layer2 = self.squaregrid2.toLayerCoordinates(loc1_in_elev)
		self.assertEqual(loc1_in_layer2.x, 4)
		self.assertEqual(loc1_in_layer2.y, -4)

		self.squaregrid2.setYShift(2)
		loc1_in_layer2 = self.squaregrid2.toLayerCoordinates(loc1_in_elev)
		self.assertEqual(loc1_in_layer2.x, 4)
		self.assertEqual(loc1_in_layer2.y, 4)
		
		self.squaregrid2.setScale(1)
		self.squaregrid2.setXShift(0)
		self.squaregrid2.setYShift(0)
		self.squaregrid2.setRotation(0)
		loc1_in_elev = self.loc1.getElevationCoordinates()
		loc1_in_layer2 = self.squaregrid2.toLayerCoordinates(loc1_in_elev)
		self.assertEqual(loc1_in_layer2, self.loc1.getLayerCoordinates())


TEST_CLASSES = [TestLocation]

if __name__ == '__main__':
    unittest.main()
