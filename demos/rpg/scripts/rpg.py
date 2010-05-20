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
# This is the rio de hola client for FIFE.

import sys, os, re, math, random, shutil

from fife import fife
from fife.extensions import *
from scripts.gamecontroller import GameController
from scripts.common import eventlistenerbase
from fife.extensions.basicapplication import ApplicationBase
from fife.extensions import pychan
from fife.extensions.pychan import widgets
from fife.extensions.fife_utils import getUserDataDirectory

class KeyFilter(fife.IKeyFilter):
	def __init__(self, keys):
		fife.IKeyFilter.__init__(self)
		self._keys = keys

	def isFiltered(self, event):
		return event.getKey().getValue() in self._keys

class ApplicationListener(eventlistenerbase.EventListenerBase):
	def __init__(self, engine, gamecontroller):
		super(ApplicationListener, self).__init__(engine,regKeys=True,regCmd=True, regMouse=False, regConsole=True, regWidget=True)
		self._engine = engine
		self._gamecontroller = gamecontroller
		
		keyfilter = KeyFilter([fife.Key.ESCAPE])
		keyfilter.__disown__()
		
		self._engine.getEventManager().setKeyFilter(keyfilter)

		self.quit = False

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		consumed = False
		if keyval == fife.Key.ESCAPE:
			self.quit = True
			evt.consume()
		elif keyval == fife.Key.F10:
			self._engine.getGuiManager().getConsole().toggleShowHide()
			evt.consume()
		elif keystr == 'p':
			self._engine.getRenderBackend().captureScreen('screenshot.png')
			evt.consume()

	def onCommand(self, command):
		self.quit = (command.getCommandType() == fife.CMD_QUIT_GAME)
		if self.quit:
			command.consume()

	def onConsoleCommand(self, command):
		result = ''
		if command.lower() in ('quit', 'exit'):
			self.quit = True
			result = 'quitting'
		elif command.lower() in ( 'help', 'help()' ):
			self._engine.getGuiManager().getConsole().println( open( 'misc/infotext.txt', 'r' ).read() )
			result = "-- End of help --"
		else:
			pass
			result = self._gamecontroller.onConsoleCommand(command)
		if not result:
			try:
				result = str(eval(command))
			except:
				pass
		if not result:
			result = 'no result'
		return result

class RPGApplication(ApplicationBase):
	def __init__(self, TDS):
		super(RPGApplication,self).__init__(TDS)
		self._settings = TDS
		
		pychan.init(self.engine, debug=self._settings.get("FIFE", "PychanDebug", False))
		
		self._gamecontroller = GameController(self, self.engine, self._settings)

	def createListener(self):
		"""
		@note: This function had to be overloaded otherwise the default
		listener would have been created.
		"""
		self._listener = ApplicationListener(self.engine, self._gamecontroller)
		return self._listener
		
	def requestQuit(self):
		cmd = fife.Command()
		cmd.setSource(None)
		cmd.setCommandType(fife.CMD_QUIT_GAME)
		self.engine.getEventManager().dispatchCommand(cmd)

	def _pump(self):
		if self._listener.quit:
			self.breakRequested = True
		else:
			self._gamecontroller.pump()
