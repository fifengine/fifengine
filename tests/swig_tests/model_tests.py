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
import math

class TestModel(unittest.TestCase):
	
	def setUp(self):
                self.engine = getEngine(True)
		self.timeManager = self.engine.getTimeManager()
		self.model = self.engine.getModel()

	def tearDown(self):
                self.engine.destroy()
		#del self.timeManager

	def testModel(self):
		map1 = self.model.createMap("map001")
		map2 = self.model.createMap("map002")
		
		query = self.model.getMap("map001")
		self.assertEqual(map1.getId(), query.getId())

		query = self.model.getMap("map002")
		self.assertEqual(map2.getId(), query.getId())

		query = self.model.getMaps()
		self.assertEqual(len(query), 2)

		self.assertEqual(len(query), self.model.getMapCount())

		self.model.deleteMap(map2)

		query = self.model.getMaps()
		self.assertEqual(len(query), 1)

		self.model.createMap("map003")
		self.model.createMap("map004")
		
		query = self.model.getMaps()
		self.assertEqual(len(query), 3)
		self.assertEqual(self.model.getMapCount(), 3)
		
		self.model.deleteMaps()
		query = self.model.getMaps()
		self.assertEqual(len(query), 0)
		self.assertEqual(self.model.getMapCount(), 0)

	def testMaps(self):
		map = self.model.createMap("map005")

		self.assertEqual(map.getLayerCount(), 0)

		grid = self.model.getCellGrid("square")

		layer1 = map.createLayer("layer001", grid)
		layer2 = map.createLayer("layer002", grid)

		self.assertEqual(layer1.getId(), "layer001")
		self.assertEqual(layer2.getId(), "layer002")

		self.assertEqual(len(map.getLayers()), 2)

		self.assertEqual(map.getLayerCount(), 2)
		map.deleteLayer(layer2)
		self.assertEqual(map.getLayerCount(), 1)
		map.deleteLayers()
		self.assertEqual(map.getLayerCount(), 0)

	def testLayers(self):
		map = self.model.createMap("map006")

		grid = self.model.getCellGrid("square")
		obj1 = self.model.createObject("object001","test_nspace")
		obj2 = self.model.createObject("object002","test_nspace")

		self.assertEqual(obj1.getId(), "object001")
		self.assertEqual(obj2.getId(), "object002")

		layer = map.createLayer("layer003", grid)

		self.assertEqual(layer.hasInstances(), 0)
		self.assertEqual(layer.getMap().getId(), map.getId())

		inst = layer.createInstance(obj1, fife.ModelCoordinate(4,4))
		layer.createInstance(obj2, fife.ModelCoordinate(5,6))
		layer.createInstance(obj2, fife.ModelCoordinate(5,4))
		
		#query = layer.getInstances("Name", "Goon")
		#self.assertEqual(len(query), 2)
		self.assertEqual(len(layer.getInstances()), 3)

		#self.assertEqual(query[0].get("Name"), "Goon")
		#p1 = fife.ModelCoordinate(4,4)
		#print p1.x, p1.y
		#p2 = inst.getLocation().getLayerCoordinates()
		#print p2.x, p2.y
		#self.assertEqual(inst.getLocation().getLayerCoordinates(), fife.ModelCoordinate(4,4))

	def testObjects(self):
		obj1 = self.model.createObject("object003","test_nspace")
		obj2 = self.model.createObject("object004","test_nspace")
		self.model.createObject("object005","test_nspace")
		self.model.createObject("object006","test_nspace")

		self.assertEqual(self.model.deleteObject(obj2),True)

		map = self.model.createMap("map007")
		grid = self.model.getCellGrid("square")
		layer = map.createLayer("layer004",grid)

		inst = layer.createInstance(obj1, fife.ModelCoordinate(4,4))
		self.assertEqual(self.model.deleteObject(obj1),False)
		self.assertEqual(self.model.deleteObjects(),False)
		layer.deleteInstance(inst)
		self.assertEqual(self.model.deleteObject(obj1),True)
		self.assertEqual(self.model.deleteObjects(),True)

