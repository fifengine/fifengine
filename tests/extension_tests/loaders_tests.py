#!/usr/bin/env python
from extension_test_utils import *
from loaders import *

class TestLoaders(unittest.TestCase):

	def setUp(self):
		self.engine = getEngine()
		self.model = self.engine.getModel()
		self.metamodel = self.model.getMetaModel()

	def tearDown(self):
		del self.engine

	def testLoading(self):
		loadMapFile("content/maps/new_official_map.xml", self.engine)

		query = self.metamodel.getObjects("id", "15001")
		self.assertEqual(len(query), 1)

		query = self.metamodel.getObjects("id", "15201")
		self.assertEqual(len(query), 1)

		query = self.model.getMaps("id", "OfficialMap")
		self.assertEqual(len(query), 1)
		self.map = query[0]

#		self.assertEqual(self.map.get("Name"), "official_map.xml")
		self.assertEqual(self.map.get("Version"), '1')
		self.assertEqual(self.map.get("Author"), "barra")

		query = self.map.getElevations("id", "OfficialMapElevation")
		self.assertEqual(len(query), 1)
		self.elevation = query[0]

		query = self.elevation.getLayers("id", "OfficialMapTileLayer")
		self.assertEqual(len(query), 1)
		self.layer = query[0]

		self.assertEqual(self.layer.hasInstances(), True)

		instances = self.layer.getInstances()


# removed from test set now due to switch to new directory structure -> content moved to clients
# to be considered if this should be taken into use again
TEST_CLASSES = []

if __name__ == '__main__':
	    unittest.main()
