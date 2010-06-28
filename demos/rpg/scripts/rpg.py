#!/usr/bin/env python

# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
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
from scripts.gamecontroller import GameController
from fife.extensions.basicapplication import ApplicationBase
from fife.extensions import pychan
from fife.extensions.pychan import widgets
from fife.extensions.fife_utils import getUserDataDirectory

class KeyFilter(fife.IKeyFilter):
	"""
	This is the implementation of the fife.IKeyFilter class.
	
	Prevents any filtered keys from being consumed by guichan.
	"""
	def __init__(self, keys):
		fife.IKeyFilter.__init__(self)
		self._keys = keys

	def isFiltered(self, event):
		return event.getKey().getValue() in self._keys

class ApplicationListener(fife.IKeyListener, fife.ICommandListener, fife.ConsoleExecuter):
	"""
	Listens for window commands, console commands and keyboard input.
	
	Does not process game related input.	
	"""
	def __init__(self, engine, gamecontroller):
		"""
		Initializes all listeners and registers itself with the eventmanager.
		"""
		self._engine = engine
		self._gamecontroller = gamecontroller
		self._eventmanager = self._engine.getEventManager()
		
		fife.IKeyListener.__init__(self)
		self._eventmanager.addKeyListener(self)
		
		fife.ICommandListener.__init__(self)
		self._eventmanager.addCommandListener(self)
		
		fife.ConsoleExecuter.__init__(self)
		self._engine.getGuiManager().getConsole().setConsoleExecuter(self)
		
		keyfilter = KeyFilter([fife.Key.ESCAPE, fife.Key.BACKQUOTE, fife.Key.PRINT_SCREEN])
		keyfilter.__disown__()		
		
		self._eventmanager.setKeyFilter(keyfilter)
		
		self.quit = False

	def keyPressed(self, event):
		"""
		Processes any non game related keyboar input.
		"""
		if event.isConsumed():
			return

		keyval = event.getKey().getValue()
		keystr = event.getKey().getAsString().lower()
		
		if keyval == fife.Key.ESCAPE:
			self.quit = True
			event.consume()
		elif keyval == fife.Key.BACKQUOTE:
			self._engine.getGuiManager().getConsole().toggleShowHide()
			event.consume()
		elif keyval == fife.Key.PRINT_SCREEN:
			self._engine.getRenderBackend().captureScreen(time.strftime("%Y%m%d_%H%M%S", time.localtime()) + ".png")
			event.consume()

	def keyReleased(self, event):
		pass

	def onCommand(self, command):
		self.quit = (command.getCommandType() == fife.CMD_QUIT_GAME)
		if self.quit:
			command.consume()

	def onConsoleCommand(self, command):
		result = ""
		
		args = command.split(" ")
		cmd = []
		for arg in args:
			arg = arg.strip()
			if arg != "":
				cmd.append(arg)		
		
		if cmd[0].lower() in ('quit', 'exit'):
			self.quit = True
			result = 'quitting'
		elif cmd[0].lower() in ( 'help' ):
			helptextfile = self._gamecontroller.settings.get("RPG", "HelpText", "misc/help.txt")
			self._engine.getGuiManager().getConsole().println( open( helptextfile, 'r' ).read() )
			result = "--OK--"
		elif cmd[0].lower() in ( 'eval' ):
			try:
				result = str(eval(command.lstrip(cmd[0])))
			except:
				result = "Invalid eval statement..."
		else:
			result = self._gamecontroller.onConsoleCommand(command)
			
		if not result:
			result = 'Command Not Found...'
		
		return result
		
	def onToolsClick(self):
		print "No tools set up yet"

class RPGApplication(ApplicationBase):
	"""
	The main application.  It inherits fife.extensions.ApplicationBase.
	
	Implements ApplicationBase._pump().
	"""
	def __init__(self, TDS):
		super(RPGApplication,self).__init__(TDS)
		self._settings = TDS
		
		self._gamecontroller = GameController(self, self.engine, self._settings)

	def createListener(self):
		"""
		@note: This function had to be overloaded otherwise the default
		listener would have been created.
		"""
		self._listener = ApplicationListener(self.engine, self._gamecontroller)
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
			self._gamecontroller.endGame()
			self.quit()
		else:
			self._gamecontroller.pump()
			
			
	def _getLogManager(self):
		return self._log
		
	logger = property(_getLogManager)
