#!/usr/bin/python

from __init__ import *

class TestModel(unittest.TestCase):
	
	def setUp(self):
		self.model = fife.Model()
		self.metamodel = self.model.getMetaModel()

	def testAddingMaps(self):
		map1 = self.model.addMap()
		map2 = self.model.addMap()
		
		map1.set_string("Name", "Map1")
		map2.set_string("Name", "Map2")
		
		self.assertEqual(map1.get_string("Name"), "Map1")
		self.assertEqual(map2.get_string("Name"), "Map2")

TEST_CLASSES = [TestModel]

if __name__ == '__main__':
    unittest.main()
