#!/usr/bin/env python
from swig_test_utils import *
import math

class TestModel(unittest.TestCase):
	
	def setUp(self):
		self.model = fife.Model()
		self.metamodel = self.model.getMetaModel()

	def testModel(self):
		map1 = self.model.createMap("map001")
		map2 = self.model.createMap("map002")
		
		map1.set("Name", "Map1")
		map2.set("Name", "Map2")
		
		self.assertEqual(map1.get("Name"), "Map1")
		self.assertEqual(map2.get("Name"), "Map2")

		map_query = self.model.getMaps("Name", "Map1")
		self.assertEqual(len(map_query), 1)

		map_query = self.model.getMaps("Name", "Map2")
		self.assertEqual(len(map_query), 1)

		map_query = self.model.getMaps()
		self.assertEqual(len(map_query), 2)

		self.assertEqual(len(map_query), self.model.getNumMaps())

		self.model.deleteMap(map2)

		map_query = self.model.getMaps()
		self.assertEqual(len(map_query), 1)

		self.model.createMap("map003")
		self.model.createMap("map004")
		
		map_query = self.model.getMaps()
		self.assertEqual(len(map_query), 3)
		self.assertEqual(self.model.getNumMaps(), 3)
		
		self.model.deleteMaps()
		map_query = self.model.getMaps()
		self.assertEqual(len(map_query), 0)
		self.assertEqual(self.model.getNumMaps(), 0)

	def testMaps(self):
		map = self.model.createMap("map005")
		map.set("Name", "MyMap")

		self.assertEqual(map.getNumLayers(), 0)

		dat = self.metamodel.createDataset("dataset001")
		grid = fife.SquareGrid()

		layer1 = map.createLayer("layer001", grid)
		layer2 = map.createLayer("layer002", grid)

		layer1.set("Name", "Layer1")
		layer2.set("Name", "Layer2")

		self.assertEqual(layer1.get("Name"), "Layer1")
		self.assertEqual(layer2.get("Name"), "Layer2")

		self.assertEqual(len(map.getLayers("Name", "Layer1")), 1)

		self.assertEqual(map.getNumLayers(), 2)
		map.deleteLayer(layer2)
		self.assertEqual(map.getNumLayers(), 1)
		map.deleteLayers()
		self.assertEqual(map.getNumLayers(), 0)

	def testLayers(self):
		map = self.model.createMap("map006")

		dat = self.metamodel.createDataset("dataset002")
		grid = fife.SquareGrid()
		obj1 = dat.createObject("object001")
		obj1.set("Name", "MyHero")
		obj2 = dat.createObject("object002")
		obj2.set("Name", "Goon")

		self.assertEqual(obj1.get("id"), "object001")
		self.assertEqual(obj2.get("id"), "object002")

		layer = map.createLayer("layer003", grid)

		self.assertEqual(layer.hasInstances(), 0)
		#self.assertEqual(layer.getMap(), map)

		inst = layer.createInstance(obj1, fife.ModelCoordinate(4,4))
		layer.createInstance(obj2, fife.ModelCoordinate(5,6))
		layer.createInstance(obj2, fife.ModelCoordinate(5,4))
		
		query = layer.getInstances("Name", "Goon")
		self.assertEqual(len(query), 2)
		self.assertEqual(len(layer.getInstances()), 3)

		self.assertEqual(query[0].get("Name"), "Goon")
		p1 = fife.ModelCoordinate(4,4)
		print p1.x, p1.y
		p2 = inst.getLocation().getLayerCoordinates()
		print p2.x, p2.y
		self.assertEqual(inst.getLocation().getLayerCoordinates(), fife.ModelCoordinate(4,4))
		
	def testMetaModel(self):
		dat1 = self.metamodel.createDataset("dataset003")
		dat2 = self.metamodel.createDataset("dataset004")
		dat1.set("Name", "Dat1")
		dat2.set("Name", "Dat2")

		self.assertEqual(dat1.get("Name"), "Dat1")
		self.assertEqual(dat2.get("Name"), "Dat2")

		meta_query = self.metamodel.getDatasets("Name", "Dat1")
		self.assertEqual(len(meta_query), 1)

		meta_query = self.metamodel.getDatasets("Name", "Dat2")
		self.assertEqual(len(meta_query), 1)

	def testDatasets(self):
		dat = self.metamodel.createDataset("dataset005")

		obj1 = dat.createObject("1")
		obj2 = dat.createObject("2")
		obj3 = dat.createObject("3")

		obj1.set("Name", "MyHero")
		obj1.set("Hitpoints", '100')
		obj1.set("Type", "Humanoid")

		obj2.set("Name", "Zombie")
		obj2.set("Hitpoints", '40')
		obj2.set("Type", "Humanoid")

		obj3.set("Name", "Box")
		obj3.set("Type", "Inanimate")

		meta_query = self.metamodel.getObjects("id", "1")
		self.assertEqual(len(meta_query), 1)
		self.assertEqual(meta_query[0].get("Name"), "MyHero")

		meta_query = self.metamodel.getObjects("Name", "MyHero")
		self.assertEqual(len(meta_query), 1)
		self.assertEqual(meta_query[0].get("Name"), "MyHero")
		
		meta_query = self.metamodel.getObjects("Type", "Humanoid")
		self.assertEqual(len(meta_query), 2)

		meta_query = self.metamodel.getObjects("Name", "Box")
		obj = meta_query[0]
		self.assertEqual(obj.get("Type"), "Inanimate")

