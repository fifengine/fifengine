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
