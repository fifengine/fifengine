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

TEST_CLASSES = [TestModel]

if __name__ == '__main__':
    unittest.main()