class TestActionAngles(unittest.TestCase):
	def setUp(self):
		self.runaction = fife.Action("action001")
		fife.ActionVisual.create(self.runaction)
		self.runaction.get2dGfxVisual().addAnimation(90, 1)
		self.runaction.get2dGfxVisual().addAnimation(0, 0)
		self.runaction.get2dGfxVisual().addAnimation(270, 3)
		self.runaction.get2dGfxVisual().addAnimation(180, 2)
		self.walkaction = fife.Action("action002")
		fife.ActionVisual.create(self.walkaction)
		self.walkaction.get2dGfxVisual().addAnimation(70, 1)
		self.walkaction.get2dGfxVisual().addAnimation(200, 2)
		self.walkaction.get2dGfxVisual().addAnimation(320, 3)

	def testRunAngle89(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(89), 1)
	
	def testRunAngle90(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(90), 1)
	
	def testRunAngle91(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(91), 1)
	
	def testRunAngle135(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(135), 2)
	
	def testRunAngle134(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(134), 1)
	
	def testRunAngle136(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(136), 2)
	
	def testRunAngle0(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(0), 0)
	
	def testRunAngle40(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(40), 0)
	
	def testRunAngle45(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(45), 1)
	
	def testRunAngle270(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(270), 3)

	def testRunAngle269(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(269), 3)
	
	def testRunAngle271(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(271), 3)
	
	def testRunAngle314(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(314), 3)
	
	def testRunAngle359(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(359), 0)
	
	def testRunAngle400(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(400), 0)
	
	def testRunAngle451(self):
		self.assertEqual(self.runaction.get2dGfxVisual().getAnimationIndexByAngle(451), 1)
	
	def testWalkAngle0(self):
		self.assertEqual(self.walkaction.get2dGfxVisual().getAnimationIndexByAngle(0), 3)

	def testWalkAngle60(self):
		self.assertEqual(self.walkaction.get2dGfxVisual().getAnimationIndexByAngle(60), 1)

	def testWalkAngle199(self):
		self.assertEqual(self.walkaction.get2dGfxVisual().getAnimationIndexByAngle(199), 2)

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
		map = fife.Map("map007")
		self.layer = map.addLayer("layer010", grid)
		
		self.target = fife.Location()
		self.target.setLayer(self.layer)
		self.target.setPosition(fife.ModelCoordinate(10,10))
		
		self.obj = fife.Object("object010")
		self.pather = fife.LinearPather()
		self.obj.setPather(self.pather)
		self.inst = self.layer.addInstance(self.obj, fife.ModelCoordinate(4,4))
		self.action = self.obj.addAction('action010', 'run')
		self.action.addAnimation(0, 1)
		self.action.thisown = 0
		self.listener = InstanceListener()
		self.inst.addListener(self.listener)
		
	def testMovingAction(self):
		self.inst.move('run', self.target, 0.5)
		for i in xrange(30):
			self.inst.update()
		self.assert_(self.listener.finished)

	def testNonMovingAction(self):
		self.inst.move('run', fife.ModelCoordinate(0,0))
		self.inst.update()
		self.assert_(self.listener.finished)

class GridTests(unittest.TestCase):
	def _testgrid(self, grid, curpos, access, cost):
		for k, v in access.items():
			print k, v
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
		grid = fife.HexGrid()
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
