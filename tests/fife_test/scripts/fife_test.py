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

import sys, os, re, math, random, shutil, time
from datetime import datetime

from fife import fife
from fife.extensions import *
from fife.extensions import pychan
from fife.extensions.pychan.pychanbasicapplication import PychanApplicationBase
from fife.extensions.pychan import widgets
from fife.extensions.pychan.internal import get_manager
from fife.extensions.fife_utils import getUserDataDirectory

import test

class KeyFilter(fife.IKeyFilter):
	"""
	This is the implementation of the fife.IKeyFilter class.

	Prevents any filtered keys from being consumed by fifechan.
	"""
	def __init__(self, keys):
		fife.IKeyFilter.__init__(self)
		self._keys = keys

	def isFiltered(self, event):
		return event.getKey().getValue() in self._keys

class ApplicationListener(fife.IKeyListener, fife.ICommandListener, fife.ConsoleExecuter):
	"""
	Listens for window commands, console commands and keyboard input.

	"""
	def __init__(self, application, testmanager):
		"""
		Initializes all listeners and registers itself with the eventmanager.
		"""
		self._application = application
			
		self._engine = self._application.engine
		self._eventmanager = self._engine.getEventManager()
		
		self._testmanager = testmanager

		fife.IKeyListener.__init__(self)
		self._eventmanager.addKeyListener(self)

		fife.ICommandListener.__init__(self)
		self._eventmanager.addCommandListener(self)

		self._console = get_manager().getConsole()
		
		fife.ConsoleExecuter.__init__(self)
		self._console.setConsoleExecuter(self)

		keyfilter = KeyFilter([fife.Key.ESCAPE, fife.Key.F10, fife.Key.PRINT_SCREEN])
		keyfilter.__disown__()		

		self._eventmanager.setKeyFilter(keyfilter)

		self.quit = False
		
		# Init Pychan
		pychan.loadFonts("data/fonts/freefont.fontdef")
		pychan.loadFonts("data/fonts/samanata.fontdef")
		pychan.manager.setDefaultFont("FreeSans")
		pychan.setupModalExecution(self._application.mainLoop, self._application.breakFromMainLoop)

	def keyPressed(self, event):
		"""
		Processes any non game related keyboard input.
		"""
		if event.isConsumed():
			return

		keyval = event.getKey().getValue()
		keystr = event.getKey().getAsString().lower()

		if keyval == fife.Key.ESCAPE:
			self.quit = True
			event.consume()
		elif keyval == fife.Key.F10:
			get_manager().getConsole().toggleShowHide()
			event.consume()
		elif keyval == fife.Key.PRINT_SCREEN:
			self._engine.getRenderBackend().captureScreen(time.strftime("%Y%m%d_%H%M%S", time.localtime()) + ".png")
			event.consume()

	def keyReleased(self, event):
		pass

	def onCommand(self, command):
		if command.getCommandType() == fife.CMD_QUIT_GAME:
			self.quit = True
			command.consume()

	def onConsoleCommand(self, command):
		result = "==OK=="
		
		args = command.split(" ")
		cmd = []
		
		for arg in args:
			arg = arg.strip()
			if arg != "":
				cmd.append(arg)
				
		if cmd[0].lower() in ('quit', 'exit'):
			self.quit = True
			self._testmanager.stopTest()
			result = "quitting."

		elif cmd[0].lower() in ('help', 'h'):
			if len(cmd) == 2:
				found = False
				for test in self._testmanager.tests:
					if test.getName().lower() == cmd[1].lower():
						self._console.println( test.getHelp() )
						found = True
						break
				if not found:
					result = "No help available for: " + cmd[1]
						
			else:
				self._console.println( open( 'data/help/fife_test.txt', 'r' ).read() )
		
		elif cmd[0].lower() in ('clear', 'cls'):
			self._console.clear()
			
		elif cmd[0].lower() in ('list', 'ls'):
			self._console.println( "List of test modules:" )
			for test in self._testmanager.tests:
				self._console.println( "\'" + test.getName() + "\' - " + test.getDescription() )

		elif cmd[0].lower() in ('run', 'r'):
			runtest = None
			
			if len(cmd) != 2:
				result = "Invalid number of arguments!"
				return result
			
			if self._testmanager.runningtest:
				result = self._testmanager.runningtest.getName() + " is currently running.  Please stop the test before running another one!"
				return result

			for test in self._testmanager.tests:
				if test.getName().lower() == cmd[1].lower():
					runtest = test
			
			if runtest:
				self._console.println( "Running test " + runtest.getName() )
				self._testmanager.runTest(runtest)	
				get_manager().getConsole().toggleShowHide()
			else:
				result = "Test " + cmd[1] + " was not found!"
			
				
		elif cmd[0].lower() in ('stop', 'st'):
			if self._testmanager.runningtest:
				self._console.println("Stopping running test..")
				self._testmanager.stopTest()
			else:
				result = "Nothing is running!"
			
		elif cmd[0].lower() in ('reset', 'rst'):
			if self._testmanager.runningtest:
				self._console.println("Resetting test " + self._testmanager.runningtest.getName() + "...")
				self._testmanager.resetTest()
				get_manager().getConsole().toggleShowHide()
			else:
				result = "Nothing is running!"
			
		elif cmd[0].lower() in ('lsrunning', 'lsrun', 'running'):
			if self._testmanager.runningtest:
				self._console.println( "Current running test: " + self._testmanager.runningtest.getName() )
			else:
				self._console.println( "No test currently running." )
		else:
			if self._testmanager.runningtest:
				result = self._testmanager.runningtest.onConsoleCommand(cmd)
			else:
				result = cmd[0] + ": not found."
			
		return result

	def onToolsClick(self):
		print "No tools set up yet"

class FifeTestApplication(PychanApplicationBase):
	"""
	The main application.  It inherits fife.extensions.ApplicationBase.

	Implements ApplicationBase._pump().
	"""
	def __init__(self, TDS):
		super(FifeTestApplication,self).__init__(TDS)
		self._settings = TDS

		#initialize TestManager and load all test plugins
		self._testmanager = test.TestManager(self.engine, self, self._settings)

	def createListener(self):
		"""
		@note: This function had to be overloaded otherwise the default
		listener would have been created.
		"""
		self._listener = ApplicationListener(self, self._testmanager)
		return self._listener

	def requestQuit(self):
		"""
		Sends the quit command to the application's listener.  We could set
		self.quitRequested to true also but this is a good example on how
		to build and dispatch a fife.Command.
		"""
		cmd = fife.Command()
		cmd.setSource(None)
		cmd.setCommandType(fife.CMD_QUIT_GAME)
		self.engine.getEventManager().dispatchCommand(cmd)

	def _pump(self):
		if self._listener.quit:
			self.quit()
		else:
			if self.testmanager.runningtest:
				self.testmanager.runningtest.pump()


	def _getLogManager(self):
		return self._log

	def _getTestManager(self):
		return self._testmanager

	logger = property(_getLogManager)
	testmanager = property(_getTestManager)
