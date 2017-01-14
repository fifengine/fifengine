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

from fife import fife
from fife.extensions import pychan

import scripts.test as test


class KeyListener(fife.IKeyListener):
	def __init__(self, test):
		self._engine = test._engine
		self._test = test
		self._eventmanager = self._engine.getEventManager()

		fife.IKeyListener.__init__(self)

	def keyPressed(self, evt):
		keystr = evt.getKey().getAsString().lower()
		if keystr == "t":
			r = self._test._camera.getRenderer('GridRenderer')
			r.setEnabled(not r.isEnabled())
		elif keystr == "a":
			self._test.toggleArmor()
		elif keystr == "h":
			self._test.toggleHead()
		elif keystr == "0":
			self._test.disarm()
		elif keystr == "1":
			self._test.equipDagger()
		elif keystr == "2":
			self._test.equipShortsword()
		elif keystr == "3":
			self._test.equipLongsword()
		elif keystr == "4":
			self._test.equipGreatsword()
		elif keystr == "8":
			self._test.equipBuckler()
		elif keystr =="9":
			self._test.equipShield()

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
		self._test.setZoom(-0.0625)

	def mouseWheelMovedDown(self, event):
		self._test.setZoom(0.0625)

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
		self._imagemanager = self._engine.getImageManager()
		self._timemanager = self._engine.getTimeManager()

		self._time = self._timemanager.getTime()
		self._armor = "clothes"
		self._head = "head1"

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
		return open('data/help/OverlayTest.txt', 'r').read()

	def pump(self):
		time = self._timemanager.getTime()
		if (time - self._time) >= 1000:
			self._time = time
			self.rotateInstances()

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
		self._skel1 = self._actorlayer.getInstance("skel1")
		self._skel2 = self._actorlayer.getInstance("skel2")

		self._camera.setLocation(self._player.getLocation())
		self._camera.attach(self._player)
		self._instance_renderer = fife.InstanceRenderer.getInstance(self._camera)
		gridrenderer = self._camera.getRenderer('GridRenderer')
		gridrenderer.activateAllLayers(self._map)

		self.createStaticColorOverlays()
		self.createDefaultPlayer()
		self.createColoringAndOutlines()
		self.createColorOverlay()

	def createStaticColorOverlays(self):
		img = self._imagemanager.load("data/tilesets/toilett_singecolor_overlay.png")
		overlay = fife.OverlayColors(img)
		overlay.changeColor(fife.Color(255,255,255), fife.Color(0,0,255,128))
		for n in (1, 5, 9):
			instance = self._actorlayer.getInstance("toilett%s" % n)
			instance.addStaticColorOverlay(0, overlay)

		img = self._imagemanager.load("data/tilesets/toilett_multicolor_overlay.png")
		overlay = fife.OverlayColors(img)
		overlay.changeColor(fife.Color(255,0,0), fife.Color(255,0,0,80))
		overlay.changeColor(fife.Color(0,255,0), fife.Color(0,255,0,80))
		overlay.changeColor(fife.Color(0,0,255), fife.Color(0,0,255,80))
		for n in (2, 6, 10):
			instance = self._actorlayer.getInstance("toilett%s" % n)
			instance.addStaticColorOverlay(0, overlay)

		overlay.changeColor(fife.Color(255,0,0), fife.Color(255,0,0,200))
		overlay.changeColor(fife.Color(0,255,0), fife.Color(0,255,0,200))
		overlay.changeColor(fife.Color(0,0,255), fife.Color(0,0,255,200))
		for n in (3, 7, 11):
			instance = self._actorlayer.getInstance("toilett%s" % n)
			instance.addStaticColorOverlay(0, overlay)

	def createColorOverlay(self):
		# create color overlay for skels
		delay = 200
		for rotation in range(0, 360, 45):
			anim = fife.Animation.createAnimation()
			for frame in range(4):
				resource = ("skel_stand_overlay.png:overlay_stand_"
				            + str(rotation) + "_"
				            + str(frame) + ".png")
				anim.addFrame(self._imagemanager.get(resource), delay)

			overlay = fife.OverlayColors(anim)
			# skel 1
			overlay.changeColor(fife.Color(255,0,0), fife.Color(255,0,0,80))
			overlay.changeColor(fife.Color(0,0,255), fife.Color(0,0,255,80))
			self._skel1.addColorOverlay("stand", rotation, overlay)
			# skel 2
			overlay.changeColor(fife.Color(255,0,0), fife.Color(0,255,0,128))
			overlay.changeColor(fife.Color(0,0,255), fife.Color(0,255,0,128))
			self._skel2.addColorOverlay("stand", rotation, overlay)

	def addAnimationOverlay(self, action, name, count, order, delay):
		for rotation in range(0, 360, 45):
			anim = fife.Animation.createAnimation()
			for frame in range(count):
				resource = name + str(rotation) + "_0" + str(frame) + ".png"
				anim.addFrame(self._imagemanager.get(resource), delay)

			self._player.addAnimationOverlay(action, rotation, order, anim)

	def removeAnimationOverlay(self, action, order):
		for rotation in range(0, 360, 45):
			self._player.removeAnimationOverlay(action, rotation, order)

	def createDefaultPlayer(self):
		self.addAnimationOverlay("stand", "clothes.png:clothes_stance_", 4, 10, 200)
		self.addAnimationOverlay("stand", "male_head1.png:male_head1_stance_", 4, 20, 200)

		self.addAnimationOverlay("walk", "clothes.png:clothes_run_", 8, 10, 100)
		self.addAnimationOverlay("walk", "male_head1.png:male_head1_run_", 8, 20, 100)

	def toggleArmor(self):
		self.removeAnimationOverlay("stand", 10)
		self.removeAnimationOverlay("walk", 10)
		id = ""
		if self._armor == "clothes":
			self._armor = "leather"
			id = "leather_armor.png:leather_armor_"
		elif self._armor == "leather":
			self._armor = "steel"
			id = "steel_armor.png:steel_armor_"
		elif self._armor == "steel":
			self._armor = "clothes"
			id = "clothes.png:clothes_"
		else:
			print "invalid armor"

		self.addAnimationOverlay("stand", id+"stance_", 4, 10, 200)
		self.addAnimationOverlay("walk", id+"run_", 8, 10, 100)

	def toggleHead(self):
		self.removeAnimationOverlay("stand", 20)
		self.removeAnimationOverlay("walk", 20)
		id = ""
		if self._head == "head1":
			self._head = "head2"
			id = "male_head2.png:male_head2_"
		elif self._head == "head2":
			self._head = "head3"
			id = "male_head3.png:male_head3_"
		elif self._head == "head3":
			self._head = "head1"
			id = "male_head1.png:male_head1_"
		else:
			print "invalid head"

		self.addAnimationOverlay("stand", id+"stance_", 4, 20, 200)
		self.addAnimationOverlay("walk", id+"run_", 8, 20, 100)

	def disarm(self):
		self.removeAnimationOverlay("stand", 40)
		self.removeAnimationOverlay("walk", 40)

		self.removeAnimationOverlay("stand", 30)
		self.removeAnimationOverlay("walk", 30)

	def equipDagger(self):
		self.removeAnimationOverlay("stand", 40)
		self.removeAnimationOverlay("walk", 40)

		self.addAnimationOverlay("stand", "dagger.png:dagger_stance_", 4, 40, 200)
		self.addAnimationOverlay("walk", "dagger.png:dagger_run_", 8, 40, 100)

	def equipShortsword(self):
		self.removeAnimationOverlay("stand", 40)
		self.removeAnimationOverlay("walk", 40)

		self.addAnimationOverlay("stand", "shortsword.png:shortsword_stance_", 4, 40, 200)
		self.addAnimationOverlay("walk", "shortsword.png:shortsword_run_", 8, 40, 100)

	def equipLongsword(self):
		self.removeAnimationOverlay("stand", 40)
		self.removeAnimationOverlay("walk", 40)

		self.addAnimationOverlay("stand", "longsword.png:longsword_stance_", 4, 40, 200)
		self.addAnimationOverlay("walk", "longsword.png:longsword_run_", 8, 40, 100)

	def equipGreatsword(self):
		self.removeAnimationOverlay("stand", 40)
		self.removeAnimationOverlay("walk", 40)

		self.addAnimationOverlay("stand", "greatsword.png:greatsword_stance_", 4, 40, 200)
		self.addAnimationOverlay("walk", "greatsword.png:greatsword_run_", 8, 40, 100)

	def equipBuckler(self):
		self.removeAnimationOverlay("stand", 30)
		self.removeAnimationOverlay("walk", 30)

		self.addAnimationOverlay("stand", "buckler.png:buckler_stance_", 4, 30, 200)
		self.addAnimationOverlay("walk", "buckler.png:buckler_run_", 8, 30, 100)

	def equipShield(self):
		self.removeAnimationOverlay("stand", 30)
		self.removeAnimationOverlay("walk", 30)

		self.addAnimationOverlay("stand", "shield.png:shield_stance_", 4, 30, 200)
		self.addAnimationOverlay("walk", "shield.png:shield_run_", 8, 30, 100)

	def createColoringAndOutlines(self):
		instances = [self._actorlayer.getInstance("toilett%s" % n) for n in range(4, 12)]
		for count, i in enumerate(instances, start=1):
			if count < 5:
				self._instance_renderer.addOutlined(i, 173, 255, 47, 2)
			elif count > 4:
				self._instance_renderer.addColored(i, 255, 0, 0, 60)

	def rotateInstances(self):
		self._skel1.setRotation(self._skel1.getRotation()+45)
		self._skel2.setRotation(self._skel2.getRotation()+45)

	def mouseMoved(self, event):
		self._instance_renderer.removeOutlined(self._player)

		pt = fife.ScreenPoint(event.getX(), event.getY())
		instances = self._camera.getMatchingInstances(pt, self._actorlayer)
		for i in instances:
			if i.getId() == "player":
				self._instance_renderer.addOutlined(i, 173, 255, 47, 2)
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

	def movePlayer(self, screenpoint):
		"""
		Simple function that moves the player instance to the given screenpoint.

		@param screenpoint A fife.ScreenPoint
		"""

		self._player.move('walk', self.getLocationAt(screenpoint), 4.0)
