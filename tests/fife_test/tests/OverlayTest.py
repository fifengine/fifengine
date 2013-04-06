#!/usr/bin/env python

# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2013 by the FIFE team
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
		pass	
		
	def mouseWheelMovedDown(self, event):
		pass
		
	def mouseDragged(self, event):
		pass

class OverlayTest(test.Test):

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

		self.loadMap("data/maps/overlay_grassland.xml")

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
		return "OverlayTest"
		
	def getAuthor(self):
		return "helios"

	def getDescription(self):
		return "Use this as a template for more complicated tests."

	def getHelp(self):
		return open( 'data/help/OverlayTest.txt', 'r' ).read()
		
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
		self._camera.setLocation(self._player.getLocation())
		self._camera.attach(self._player)
		gridrenderer = self._camera.getRenderer('GridRenderer')
		gridrenderer.activateAllLayers(self._map)

		manager = self._engine.getImageManager()
		img = manager.load("data/tilesets/toilett_singecolor_overlay.png")
		overlay = fife.OverlayColors(img)
		overlay.changeColor(fife.Color(255,255,255), fife.Color(0,0,255,128))
		self._actorlayer.getInstance("toilett1").addStaticColorOverlay(0, overlay)
		self._actorlayer.getInstance("toilett5").addStaticColorOverlay(0, overlay)
		self._actorlayer.getInstance("toilett9").addStaticColorOverlay(0, overlay)

		img = manager.load("data/tilesets/toilett_multicolor_overlay.png")
		overlay = fife.OverlayColors(img)
		overlay.changeColor(fife.Color(255,0,0), fife.Color(255,0,0,80))
		overlay.changeColor(fife.Color(0,255,0), fife.Color(0,255,0,80))
		overlay.changeColor(fife.Color(0,0,255), fife.Color(0,0,255,80))
		self._actorlayer.getInstance("toilett2").addStaticColorOverlay(0, overlay)
		self._actorlayer.getInstance("toilett6").addStaticColorOverlay(0, overlay)
		self._actorlayer.getInstance("toilett10").addStaticColorOverlay(0, overlay)
		
		overlay.changeColor(fife.Color(255,0,0), fife.Color(255,0,0,200))
		overlay.changeColor(fife.Color(0,255,0), fife.Color(0,255,0,200))
		overlay.changeColor(fife.Color(0,0,255), fife.Color(0,0,255,200))
		self._actorlayer.getInstance("toilett3").addStaticColorOverlay(0, overlay)
		self._actorlayer.getInstance("toilett7").addStaticColorOverlay(0, overlay)
		self._actorlayer.getInstance("toilett11").addStaticColorOverlay(0, overlay)

		obj = self._player.getObject()
		stand_action = obj.getAction("stand")
		action_visual = stand_action.get2dGfxVisual()
		angles = action_visual.getActionImageAngles()
		for angle in angles:
			#print "angle: ", angle
			animation = action_visual.getAnimationByAngle(angle)
			action_visual.addAnimationOverlay(angle, 0, animation)
		delay = 200
		anim = fife.Animation.createAnimation()
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_180_0.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_180_1.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_180_2.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_180_3.png"), delay)
		action_visual.addAnimationOverlay(180, 1, anim)
		#overlay = fife.OverlayColors(anim)
		#overlay.changeColor(fife.Color(255,0,0), fife.Color(255,0,0,80))
		#overlay.changeColor(fife.Color(0,0,255), fife.Color(0,0,255,80))
		#action_visual.addColorOverlay(180, overlay)

		anim = fife.Animation.createAnimation()
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_0_0.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_0_1.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_0_2.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_0_3.png"), delay)
		action_visual.addAnimationOverlay(0, 1, anim)
		#overlay.setColorOverlayAnimation(anim)
		#action_visual.addColorOverlay(0, overlay)

		anim = fife.Animation.createAnimation()
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_45_0.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_45_1.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_45_2.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_45_3.png"), delay)
		action_visual.addAnimationOverlay(45, 1, anim)
		#overlay.setColorOverlayAnimation(anim)
		#action_visual.addColorOverlay(45, overlay)

		anim = fife.Animation.createAnimation()
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_90_0.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_90_1.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_90_2.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_90_3.png"), delay)
		action_visual.addAnimationOverlay(90, 1, anim)
		#overlay.setColorOverlayAnimation(anim)
		#action_visual.addColorOverlay(90, overlay)

		anim = fife.Animation.createAnimation()
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_135_0.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_135_1.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_135_2.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_135_3.png"), delay)
		action_visual.addAnimationOverlay(135, 1, anim)
		#overlay.setColorOverlayAnimation(anim)
		#action_visual.addColorOverlay(135, overlay)

		anim = fife.Animation.createAnimation()
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_225_0.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_225_1.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_225_2.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_225_3.png"), delay)
		action_visual.addAnimationOverlay(225, 1, anim)
		#overlay.setColorOverlayAnimation(anim)
		#action_visual.addColorOverlay(225, overlay)

		anim = fife.Animation.createAnimation()
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_270_0.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_270_1.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_270_2.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_270_3.png"), delay)
		action_visual.addAnimationOverlay(270, 1, anim)
		#overlay.setColorOverlayAnimation(anim)
		#action_visual.addColorOverlay(270, overlay)

		anim = fife.Animation.createAnimation()
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_315_0.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_315_1.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_315_2.png"), delay)
		anim.addFrame(manager.get("skel_stand_overlay.png:overlay_stand_315_3.png"), delay)
		action_visual.addAnimationOverlay(315, 1, anim)
		#overlay.setColorOverlayAnimation(anim)
		#action_visual.addColorOverlay(315, overlay)

		self.createColoringAndOutlines()
		

	def createColoringAndOutlines(self):
		renderer = fife.InstanceRenderer.getInstance(self._camera)
		instances = list()
		instances.append(self._actorlayer.getInstance("toilett4"))
		instances.append(self._actorlayer.getInstance("toilett5"))
		instances.append(self._actorlayer.getInstance("toilett6"))
		instances.append(self._actorlayer.getInstance("toilett7"))
		instances.append(self._actorlayer.getInstance("toilett8"))
		instances.append(self._actorlayer.getInstance("toilett9"))
		instances.append(self._actorlayer.getInstance("toilett10"))
		instances.append(self._actorlayer.getInstance("toilett11"))
		count = 0
		for i in instances:
			count += 1
			if count < 5:
				renderer.addOutlined(i, 173, 255, 47, 2)
			elif count > 4:
				renderer.addColored(i, 255, 0, 0, 60)
		
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
