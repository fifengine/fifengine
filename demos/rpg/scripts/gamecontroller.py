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

class KeyState(object):
	def __init__(self):
		self._keystate = { }
		
	def updateKey(self, key, state):
		self._keystate[key] = state
		
	def checkKey(self, key):
		if key in self._keystate:
			return self._keystate[key]
		else:
			return False
			
	def reset(self):
		self._keystate.clear()

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
			
		self._gamecontroller.keystate.updateKey(keyval, True)
		
	def keyReleased(self, event):
		keyval = event.getKey().getValue()
		keystr = event.getKey().getAsString().lower()
		
		self._gamecontroller.keystate.updateKey(keyval, False)
		
		
class GameController(object):
	def __init__(self, application, engine, settings):
		self._application = application
		self._engine = engine
		self._settings = settings
		
		self._keystate = KeyState()
		
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
		
		self._keystate.reset()
		
		if self._scene:
			self._scene.destroyScene()
			self._scene = None
		
		self._scene = Scene(self)
		self._scene.createScene(self._settings.get("RPG", "TownMapFile", "maps/town.xml"))

		#start listening to events
		self._listener.attach()
		
	def endGame(self):
		if self._scene:
			self._scene.destroyScene()
			self._scene = None		
		
	def quit(self):
		self.endGame()
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
		
	def _getScene(self):
		return self._scene
	
	def _getKeyState(self):
		return self._keystate
	
	guicontroller = property(_getGUIController) 
	engine = property(_getEngine)
	settings = property(_getSettings)
	scene = property(_getScene)
	keystate = property(_getKeyState)
