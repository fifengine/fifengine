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

import os

class TestManager(object):
	def __init__(self, engine, application, settings):
		self._engine = engine
		self._application = application
		self._settings = settings
		
		self._running = None

		self._testdir = "tests"
		self._tests = []

		files = []
		for f in os.listdir(self._testdir):
			path = os.path.join(self._testdir, f)
			if os.path.isfile(path) and os.path.splitext(f)[1] == ".py" and f != "__init__.py":
				files.append(os.path.splitext(f)[0])

		for f in files:
			importtest = self._settings.get("Tests", f, False)
			if importtest:
				try:
					print "Importing test plugin: ", f
					exec "import " + self._testdir + "." + f
					test = eval(self._testdir + "." + f + "." + f + "()")
					if isinstance(test, Test) is False:
						print f + " is not an instance of Test!"
					else:
						self._tests.append(test)
				except BaseException, error:
					print "Error: ", error
					print "Invalid test: ", f
			else:
				print "Not importing test: ", f

			self._settings.set("Tests", f, importtest)

	def _getRunningTest(self):
		return self._running
	
	def runTest(self, test):
		if test in self._tests and not self._running:
			self._running = test
			self._running.create(self._engine, self._application)
			self._running.run()
	
	def stopTest(self):
		if self._running:
			if self._running.isRunning():
				self._running.stop()
				self._running.destroy()
			
			self._running = None	

	def resetTest(self):
		if self._running:
			if self._running.isRunning():
				self._running.stop()
			
			self._running.destroy()
			self._running.create(self._engine, self._application)
			self._running.run()				
		
	def _getTests(self):
		return self._tests
			
	def _getTestNameList(self):
		namelist = []
		for t in self._tests:
			namelist.append(t.getName())
			
		return namelist
	
	tests = property(_getTests)
	testnames = property(_getTestNameList)
	runningtest = property(_getRunningTest)
	
class Test(object):
	""" The base calss for all tests.  All tests must override these functions! """

	def create(self, engine, application):
		raise NotImplementedError, "Test has not implemented the init() function!"
	
	def destroy(self):
		raise NotImplementedError, "Test has not implemented the destroy() function!"
	
	def run(self):
		raise NotImplementedError, "Test has not implemented the run() function!"

	def stop(self):
		raise NotImplementedError, "Test has not implemented the stop() function!"

	def isRunning(self):
		raise NotImplementedError, "Test has not implemented the isRunning() function!"

	def getName(self):
		raise NotImplementedError, "Test has not implemented the getName() function!"

	def getAuthor(self):
		return "unknown"

	def getDescription(self):
		return "none"

	def getHelp(self):
		return "You're on your own for this one!"
		
	def onConsoleCommand(self, cmd):
		return cmd[0] + ": not found."
		
	def pump(self):
		pass
	

