#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2019 by the FIFE team
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

from __future__ import print_function
from __future__ import absolute_import
from builtins import range
from .swig_test_utils import *

class TestController(unittest.TestCase):

	def setUp(self):
		self.engine = getEngine(True)

	def tearDown(self):
		self.engine.destroy()

	def testInstances(self):
		print("\nTest SoundManager")
		self.assertTrue(self.engine.getSoundManager())
		print("Test EventManager")
		self.assertTrue(self.engine.getEventManager())
		print("Test TimeManager")
		self.assertTrue(self.engine.getTimeManager())
		print("Test ImageManager")
		self.assertTrue(self.engine.getImageManager())
		print("Test AnimationManager")
		self.assertTrue(self.engine.getAnimationManager())
		print("Test SoundClipManager")
		self.assertTrue(self.engine.getSoundClipManager())
		print("Test LogManager")
		self.assertTrue(self.engine.getLogManager())
		pass

	def testPumping(self):
		self.engine.initializePumping()
		for i in range(10):
			self.engine.pump()
		self.engine.finalizePumping()

TEST_CLASSES = [TestController]

if __name__ == '__main__':
	unittest.main()
