#!/usr/bin/python
from swig_test_utils import *
from loaders import *

print "blah"

class TestLoaders(unittest.TestCase):

	def setUp(self):
		self.model = fife.Model()
		self.model.thisown = 0
		self.metamodel = self.model.getMetaModel()
		self.metamodel.thisown = 0

	def testLoading(self):

		loadMapFile("content/maps/new_official_map.xml", self.model)

		query = self.metamodel.getObjectsByString("id", "15001")
		self.assertEqual(len(query), 1)

		query = self.metamodel.getObjectsByString("id", "15201")
		self.assertEqual(len(query), 1)


TEST_CLASSES = [TestLoaders]

if __name__ == '__main__':
	    unittest.main()
