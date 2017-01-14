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

import random
from fife import fife
from fife.extensions import pychan
from fife.extensions.pychan.tools import callbackWithArguments as cbwa
from fife.extensions.fife_timer import Timer

import scripts.test as test

class KeyListener(fife.IKeyListener):
	def __init__(self, test):
		self._engine = test._engine
		self._test = test
		self._eventmanager = self._engine.getEventManager()
		
		fife.IKeyListener.__init__(self)

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		if keystr == "t":
			r = self._test._camera.getRenderer('GridRenderer')
			r.setEnabled(not r.isEnabled())
		
	def keyReleased(self, evt):
		pass

class MouseListener(fife.IMouseListener):
	def __init__(self, test):
		self._engine = test._engine
		self._test = test
		self._eventmanager = self._engine.getEventManager()
		
		fife.IMouseListener.__init__(self)
		
	def mousePressed(self, event):
		if event.isConsumedByWidgets():
			return

		clickpoint = fife.ScreenPoint(event.getX(), event.getY())

		self._test.movePlayer(clickpoint)
				
	def mouseReleased(self, event):
		pass

	def mouseMoved(self, event):
		self._test.mouseMoved(event)
		
	def mouseEntered(self, event):
		pass
		
	def mouseExited(self, event):
		pass
		
	def mouseClicked(self, event):
		pass
	
	def mouseWheelMovedUp(self, event):
		self._test.setZoom(-0.1)	
		
	def mouseWheelMovedDown(self, event):
		self._test.setZoom(0.1)
		
	def mouseDragged(self, event):
		pass

class InstanceActionListener(fife.InstanceActionListener):
	def __init__(self, test):
		self._engine = test._engine
		self._test = test
		
		fife.InstanceActionListener.__init__(self)

	def onInstanceActionFinished(self, instance, action):
		instance.move('walk', self._test.createRandomTarget(), 4.0)

	def onInstanceActionCancelled(self, instance, action):
		pass
		
	def onInstanceActionFrame(self, instance, action, frame):
		pass
	
class MultiPathfinderTest(test.Test):

	def create(self, engine, application):
		self._application = application
		self._engine = engine
		self._running = False

		self._loader = fife.MapLoader(self._engine.getModel(), 
									self._engine.getVFS(), 
									self._engine.getImageManager(), 
									self._engine.getRenderBackend())

		self._eventmanager = self._engine.getEventManager()
		self._imagemanager = self._engine.getImageManager()



	def destroy(self):
		#any left over cleanup here
		pass
		
	def run(self):
		random.seed()
		self._running = True
		
		self._mouselistener = MouseListener(self)
		self._eventmanager.addMouseListener(self._mouselistener)
		
		self._keylistener = KeyListener(self)
		self._eventmanager.addKeyListener(self._keylistener)

		self._actionlistener = InstanceActionListener(self)
		
		self._font = pychan.internal.get_manager().createFont("data/fonts/rpgfont.png")
		if self._font is None:
			raise InitializationError("Could not load font %s" % name)

		self.loadMap("data/maps/multipathfinder_grassland.xml")

	def stop(self):
		self._running = False
		
		self._engine.getModel().deleteMap(self._map)
		self._engine.getModel().deleteObjects()
		
		self._eventmanager.removeMouseListener(self._mouselistener)
		self._eventmanager.removeKeyListener(self._keylistener)
		
		del self._mouselistener
		del self._keylistener
		
	def isRunning(self):
		return self._running

	def getName(self):
		return "MultiPathfinderTest"
		
	def getAuthor(self):
		return "helios"

	def getDescription(self):
		return "Use this as a template for more complicated tests."

	def getHelp(self):
		return open( 'data/help/MultiPathfinderTest.txt', 'r' ).read()
		
	def pump(self):
		"""
		This gets called every frame that the test is running.  We have nothing
		to do here for this test.
		"""
		pass

	def loadMap(self, filename):
		"""
		Simple function to load and display a map file. We could of course 
		have passed in the map filename but I'll leave that up to you.
		
		@param filename The filename.
		"""
	
		self._mapfilename = filename
		
		if self._loader.isLoadable(self._mapfilename):
			self._map = self._loader.load(self._mapfilename)
			self._mapLoaded = True

		self._camera = self._map.getCamera("camera1")
		self._actorlayer = self._map.getLayer("item_layer")
		self._groundlayer = self._map.getLayer("ground_layer")
		self._player = self._actorlayer.getInstance("player")
		self._frigate1 = self._actorlayer.getInstance("frigate1")
		self._frigate1.actOnce("stand", self._frigate1.getFacingLocation())
		self._frigate1.addActionListener(self._actionlistener)
		self._frigate2 = self._actorlayer.getInstance("frigate2")
		self._frigate2.actOnce("stand", self._frigate2.getFacingLocation())
		self._frigate2.addActionListener(self._actionlistener)
		
		self._camera.setLocation(self._player.getLocation())
		self._camera.attach(self._player)
		self._instance_renderer = fife.InstanceRenderer.getInstance(self._camera)
		cellrenderer = fife.CellRenderer.getInstance(self._camera)
		cellrenderer.addActiveLayer(self._actorlayer)
		cellrenderer.setEnabledBlocking(True)
		cellrenderer.setEnabledPathVisual(True)
		cellrenderer.addPathVisual(self._player)
		cellrenderer.addPathVisual(self._frigate1)
		cellrenderer.addPathVisual(self._frigate2)
		cellrenderer.setEnabled(True)

		coordinaterenderer = fife.CoordinateRenderer.getInstance(self._camera)
		coordinaterenderer.setFont(self._font)
		coordinaterenderer.addActiveLayer(self._actorlayer)
		#coordinaterenderer.setEnabled(True)
		
		gridrenderer = self._camera.getRenderer('GridRenderer')
		gridrenderer.activateAllLayers(self._map)

	def mouseMoved(self, event):
		self._instance_renderer.removeOutlined(self._player)

		pt = fife.ScreenPoint(event.getX(), event.getY())
		instances = self._camera.getMatchingInstances(pt, self._actorlayer)
		for i in instances:
			if i.getId() == "player":
				self._instance_renderer.addOutlined(i, 173, 255, 47, 2, 250)
				break

	def setZoom(self, zoom):
		self._camera.setZoom(self._camera.getZoom() + zoom)
		
	def getLocationAt(self, screenpoint):
		"""
		Query the main camera for the Map location (on the actor layer)
		that a screen point refers to.
		
		@param screenpoint A fife.ScreenPoint
		"""
		
		target_mapcoord = self._camera.toMapCoordinates(screenpoint, False)
		target_mapcoord.z = 0
		location = fife.Location(self._actorlayer)
		location.setMapCoordinates(target_mapcoord)
		return location

	def createRandomTarget(self):
		x = random.randint(-13, 22)
		y = random.randint(-28, 13)
		mc = fife.ModelCoordinate(x,y)

		location = fife.Location(self._actorlayer)
		location.setLayerCoordinates(mc)
		return location

	def movePlayer(self, screenpoint):
		"""
		Simple function that moves the player instance to the given screenpoint.
		
		@param screenpoint A fife.ScreenPoint
		"""
		
		self._player.move('walk', self.getLocationAt(screenpoint), 4.0)
