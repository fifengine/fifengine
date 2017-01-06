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
		if keystr == 't':
			r = self._test._camera.getRenderer('GridRenderer')
			r.setEnabled(not r.isEnabled())
		elif keystr == 'c':
			r = self._test._camera.getRenderer('CoordinateRenderer')
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
		pass
		
	def mouseEntered(self, event):
		pass
		
	def mouseExited(self, event):
		pass
		
	def mouseClicked(self, event):
		pass
	
	def mouseWheelMovedUp(self, event):
		self._test.setZoom(0.0625)

	def mouseWheelMovedDown(self, event):
		self._test.setZoom(-0.0625)
		
	def mouseDragged(self, event):
		pass

class InstanceActionListener(fife.InstanceActionListener):
	def __init__(self, test):
		self._engine = test._engine
		self._test = test
		
		fife.InstanceActionListener.__init__(self)

	def onInstanceActionFinished(self, instance, action):
		pass
		# lets the skels dance ^^
		#instance.actOnce("stand", instance.getRotation()+45)
		# skels walk to random locations
		#instance.move('walk', self._test.createRandomTarget(), 4.0)

	def onInstanceActionCancelled(self, instance, action):
		pass
		
	def onInstanceActionFrame(self, instance, action, frame):
		pass
	
class BenchmarkTest(test.Test):

	def create(self, engine, application):
		self._application = application
		self._engine = engine
		self._running = False

		self._loader = fife.MapLoader(self._engine.getModel(), 
									self._engine.getVFS(), 
									self._engine.getImageManager(), 
									self._engine.getRenderBackend())

		self._eventmanager = self._engine.getEventManager()

	def destroy(self):
		#any left over cleanup here
		pass
		
	def run(self):
		self._running = True
		
		self._mouselistener = MouseListener(self)
		self._eventmanager.addMouseListener(self._mouselistener)
		
		self._keylistener = KeyListener(self)
		self._eventmanager.addKeyListener(self._keylistener)

		self._font = pychan.internal.get_manager().createFont("data/fonts/rpgfont.png")
		if self._font is None:
			raise InitializationError("Could not load font %s" % name)

		self.loadMap("data/maps/benchmark.xml")

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
		return "BenchmarkTest"
		
	def getAuthor(self):
		return "helios"

	def getDescription(self):
		return "Simple benchmark test."

	def getHelp(self):
		return open( 'data/help/BenchmarkTest.txt', 'r' ).read()
		
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
		# sets ground static
		self._groundlayer.setStatic(True)
		self._player = self._actorlayer.getInstance("player")
		
		gridrenderer = self._camera.getRenderer('GridRenderer')
		gridrenderer.activateAllLayers(self._map)

		coordrenderer = fife.CoordinateRenderer.getInstance(self._camera)
		coordrenderer.setFont(self._font)
		coordrenderer.clearActiveLayers()
		coordrenderer.addActiveLayer(self._groundlayer)

		self.min = fife.ModelCoordinate()
		self.max = fife.ModelCoordinate()
		self._groundlayer.getMinMaxCoordinates(self.min, self.max)
		name = "skel"
		self.obj = self._player.getObject()
		# change pathfinder max ticks
		#pather = self.obj.getPather()
		#pather.setMaxTicks(1000)
		self._actionlistener = InstanceActionListener(self)
		for x in xrange(0, 1000):
			loc = self.createRandomTarget()
			i = self._actorlayer.createInstance(self.obj, loc.getLayerCoordinates(), name+str(x))
			fife.InstanceVisual.create(i)
			i.addActionListener(self._actionlistener)
			i.actOnce("stand", i.getFacingLocation())


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

	def movePlayer(self, screenpoint):
		"""
		Simple function that moves the player instance to the given screenpoint.
		
		@param screenpoint A fife.ScreenPoint
		"""
		
		self._player.move('walk', self.getLocationAt(screenpoint), 4.0)

	def setZoom(self, zoom):
		z = self._camera.getZoom() + zoom
		if z <= 0.0625:
			z = 0.0625
		self._camera.setZoom(z)

	def createRandomTarget(self):
		cache = self._actorlayer.getCellCache()
		while 1:
			x = random.randint(self.min.x, self.max.x)
			y = random.randint(self.min.y, self.max.y)
			mc = fife.ModelCoordinate(x,y)
			c = cache.getCell(mc)
			if c and c.getCellType() == fife.CTYPE_NO_BLOCKER:
				break

		location = fife.Location(self._actorlayer)
		location.setLayerCoordinates(mc)
		return location