class TestActionAngles(unittest.TestCase):
	def setUp(self):
		self.engine = getEngine(True)
		self.anim0 = self.engine.getAnimationManager().create("0")
		self.anim1 = self.engine.getAnimationManager().create("1")
		self.anim2 = self.engine.getAnimationManager().create("2")
		self.anim3 = self.engine.getAnimationManager().create("3")
		self.runaction = fife.Action("action001")
		fife.ActionVisual.create(self.runaction)
		self.runaction.get2dGfxVisual().addAnimation(90, self.anim1)
		self.runaction.get2dGfxVisual().addAnimation(0, self.anim0)
		self.runaction.get2dGfxVisual().addAnimation(270, self.anim3)
		self.runaction.get2dGfxVisual().addAnimation(180, self.anim2)
		self.walkaction = fife.Action("action002")
		fife.ActionVisual.create(self.walkaction)
		self.walkaction.get2dGfxVisual().addAnimation(70, self.anim1)
		self.walkaction.get2dGfxVisual().addAnimation(200, self.anim2)
		self.walkaction.get2dGfxVisual().addAnimation(320, self.anim3)

	def testRunAngle89(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(89).getHandle(), self.anim1.getHandle())
	
	def testRunAngle90(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(90).getHandle(), self.anim1.getHandle())
	
	def testRunAngle91(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(91).getHandle(), self.anim1.getHandle())
	
	def testRunAngle135(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(135).getHandle(), self.anim1.getHandle())
	
	def testRunAngle134(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(134).getHandle(), self.anim1.getHandle())
	
	def testRunAngle136(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(136).getHandle(), self.anim2.getHandle())
	
	def testRunAngle0(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(0).getHandle(), self.anim0.getHandle())
	
	def testRunAngle40(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(40).getHandle(), self.anim0.getHandle())
	
	def testRunAngle45(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(45).getHandle(), self.anim0.getHandle())
	
	def testRunAngle270(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(270).getHandle(), self.anim3.getHandle())

	def testRunAngle269(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(269).getHandle(), self.anim3.getHandle())
	
	def testRunAngle271(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(271).getHandle(), self.anim3.getHandle())
	
	def testRunAngle314(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(314).getHandle(), self.anim3.getHandle())
	
	def testRunAngle359(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(359).getHandle(), self.anim0.getHandle())
	
	def testRunAngle400(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(400).getHandle(), self.anim0.getHandle())
	
	def testRunAngle451(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationByAngle(451).getHandle(), self.anim1.getHandle())
	
	def testWalkAngle0(self):
		self.assertEqual(self.walkaction.get2dGfxVisual().getAnimationByAngle(0).getHandle(), self.anim3.getHandle())

	def testWalkAngle60(self):
		self.assertEqual(self.walkaction.get2dGfxVisual().getAnimationByAngle(60).getHandle(), self.anim1.getHandle())

	def testWalkAngle199(self):
		self.assertEqual(self.walkaction.get2dGfxVisual().getAnimationByAngle(199).getHandle(), self.anim2.getHandle())

class GridTests(unittest.TestCase):
	def _testgrid(self, grid, curpos, access, cost):
		for k, v in access.items():
			#print(k, v)
			self.assertEqual(grid.isAccessible(fife.ModelCoordinate(*curpos), fife.ModelCoordinate(*k)), v)
		for k, v in cost.items():
			self.assertEqual(int(10000 * grid.getAdjacentCost(fife.ModelCoordinate(*curpos), fife.ModelCoordinate(*k))), 
			                 int(10000 * v))
		
		curpos = fife.ModelCoordinate(*curpos)
		accessiblepts = fife.ModelCoordinateVector()
		grid.getAccessibleCoordinates(curpos, accessiblepts)
		costpts = [fife.ModelCoordinate(*pt) for pt in cost.keys()]
		for pt in costpts:
			self.assert_(pt in accessiblepts)
		for pt in accessiblepts:
			self.assert_(pt in costpts)

	
	def testHexGrid(self):
		self.engine = getEngine(True)
		self.model = self.engine.getModel()
		grid = self.model.getCellGrid("hexagonal")
		curpos = (1,1)
		access = {
			(0,0): False,
			(0,1): True,
			(0,2): False,
			(1,0): True,
			(1,1): True,
			(1,2): True,
			(2,0): True,
			(2,1): True,
			(2,2): True,
		}
		cost = {
			(0,1): 1,
			(1,0): 1,
			(1,1): 0,
			(1,2): 1,
			(2,0): 1,
			(2,1): 1,
			(2,2): 1,
		}
		self._testgrid(grid, curpos, access, cost)
	
	def testSquareGrid(self):
		self.engine = getEngine(True)
		self.model = self.engine.getModel()
		grid = self.model.getCellGrid("square")
		grid.setAllowDiagonals(False)
		curpos = (1,1)
		access = {
			(0,0): False,
			(0,1): True,
			(0,2): False,
			(1,0): True,
			(1,1): True,
			(1,2): True,
			(2,0): False,
			(2,1): True,
			(2,2): False,
		}
		cost = {
			(0,1): 1,
			(1,0): 1,
			(1,1): 0,
			(1,2): 1,
			(2,1): 1,
		}
		self._testgrid(grid, curpos, access, cost)

	def testDiagSquareGrid(self):
		self.engine = getEngine(True)
		self.model = self.engine.getModel()
		grid = self.model.getCellGrid("square")
		grid.setAllowDiagonals(True)
		curpos = (1,1)
		access = {
			(0,0): True,
			(0,1): True,
			(0,2): True,
			(1,0): True,
			(1,1): True,
			(1,2): True,
			(2,0): True,
			(2,1): True,
			(2,2): True,
		}
		cost = {
			(0,0): 1.4,
			(0,1): 1,
			(0,2): 1.4,
			(1,0): 1,
			(1,1): 0,
			(1,2): 1,
			(2,0): 1.4,
			(2,1): 1,
			(2,2): 1.4,
		}
		self._testgrid(grid, curpos, access, cost)


TEST_CLASSES = [TestModel, TestActionAngles, GridTests]

if __name__ == '__main__':
    unittest.main()
