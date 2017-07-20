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

from swig_test_utils import *
import time

class MyTimeEvent(fife.TimeEvent):
	def __init__(self, period):
		fife.TimeEvent.__init__(self, period)
		self.counter = 0
	
	def updateEvent(self, curtime):
		print "testing timer event... %d, %d" % (curtime, self.counter)
		self.counter += 1

class TestTimer(unittest.TestCase):
	def setUp(self):
		self.engine = getEngine(True)
		self.timemanager = self.engine.getTimeManager()

	def tearDown(self):
		self.engine.destroy()
	
	def testEvents(self):
		e = MyTimeEvent(100)
		self.timemanager.registerEvent(e)

		for i in xrange(10):
			time.sleep(0.1)
			self.timemanager.update()

		self.timemanager.unregisterEvent(e)

TEST_CLASSES = [TestTimer]

if __name__ == '__main__':
    unittest.main()

