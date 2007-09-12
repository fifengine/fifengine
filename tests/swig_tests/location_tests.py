#!/usr/bin/python
from swig_test_utils import *
import fifelog

P = fife.Point
D = fife.DoublePoint

NEAR_TRESHOLD = 0.001
def is_near(x, y):
	return (y-NEAR_TRESHOLD) < x < (y+NEAR_TRESHOLD)

def pt_is_near(p1, p2):
	return ((p2.x-NEAR_TRESHOLD) < p1.x < (p2.x+NEAR_TRESHOLD)) and \
	       ((p2.y-NEAR_TRESHOLD) < p1.y < (p2.y+NEAR_TRESHOLD))

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
	
	def testBasicMapping(self):
		self.loc1.setLayerCoordinates(P(5,5))
		pt = self.loc1.getLayerCoordinates()
		self.assertEqual(pt, P(5,5))
		pt = self.loc1.getExactLayerCoordinates()
		self.assertEqual(pt, D(5,5))
		pt = self.loc1.getElevationCoordinates()
		self.assertEqual(pt, D(5,5))
		pt = self.loc1.getLayerCoordinates(self.layer2)
		self.assertEqual(pt, P(5,5))
		pt = self.loc1.getExactLayerCoordinates(self.layer2)
		self.assertEqual(pt, D(5,5))
	
	def testSquareGridScale(self):
		self.squaregrid2.setScale(0.2)
		self.loc1.setLayerCoordinates(P(3,3))
		pt = self.loc1.getLayerCoordinates(self.layer2)
		self.assertEqual(pt, P(15,15))
		pt = self.loc1.getExactLayerCoordinates(self.layer2)
		self.assertEqual(pt, D(15,15))
		
	def testSquareGridRotation(self):
		self.squaregrid1.setRotation(90)
		self.loc1.setLayerCoordinates(P(3,3))
		pt = self.loc1.getElevationCoordinates()
		self.assertEqual(pt, D(3,-3))

	def testSquareGridShifts(self):
		self.squaregrid1.setXShift(-3)
		self.squaregrid1.setYShift(-3)
		self.loc1.setLayerCoordinates(P(3,3))
		pt = self.loc1.getElevationCoordinates()
		self.assertEqual(pt, D(0,0))

	def testSquareCombinations(self):
		# correct order in combinations = scale, rotate, translate
		self.squaregrid1.setXShift(2)
		self.squaregrid1.setYShift(2)
		self.squaregrid1.setRotation(90)
		self.squaregrid1.setScale(5)
		self.loc1.setLayerCoordinates(P(1,1))
		pt = self.loc1.getElevationCoordinates()
		self.assert_(is_near(pt.x, 7.0))
		self.assert_(is_near(pt.y, 3.0))

class TestHexGrid(unittest.TestCase):
	def setUp(self):
		self.hexgrid1 = fife.HexGrid()
		self.squaregrid1 = fife.SquareGrid()
		self.hex_layer1 = fife.Layer("hexlayer", None, self.hexgrid1)
		self.square_layer1 = fife.Layer("squarelayer", None, self.squaregrid1)
		self.loc1 = fife.Location()
		self.loc1.setLayer(self.hex_layer1)
		self.loc2 = fife.Location()
		self.loc2.setLayer(self.square_layer1)
		self.engine = fife.Engine()
		self.log = fifelog.LogManager(self.engine)
		self.log.setVisibleModules('hexgrid')

	def tearDown(self):
		del self.engine
	
	def _testHexGrid(self):
		pass



TEST_CLASSES = [TestHexGrid]

if __name__ == '__main__':
    unittest.main()
