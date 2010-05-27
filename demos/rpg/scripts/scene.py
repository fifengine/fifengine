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
from fife.extensions.loaders import loadMapFile
from fife.extensions.fife_settings import Setting

from scripts.actors.baseactor import Actor
from scripts.actors.baseactor import QuestGiver
from scripts.actors.player import Player
from scripts.objects.baseobject import GameObjectTypes

class Scene(object):
	def __init__(self, gamecontroller):
		self._gamecontroller = gamecontroller
		
		self._map = None
		self._maincameraname = self._gamecontroller.settings.get("RPG", "DefaultCameraName", "camera1")
		self._cameras = {}
		
		self._actorlayer = None
		
		self._player = None
		self._objectlist = {}
		
	def createScene(self, mapfilename):
		if not self._map:
			self._map = loadMapFile(mapfilename, self._gamecontroller.engine)
		else:
			self.destroyScene()

		for cam in self._map.getCameras():
			self._cameras[cam.getId()] = cam
		
		self._cameras[self._maincameraname].setZoom(self._gamecontroller.settings.get("RPG", "DefaultZoom", 2.0))
		
		self._actorlayer = self._map.getLayer(self._gamecontroller.settings.get("RPG", "ActorLayer", "actor_layer"))
		
		self._player = Player(self._gamecontroller, "warrior")
		
		mapname = os.path.splitext(os.path.basename(mapfilename))
		objectfile = "maps/" + mapname[0] + "_objects.xml"
		objectsettings = Setting(app_name="",settings_file=objectfile)
		
		for npc in objectsettings.get(mapname[0], "npclist", []):
			(objtype, modelname, posx, posy) = objectsettings.get(mapname[0], npc, ["NPC", "warrior", "0", "0"])
			if objtype == "QUESTGIVER":
				actor = QuestGiver(self._gamecontroller, modelname, npc, True)
			elif objtype == "NPC":
				actor = Actor(self._gamecontroller, modelname, npc, True)

			actor.setMapPosition(float(posx), float(posy))
			self._objectlist[actor.instance.getId()] = actor
			
		
	def destroyScene(self):
		self._cameras.clear()
		
		self._player.destroy()
		
		for obj in self._objectlist.values():
			obj.destroy()

		if self._map:
			self._gamecontroller.engine.getModel().deleteMap(self._map)
		
		self._map = None
		self._actorlayer = None
		
		self._player = None
		self._objectlist.clear()
		
	def getInstancesAt(self, clickpoint):
		"""
		Query the main camera for instances on the actor layer.
		"""
		return self.cameras[self._maincameraname].getMatchingInstances(clickpoint, self._actorlayer)

	def getLocationAt(self, clickpoint):
		"""
		Query the main camera for the Map location (on the actor layer)
		that a screen point refers to.
		"""
		target_mapcoord = self._cameras[self._maincameraname].toMapCoordinates(clickpoint, False)
		target_mapcoord.z = 0
		location = fife.Location(self._actorlayer)
		location.setMapCoordinates(target_mapcoord)
		return location
				
	def updateScene(self):
		pass
		
	def _getActorLayer(self):
		return self._actorlayer
	
	def _getCameras(self):
		return self._cameras
	
	def _getPlayer(self):
		return self._player
	
	def _getObjectList(self):
		return self._objectlist
	
	actorlayer = property(_getActorLayer)
	cameras = property(_getCameras)
	player = property(_getPlayer)
	objectlist = property(_getObjectList)
		
