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

from scripts.ships.shipbase import Ship
from scripts.ships.player import Player
from scripts.scene import Scene
from scripts.weapons import Weapon

class World(EventListenerBase):
	"""
	The world!

	This class handles:
	  setup of map view (cameras ...)
	  loading the map
	  GUI for right clicks
	  handles mouse/key events which aren't handled by the GUI.
	   ( by inheriting from EventlistenerBase )

	That's obviously too much, and should get factored out.
	"""
	def __init__(self, engine):
		super(World, self).__init__(engine, regMouse=True, regKeys=True)
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

	def reset(self):
		"""
		Clear the agent information and reset the moving secondary camera state.
		"""
		self.cameras = {}

	def load(self, filename):
		"""
		Load a xml map and setup agents and cameras.
		"""
		self.filename = filename
		self.reset()
		self.map = loadMapFile(filename, self.engine)

		self.scene = Scene(self.engine, self.map.getLayer('objects'))
		#self.initPlayer()
		self.initCameras()
		
		self.mainwindow = pychan.loadXML('gui/mainwindow.xml')
		self.fpstext = self.mainwindow.findChild(name="fps")
		self.velocitytext = self.mainwindow.findChild(name="velocity")
		self.positiontext = self.mainwindow.findChild(name="position")
		self.mainwindow.position = (0,0)
		self.mainwindow.show()
		
		#give player the default weapon
		self.scene.player.weapon = Weapon(self.model, self.map.getLayer('objects'), self.scene.player, 100)
		
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
		
		#update the scene
		self.scene.update(self.timemanager.getTime(), self.keystate)
		
		
		#update the HUD
		avgframe = self.timemanager.getAverageFrameTime()
		if avgframe > 0:
			fps = 1 / (avgframe / 1000)
		else:
			fps = 0
		fpstxt = "%3.2f" % fps
		self.fpstext.text = unicode(fpstxt)
		
		player = self.scene.player
		exactcoords = player.location.getExactLayerCoordinates()
		pos = "%1.2f" % exactcoords.x + ", %1.2f" % exactcoords.y
		self.positiontext.text = unicode(pos)
		
		vel = "%1.2f" % player.velocity.x + ", %1.2f" % player.velocity.y
		self.velocitytext.text = unicode(vel)
		
		
		self.pump_ctr += 1
