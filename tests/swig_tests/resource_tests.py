#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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

import sys
from swig_test_utils import *
from fife.serializers import *

class TestPool(unittest.TestCase):
	
	def setUp(self):
		self.engine = getEngine()
		
	def tearDown(self):
		self.engine.destroy()
			
	def testImagePool(self):
		pool = self.engine.getImageManager()
		self.assert_(pool)
		self.assert_(pool.getResourceCount(fife.RES_LOADED) == 0)
		id = pool.addResourceFromFile('tests/data/beach_e1.png')
		self.assertEqual(pool.getResourceCount(fife.RES_LOADED), 0)
		self.assertEqual(pool.getResourceCount(fife.RES_NON_LOADED), 1)
		img = pool.getImage(id)
		self.assertEqual(pool.getResourceCount(fife.RES_LOADED), 1)
		self.assertEqual(pool.getResourceCount(fife.RES_NON_LOADED), 0)

	def testImagePoolFail(self):
		pool = self.engine.getImagePool()
		id = pool.addResourceFromFile('bogus_image.png')
		self.assertRaises(RuntimeError,pool.getImage,id)


TEST_CLASSES = [TestPool]

if __name__ == '__main__':
    unittest.main()
