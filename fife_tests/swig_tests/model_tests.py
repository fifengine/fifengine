#!/usr/bin/python

from __init__ import *

class TestModel(unittest.TestCase):
	
	def setUp(self):
		self.model = fife.Model()
		self.metamodel = self.model.getMetaModel()

	def testModel(self):
		map1 = self.model.addMap()
		map2 = self.model.addMap()
		
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

		self.model.addMap()
		self.model.addMap()
		
		map_query = self.model.getMaps()
		self.assertEqual(len(map_query), 3)
		self.assertEqual(self.model.getNumMaps(), 3)
		
		self.model.clearMaps()
		map_query = self.model.getMaps()
		self.assertEqual(len(map_query), 0)
		self.assertEqual(self.model.getNumMaps(), 0)

	def testMaps(self):
		map = self.model.addMap()
		map.set_string("Name", "MyMap")

		elev1 = map.addElevation()
		elev2 = map.addElevation()

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
		map = self.model.addMap()
		elev = map.addElevation()
		elev.set_string("Name", "MyElevation")

		#self.assertEqual(elev.getMap(), map)
		self.assertEqual(elev.getNumLayers(), 0)

		dat = self.metamodel.addDataset()
		geom = dat.addGeometryType()

		layer1 = elev.addLayer(geom)
		layer2 = elev.addLayer(geom)

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
		map = self.model.addMap()
		elev = map.addElevation()

		dat = self.metamodel.addDataset()
		geom = dat.addGeometryType()
		obj1 = dat.addObject()
		obj1.set_string("Name", "MyHero")
		obj2 = dat.addObject()
		obj2.set_string("Name", "Goon")

		layer = elev.addLayer(geom)

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
		dat1 = self.metamodel.addDataset()
		dat2 = self.metamodel.addDataset()
		dat1.set_string("Name", "Dat1")
		dat2.set_string("Name", "Dat2")

		self.assertEqual(dat1.get_string("Name"), "Dat1")
		self.assertEqual(dat2.get_string("Name"), "Dat2")

		meta_query = self.metamodel.getDatasetsByString("Name", "Dat1")
		self.assertEqual(len(meta_query), 1)

		meta_query = self.metamodel.getDatasetsByString("Name", "Dat2")
		self.assertEqual(len(meta_query), 1)

	def testDatasets(self):
		dat = self.metamodel.addDataset()

		obj1 = dat.addObject()
		obj2 = dat.addObject()
		obj3 = dat.addObject()

		obj1.set_string("Name", "MyHero")
		obj1.set_int("Hitpoints", 100)
		obj1.set_string("Type", "Humanoid")

		obj2.set_string("Name", "Zombie")
		obj2.set_int("Hitpoints", 40)
		obj2.set_string("Type", "Humanoid")

		obj3.set_string("Name", "Box")
		obj3.set_string("Type", "Inanimate")

		meta_query = self.metamodel.getObjectsByString("Name", "MyHero")
		self.assertEqual(len(meta_query), 1)
		self.assertEqual(meta_query[0].oget_string("Name"), "MyHero")
		
		meta_query = self.metamodel.getObjectsByString("Type", "Humanoid")
		self.assertEqual(len(meta_query), 2)

		meta_query = self.metamodel.getObjectsByString("Name", "Box")
		obj = meta_query[0]
		self.assertEqual(obj.oget_string("Type"), "Inanimate")

class TestActions(unittest.TestCase, fife.InstanceListener):
	
	def setUp(self):
		self.model = fife.Model()
		self.metamodel = self.model.getMetaModel()
		map = self.model.addMap()
		elev = map.addElevation()

		dat = self.metamodel.addDataset()
		geom = dat.addGeometryType()
		self.layer = elev.addLayer(geom)
		self.obj1 = dat.addObject()
		self.obj1.set_string("Name", "MyHero")

		self.runaction = self.obj1.addAction("run")
		self.runaction.addAnimation(90, 1)
		self.runaction.addAnimation(0, 0)
		self.runaction.addAnimation(270, 3)
		self.runaction.addAnimation(180, 2)
		self.walkaction = self.obj1.addAction("walk")
		self.walkaction.addAnimation(70, 1)
		self.walkaction.addAnimation(200, 2)
		self.walkaction.addAnimation(320, 3)

	def OnActionFinished(self, instance, action):
		self.finishedInstance = instance
		self.finishedAction = action
	
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

TEST_CLASSES = [TestModel, TestActions]

if __name__ == '__main__':
    unittest.main()
