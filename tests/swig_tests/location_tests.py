#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from swig_test_utils import *

P = fife.ModelCoordinate
D = fife.ExactModelCoordinate

NEAR_TRESHOLD = 0.001
def is_near(x, y):
	return (y-NEAR_TRESHOLD) < x < (y+NEAR_TRESHOLD)

def pt_is_near(p1, p2):
	return ((p2.x-NEAR_TRESHOLD) < p1.x < (p2.x+NEAR_TRESHOLD)) and \
	       ((p2.y-NEAR_TRESHOLD) < p1.y < (p2.y+NEAR_TRESHOLD))

class TestLocation(unittest.TestCase):
	def setUp(self):
		self.squaregrid1 = fife.SquareGrid()
		self.squaregrid1.thisown = 0
		self.squaregrid2 = fife.SquareGrid()
		self.squaregrid2.thisown = 0
		self.layer1 = fife.Layer("layer1", None, self.squaregrid1)
		self.layer2 = fife.Layer("layer2", None, self.squaregrid2)
		self.loc1 = fife.Location(self.layer1)
		self.loc2 = fife.Location(self.layer2)
	
	def testBasicMapping(self):
		self.loc1.setLayerCoordinates(P(5,5))
		pt = self.loc1.getLayerCoordinates()
		self.assertEqual(pt, P(5,5))
		pt = self.loc1.getExactLayerCoordinates()
		self.assertEqual(pt, D(5,5))
		pt = self.loc1.getMapCoordinates()
		self.assertEqual(pt, D(5,5))
		pt = self.loc1.getLayerCoordinates(self.layer2)
		self.assertEqual(pt, P(5,5))
		pt = self.loc1.getExactLayerCoordinates(self.layer2)
		self.assertEqual(pt, D(5,5))
	
	def testSquareGridScale(self):
		self.squaregrid2.setXScale(5)
		self.squaregrid2.setYScale(5)
		self.loc1.setLayerCoordinates(P(5,5))
		pt = self.loc1.getLayerCoordinates(self.layer2)
		self.assertEqual(pt, P(1,1))
		pt = self.loc1.getExactLayerCoordinates(self.layer2)
		self.assertEqual(pt, D(5,5))
		
	def testSquareGridRotation(self):
		self.squaregrid1.setRotation(90)
		self.loc1.setLayerCoordinates(P(3,3))
		pt = self.loc1.getMapCoordinates()
		self.assertEqual(pt, D(-3,3))

	def testSquareGridShifts(self):
		self.squaregrid1.setXShift(-3)
		self.squaregrid1.setYShift(-3)
		self.loc1.setLayerCoordinates(P(3,3))
		pt = self.loc1.getMapCoordinates()
		self.assertEqual(pt, D(0,0))

	def testSquareCombinations(self):
		# correct order in combinations = translate, rotate, scale
		self.squaregrid1.setXShift(2)
		self.squaregrid1.setYShift(2)
		self.squaregrid1.setRotation(90)
		self.squaregrid1.setXScale(5)
		self.squaregrid1.setYScale(5)
		self.loc1.setLayerCoordinates(P(1,1))
		pt = self.loc1.getMapCoordinates()
		print pt 
		self.assert_(is_near(pt.x, -3))
		self.assert_(is_near(pt.y, 7))

class TestHexGrid(unittest.TestCase):
	def setUp(self):
		self.hexgrid1 = fife.HexGrid()
		self.hexgrid1.thisown = 0
		self.squaregrid1 = fife.SquareGrid()
		self.squaregrid1.thisown = 0
		self.hex_layer1 = fife.Layer("hexlayer", None, self.hexgrid1)
		self.square_layer1 = fife.Layer("squarelayer", None, self.squaregrid1)
		self.loc1 = fife.Location(self.hex_layer1)
		self.loc2 = fife.Location(self.square_layer1)
		self.engine = getEngine(True)
		self.log = fifelog.LogManager(self.engine)
		#self.log.setVisibleModules('hexgrid', 'cellgrid')

	def tearDown(self):
		del self.engine
		del self.log

	def testHexGrid0row(self):
		self.loc1.setMapCoordinates(D(-2,0))
		self.assert_(self.loc1.getLayerCoordinates() == P(-2,0))
		
		self.loc1.setMapCoordinates(D(-1,0))
		self.assert_(self.loc1.getLayerCoordinates() == P(-1,0))
		
		self.loc1.setMapCoordinates(D(0,0))
		self.assert_(self.loc1.getLayerCoordinates() == P(0,0))

		self.loc1.setMapCoordinates(D(1,0))
		self.assert_(self.loc1.getLayerCoordinates() == P(1,0))

		self.loc1.setMapCoordinates(D(2,0))
		self.assert_(self.loc1.getLayerCoordinates() == P(2,0))

	def testHexGrid1row(self):
		self.loc1.setMapCoordinates(D(-1.1,1))
		self.assert_(self.loc1.getLayerCoordinates() == P(-2,1))
		
		self.loc1.setMapCoordinates(D(-0.5,1))
		self.assert_(self.loc1.getLayerCoordinates() == P(-1,1))
		
		self.loc1.setMapCoordinates(D(-0.1,1))
		self.assert_(self.loc1.getLayerCoordinates() == P(-1,1))
		
		self.loc1.setMapCoordinates(D(0.1,1))
		self.assert_(self.loc1.getLayerCoordinates() == P(0,1))

	def testHexGridm1row(self):
		self.loc1.setMapCoordinates(D(-1.1,-1))
		self.assert_(self.loc1.getLayerCoordinates() == P(-2,-1))
		
		self.loc1.setMapCoordinates(D(-0.5,-1))
		self.assert_(self.loc1.getLayerCoordinates() == P(-1,-1))
		
		self.loc1.setMapCoordinates(D(-0.1,-1))
		self.assert_(self.loc1.getLayerCoordinates() == P(-1,-1))
		
		self.loc1.setMapCoordinates(D(0.1,-1))
		self.assert_(self.loc1.getLayerCoordinates() == P(0,-1))

	def testEdgeHits(self):
		self.loc1.setMapCoordinates(D(0.5,0.5))
		self.assert_(self.loc1.getLayerCoordinates() == P(0,1))
		
		self.loc1.setMapCoordinates(D(0.1,0.4))
		#print "______''___" + str(self.loc1.getLayerCoordinates())
		self.assert_(self.loc1.getLayerCoordinates() == P(0,0))
		
		self.loc1.setMapCoordinates(D(0.1,-0.4))
		self.assert_(self.loc1.getLayerCoordinates() == P(0,0))

		self.loc1.setMapCoordinates(D(-0.5,-0.5))
		self.assert_(self.loc1.getLayerCoordinates() == P(-1,-1))

TEST_CLASSES = [TestHexGrid, TestLocation]

if __name__ == '__main__':
    unittest.main()
