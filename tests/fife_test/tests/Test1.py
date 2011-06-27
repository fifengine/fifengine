#!/usr/bin/env python

# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2011 by the FIFE team
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

import scripts.test as test

class Test1(test.Test):
	""" The base calss for all tests.  All tests must override these functions! """

	def create(self, application, engine):
		self._application = application
		self._engine = engine
		self._running = False
		
	def destroy(self):
		pass
			
	def run(self):
		self._running = True

	def stop(self):
		self._running = False

	def isRunning(self):
		return self._running

	def getName(self):
		return "Test1"
		
	def getAuthor(self):
		return "prock"

	def getDescription(self):
		return "this is the first test plugin ever written"

	def getLicense(self):
		return ""

	def getVersion(self):
		return "0.1"

