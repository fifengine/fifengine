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

from fife import fife
import math, random
from fife.extensions import pychan
from fife.extensions.pychan import widgets

from scripts.common.eventlistenerbase import EventListenerBase
from scripts.common.helpers import normalize
from fife.extensions.loaders import loadMapFile

from scripts.gui.guis import *

from scripts.ships.shipbase import Ship
from scripts.ships.player import Player
from scripts.scene import Scene

class World(EventListenerBase):
	"""
	The world!

	This class handles:
	  setup of map view (cameras ...)
	  loading the map
	  handles mouse/key events which aren't handled by the GUI.
	   ( by inheriting from EventlistenerBase )

	"""
	def __init__(self, app, engine):
		super(World, self).__init__(engine, regMouse=True, regKeys=True)
		self._applictaion = app
		self.engine = engine
		self.timemanager = engine.getTimeManager()
		self.eventmanager = engine.getEventManager()
		self.model = engine.getModel()
		self.filename = ''
		self.keystate = { 'UP': False, 'DOWN': False, 'LEFT': False, 'RIGHT': False, 'CTRL': False, 'SPACE': False, } 
		self.dynamic_widgets = {}
		self.pump_ctr = 0
		self.map = None
		self.scene = None
		self._paused = True
		self._pausedtime = 0
		self._starttime = 0
		
		self._mainmenu = MainMenu(self)
		self.showMainMenu()
		
		self._hudwindow = HeadsUpDisplay(self)
		self._hudwindow.hide()

	
	def showMainMenu(self):
		if self.scene:
			self._paused = True
			cont = True
		else:
			cont = False
			
		self._mainmenu.show(cont)
		
	def showCredits(self):
		pass
		
	def showHighScores(self):
		pass
		
	def quit(self):
		self.reset()
		self._applictaion.requestQuit()
		
	def reset(self):
		#TODO: ensure these get destroyed correctly
		if self.map:
			self.model.deleteMap(self.map)
		self.map = None
		
		self.cameras = {}
		self.scene = None

	def loadLevel(self, filename):
		"""
		Load a xml map and setup cameras.
		"""
		self.filename = filename
		self.reset()
		self.map = loadMapFile(self.filename, self.engine)

		self.scene = Scene(self.engine, self.map.getLayer('objects'))
		self.scene.initScene(self.map)

		self.initCameras()

		self._hudwindow.show()
		
		self._starttime = self.timemanager.getTime()
		

	def newGame(self):
		self.loadLevel("maps/shooter_map1.xml")
		self._mainmenu.hide()
		self._paused = False
		
	def continueGame(self):
		self._mainmenu.hide()
		self._paused = False
		
	def initCameras(self):
		"""
		Before we can actually see something on screen we have to specify the render setup.
		This is done through Camera objects which offer a viewport on the map.
		"""
		
		for cam in self.map.getCameras():
			if cam.getId() == 'main':
				self.cameras['main'] = cam
				
		self.scene.attachCamera(self.cameras['main'])
		

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		if keyval == fife.Key.UP:
			self.keystate['UP'] = True
		elif keyval == fife.Key.DOWN:
			self.keystate['DOWN'] = True
		elif keyval == fife.Key.LEFT:
			self.keystate['LEFT'] = True
		elif keyval == fife.Key.RIGHT:
			self.keystate['RIGHT'] = True
		elif keyval == fife.Key.SPACE:
			self.keystate['SPACE'] = True
		elif keyval == fife.Key.P:
			self._paused = not self._paused
			self._pausedtime += self.timemanager.getTime()
		elif keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
			self.keystate['CTRL'] = True

	def keyReleased(self, evt):
		keyval = evt.getKey().getValue()
		if keyval == fife.Key.UP:
			self.keystate['UP'] = False
		elif keyval == fife.Key.DOWN:
			self.keystate['DOWN'] = False
		elif keyval == fife.Key.LEFT:
			self.keystate['LEFT'] = False
		elif keyval == fife.Key.RIGHT:
			self.keystate['RIGHT'] = False
		elif keyval == fife.Key.SPACE:
			self.keystate['SPACE'] = False
		elif keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
			self.keystate['CTRL'] = False

	def mouseWheelMovedUp(self, evt):
		if self.keystate['CTRL']:
			self.cameras['main'].setZoom(self.cameras['main'].getZoom() * 1.05)

	def mouseWheelMovedDown(self, evt):
		if self.keystate['CTRL']:
			self.cameras['main'].setZoom(self.cameras['main'].getZoom() / 1.05)

	def mousePressed(self, evt):
		if evt.isConsumedByWidgets():
			return

		clickpoint = fife.ScreenPoint(evt.getX(), evt.getY())

	def mouseMoved(self, evt):
		pt = fife.ScreenPoint(evt.getX(), evt.getY())

	def pump(self):
		"""
		Called every frame.
		"""
		
		if not self.scene:
			return
		
		#update the scene
		if not self._paused:
			if self.scene.paused:
				self.scene.unpause(self.timemanager.getTime() - self._starttime)
				
			self.scene.update(self.timemanager.getTime() - self._starttime, self.keystate)
		
		
			#update the HUD
			avgframe = self.timemanager.getAverageFrameTime()
			if avgframe > 0:
				fps = 1 / (avgframe / 1000)
			else:
				fps = 0
			fpstxt = "%3.2f" % fps
			self._hudwindow.setFPSText(unicode(fpstxt))
		
			player = self.scene.player
			exactcoords = player.location.getExactLayerCoordinates()
			pos = "%1.2f" % exactcoords.x + ", %1.2f" % exactcoords.y
			self._hudwindow.setPositionText(unicode(pos))
		
			vel = "%1.2f" % player.velocity.x + ", %1.2f" % player.velocity.y
			self._hudwindow.setVelocityText(unicode(vel))
		
			score = unicode(str(player.score))
			self._hudwindow.setScoreText(score)
			
		else:
			if not self.scene.paused:
				self.scene.pause(self.timemanager.getTime() - self._starttime)
		
		self.pump_ctr += 1
