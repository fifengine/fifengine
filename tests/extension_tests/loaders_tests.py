#!/usr/bin/python
from extension_test_utils import *
from loaders import *

class TestLoaders(unittest.TestCase):

	def setUp(self):
		self.engine = fife.Engine()
		self.model = self.engine.getModel()
		self.model.thisown = 0
		self.metamodel = self.model.getMetaModel()
		self.metamodel.thisown = 0

	def tearDown(self):
		del self.engine

	def testLoading(self):

		loadMapFile("content/maps/new_official_map.xml", self.engine)

		query = self.metamodel.getObjectsByString("id", "15001")
		self.assertEqual(len(query), 1)

		query = self.metamodel.getObjectsByString("id", "15201")
		self.assertEqual(len(query), 1)

		query = self.model.getMapsByString("id", "OfficialMap")
		self.assertEqual(len(query), 1)
		self.map = query[0]
		self.map.thisown = 0

		query = self.map.getElevationsByString("id", "OfficialMapElevation")
		self.assertEqual(len(query), 1)
		self.elevation = query[0]
		self.elevation.thisown = 0

		query = self.elevation.getLayersByString("id", "OfficialMapTileLayer")
		self.assertEqual(len(query), 1)
		self.layer = query[0]
		self.layer.thisown = 0

		self.assertEqual(self.layer.hasInstances(), True)

		instances = self.layer.getInstances()

		#for inst in instances:
		#	print inst.getPosition().x, " ", inst.getPosition().y, " ", inst.getObject().get_string("id")

TEST_CLASSES = [TestLoaders]

if __name__ == '__main__':
	    unittest.main()
