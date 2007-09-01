#!/usr/bin/python
from swig_test_utils import *
import math

class TestModel(unittest.TestCase):
	
	def setUp(self):
		self.model = fife.Model()
		self.metamodel = self.model.getMetaModel()

	def testModel(self):
		map1 = self.model.addMap("map001")
		map2 = self.model.addMap("map002")
		
		map1.set_string("Name", "Map1")
		map2.set_string("Name", "Map2")
		
		self.assertEqual(map1.get_string("Name"), "Map1")
		self.assertEqual(map2.get_string("Name"), "Map2")

		map_query = self.model.getMapsByString("Name", "Map1")
		self.assertEqual(len(map_query), 1)

		map_query = self.model.getMapsByString("Name", "Map2")
		self.assertEqual(len(map_query), 1)

		map_query = self.model.getMaps()
		self.assertEqual(len(map_query), 2)

		self.assertEqual(len(map_query), self.model.getNumMaps())

		self.model.removeMap(map2)

		map_query = self.model.getMaps()
		self.assertEqual(len(map_query), 1)

		self.model.addMap("map003")
		self.model.addMap("map004")
		
		map_query = self.model.getMaps()
		self.assertEqual(len(map_query), 3)
		self.assertEqual(self.model.getNumMaps(), 3)
		
		self.model.clearMaps()
		map_query = self.model.getMaps()
		self.assertEqual(len(map_query), 0)
		self.assertEqual(self.model.getNumMaps(), 0)

	def testMaps(self):
		map = self.model.addMap("map005")
		map.set_string("Name", "MyMap")

		elev1 = map.addElevation("elevation001")
		elev2 = map.addElevation("elevation002")

		elev1.set_string("Name", "Elev1")
		elev2.set_string("Name", "Elev2")

		self.assertEqual(elev1.get_string("Name"), "Elev1")
		self.assertEqual(elev2.get_string("Name"), "Elev2")

		query = map.getElevationsByString("Name", "Elev1")
		self.assertEqual(len(query), 1)

		query = map.getElevationsByString("Name", "Elev2")
		self.assertEqual(len(query), 1)

		self.assertEqual(map.getNumElevations(), 2)

		map.clearElevations()
		self.assertEqual(map.getNumElevations(), 0)

	def testElevations(self):
		map = self.model.addMap("map006")
		elev = map.addElevation("elevation003")
		elev.set_string("Name", "MyElevation")

		#self.assertEqual(elev.getMap(), map)
		self.assertEqual(elev.getNumLayers(), 0)

		dat = self.metamodel.addDataset("dataset001")
		grid = fife.SquareGrid()

		layer1 = elev.addLayer("layer001", grid)
		layer2 = elev.addLayer("layer002", grid)

		layer1.set_string("Name", "Layer1")
		layer2.set_string("Name", "Layer2")

		self.assertEqual(layer1.get_string("Name"), "Layer1")
		self.assertEqual(layer2.get_string("Name"), "Layer2")

		self.assertEqual(len(elev.getLayersByString("Name", "Layer1")), 1)

		self.assertEqual(elev.getNumLayers(), 2)
		elev.removeLayer(layer2)
		self.assertEqual(elev.getNumLayers(), 1)
		elev.clearLayers()
		self.assertEqual(elev.getNumLayers(), 0)

	def testLayers(self):
		map = self.model.addMap("map007")
		elev = map.addElevation("elevation004")

		dat = self.metamodel.addDataset("dataset002")
		grid = fife.SquareGrid()
		obj1 = dat.addObject("object001")
		obj1.set_string("Name", "MyHero")
		obj2 = dat.addObject("object002")
		obj2.set_string("Name", "Goon")

		self.assertEqual(obj1.oget_string("id"), "object001")
		self.assertEqual(obj2.oget_string("id"), "object002")

		layer = elev.addLayer("layer003", grid)

		self.assertEqual(layer.hasInstances(), 0)
		#self.assertEqual(layer.getElevation(), elev)

		inst = layer.addInstance(obj1, fife.Point(4,4))
		layer.addInstance(obj2, fife.Point(5,6))
		layer.addInstance(obj2, fife.Point(5,4))
		
		query = layer.getInstancesByString("Name", "Goon")
		self.assertEqual(len(query), 2)
		self.assertEqual(len(layer.getInstances()), 3)

		self.assertEqual(query[0].get_string("Name"), "Goon")
		self.assertEqual(inst.getPosition(), fife.Point(4,4))
		
	def testMetaModel(self):
		dat1 = self.metamodel.addDataset("dataset003")
		dat2 = self.metamodel.addDataset("dataset004")
		dat1.set_string("Name", "Dat1")
		dat2.set_string("Name", "Dat2")

		self.assertEqual(dat1.get_string("Name"), "Dat1")
		self.assertEqual(dat2.get_string("Name"), "Dat2")

		meta_query = self.metamodel.getDatasetsByString("Name", "Dat1")
		self.assertEqual(len(meta_query), 1)

		meta_query = self.metamodel.getDatasetsByString("Name", "Dat2")
		self.assertEqual(len(meta_query), 1)

	def testDatasets(self):
		dat = self.metamodel.addDataset("dataset005")

		obj1 = dat.addObject("1")
		obj2 = dat.addObject("2")
		obj3 = dat.addObject("3")

		obj1.set_string("Name", "MyHero")
		obj1.set_int("Hitpoints", 100)
		obj1.set_string("Type", "Humanoid")

		obj2.set_string("Name", "Zombie")
		obj2.set_int("Hitpoints", 40)
		obj2.set_string("Type", "Humanoid")

		obj3.set_string("Name", "Box")
		obj3.set_string("Type", "Inanimate")

		meta_query = self.metamodel.getObjectsByString("id", "1")
		self.assertEqual(len(meta_query), 1)
		self.assertEqual(meta_query[0].oget_string("Name"), "MyHero")

		meta_query = self.metamodel.getObjectsByString("Name", "MyHero")
		self.assertEqual(len(meta_query), 1)
		self.assertEqual(meta_query[0].oget_string("Name"), "MyHero")
		
		meta_query = self.metamodel.getObjectsByString("Type", "Humanoid")
		self.assertEqual(len(meta_query), 2)

		meta_query = self.metamodel.getObjectsByString("Name", "Box")
		obj = meta_query[0]
		self.assertEqual(obj.oget_string("Type"), "Inanimate")

