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


TEST_CLASSES = [TestModel]

if __name__ == '__main__':
    unittest.main()
