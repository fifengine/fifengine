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

from scripts.scene import Scene
from scripts.guicontroller import GUIController

class GameListener(fife.IKeyListener, fife.IMouseListener):
	def __init__(self, gamecontroller):
		self._engine = gamecontroller.engine
		self._gamecontroller = gamecontroller
		self._settings = gamecontroller.settings
		self._eventmanager = self._engine.getEventManager()
		
		fife.IMouseListener.__init__(self)
		self._eventmanager.addMouseListener(self)
	
		fife.IKeyListener.__init__(self)
		self._eventmanager.addKeyListener(self)
		
		self._attached = False
		
	def attach(self):
		if not self._attached:
			self._eventmanager.addMouseListenerFront(self)
			self._eventmanager.addKeyListenerFront(self)
			self._attached = True
	
	def detach(self):
		if self._attached:
			self._eventmanager.removeMouseListener(self)
			self._eventmanager.removeKeyListener(self)
			self._attached = False
		
	def mousePressed(self, event):
		pass
	def mouseReleased(self, event):
		pass	
	def mouseEntered(self, event):
		pass
	def mouseExited(self, event):
		pass
	def mouseClicked(self, event):
		pass
	def mouseWheelMovedUp(self, event):
		pass	
	def mouseWheelMovedDown(self, event):
		pass
	def mouseMoved(self, event):
		pass
	def mouseDragged(self, event):
		pass
	def keyPressed(self, event):
		keyval = event.getKey().getValue()
		keystr = event.getKey().getAsString().lower()
		if keyval == fife.Key.ESCAPE:
			self.detach()
			self._gamecontroller.guicontroller.showMainMenu()
			event.consume()
		
	def keyReleased(self, event):
		pass
		
class GameController(object):
	def __init__(self, application, engine, settings):
		self._application = application
		self._engine = engine
		self._settings = settings
		
		self._guicontroller = GUIController(self)
		
		self._listener = GameListener(self)
		
		self._guicontroller.showMainMenu()
		
		self._scene = None
	
	def onConsoleCommand(self, command):
		"""
		Might be useful if you want to have the game parse a command.
		Not sure if I am going to keep this or not.
		"""
		result = ""
		return result
		
	def newGame(self):
		self._guicontroller.hideMainMenu()
		
		if self._scene:
			self._scene.destroyScene()
			self._scene = None
		
		self._scene = Scene(self)
		self._scene.createScene(self._settings.get("RPG", "TownMapFile", "maps/town.xml"))

		#start listening to events
		self._listener.attach()
		
	def quit(self):
		self._application.requestQuit()

	def pump(self):
		if self._scene:
			self._scene.updateScene()
		
		
	def _getGUIController(self):
		return self._guicontroller
		
	def _getEngine(self):
		return self._engine
		
	def _getSettings(self):
		return self._settings
		
	guicontroller = property(_getGUIController) 
	engine = property(_getEngine)
	settings = property(_getSettings)