class TestActionAngles(unittest.TestCase):
	def setUp(self):
		self.runaction = fife.Action("action001")
		self.runaction.addAnimation(90, 1)
		self.runaction.addAnimation(0, 0)
		self.runaction.addAnimation(270, 3)
		self.runaction.addAnimation(180, 2)
		self.walkaction = fife.Action("action002")
		self.walkaction.addAnimation(70, 1)
		self.walkaction.addAnimation(200, 2)
		self.walkaction.addAnimation(320, 3)

	def testRunAngle89(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(89), 1)
	
	def testRunAngle90(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(90), 1)
	
	def testRunAngle91(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(91), 1)
	
	def testRunAngle135(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(135), 2)
	
	def testRunAngle134(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(134), 1)
	
	def testRunAngle136(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(136), 2)
	
	def testRunAngle0(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(0), 0)
	
	def testRunAngle40(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(40), 0)
	
	def testRunAngle45(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(45), 1)
	
	def testRunAngle270(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(270), 3)

	def testRunAngle269(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(269), 3)
	
	def testRunAngle271(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(271), 3)
	
	def testRunAngle314(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(314), 3)
	
	def testRunAngle359(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(359), 0)
	
	def testRunAngle400(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(400), 0)
	
	def testRunAngle451(self):
		self.assertEqual(self.runaction.getAnimationIndexByAngle(451), 1)
	
	def testWalkAngle0(self):
		self.assertEqual(self.walkaction.getAnimationIndexByAngle(0), 3)

	def testWalkAngle60(self):
		self.assertEqual(self.walkaction.getAnimationIndexByAngle(60), 1)

	def testWalkAngle199(self):
		self.assertEqual(self.walkaction.getAnimationIndexByAngle(199), 2)

class InstanceListener(fife.InstanceListener):
	def __init__(self):
		fife.InstanceListener.__init__(self)
		self.finished = False

	def OnActionFinished(self, instance, action):
		self.finishedInstance = instance
		self.finishedAction = action
		self.finished = True

class ActivityTests(unittest.TestCase):
	def setUp(self):
		grid = fife.HexGrid()
		elev = fife.Elevation("elevation010")
		self.layer = elev.addLayer("layer010", grid)
		
		self.target = fife.Location()
		self.target.layer = self.layer
		self.target.elevation = elev
		self.target.position = fife.Point(10,10)
		
		self.obj = fife.Object("object010")
		self.pather = fife.LinearPather()
		self.obj.setPather(self.pather)
		self.inst = self.layer.addInstance(self.obj, fife.Point(4,4))
		self.action = self.obj.addAction('action010', 'run')
		self.action.addAnimation(0, 1)
		self.action.thisown = 0
		self.listener = InstanceListener()
		self.inst.addListener(self.listener)
		
	def testMovingAction(self):
		self.inst.act('run', self.target, 0.5)
		for i in xrange(30):
			self.inst.update()
		self.assert_(self.listener.finished)

	def testNonMovingAction(self):
		self.inst.act('run', fife.Point(0,0))
		self.inst.update()
		self.assert_(self.listener.finished)

class GridTests(unittest.TestCase):
	def _testgrid(self, grid, curpos, access, cost):
		for k, v in access.items():
			self.assertEqual(grid.isAccessible(fife.Point(*curpos), fife.Point(*k)), v)
		for k, v in cost.items():
			self.assertEqual(int(10000 * grid.getAdjacentCost(fife.Point(*curpos), fife.Point(*k))), 
			                 int(10000 * v))
		
		curpos = fife.Point(*curpos)
		accessiblepts = fife.PointVector()
		grid.getAccessibleCoordinates(curpos, accessiblepts)
		costpts = [fife.Point(*pt) for pt in cost.keys()]
		for pt in costpts:
			self.assert_(pt in accessiblepts)
		for pt in accessiblepts:
			self.assert_(pt in costpts)

	
	def testHexGrid(self):
		grid = fife.HexGrid()
		curpos = (1,1)
		access = {
			(0,0): True,
			(0,1): True,
			(0,2): False,
			(1,0): True,
			(1,1): True,
			(1,2): True,
			(2,0): False,
			(2,1): True,
			(2,2): True,
		}
		cost = {
			(0,0): 1,
			(0,1): 1,
			(1,0): 1,
			(1,1): 0,
			(1,2): 1,
			(2,1): 1,
			(2,2): 1,
		}
		self._testgrid(grid, curpos, access, cost)
	
	def testSquareGrid(self):
		grid = fife.SquareGrid()
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
		grid = fife.SquareGrid(True)
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
			(0,0): math.sqrt(2),
			(0,1): 1,
			(0,2): math.sqrt(2),
			(1,0): 1,
			(1,1): 0,
			(1,2): 1,
			(2,0): math.sqrt(2),
			(2,1): 1,
			(2,2): math.sqrt(2),
		}
		self._testgrid(grid, curpos, access, cost)


TEST_CLASSES = [TestModel, TestActionAngles, GridTests] #ActivityTests

if __name__ == '__main__':
    unittest.main()
