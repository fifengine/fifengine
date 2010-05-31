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
from fife.extensions.loaders import loadImportFile
from fife.extensions.fife_settings import Setting

from scripts.actors.baseactor import Actor
from scripts.actors.questgiver import QuestGiver
from scripts.quests.basequest import Quest
from scripts.actors.player import Player
from scripts.objects.baseobject import GameObjectTypes
from scripts.objects.items import BaseItem, GoldStack
from scripts.misc.exceptions import ObjectNotFoundError

class Scene(object):
	def __init__(self, gamecontroller):
		self._gamecontroller = gamecontroller
		
		self._map = None
		self._mapname = None
		self._maincameraname = self._gamecontroller.settings.get("RPG", "DefaultCameraName", "camera1")
		self._cameras = {}
		
		self._actorlayer = None
		
		self._player = None
		self._objectlist = {}
		
		self._objectsettings = None
		self._modelsettings = None

	def loadItem(self, itemid):
		try:
			itemdict = self._objectsettings.get("items", itemid, {})
			modeldict = self._modelsettings.get("models", itemdict["typename"], {})	
		
			loadImportFile(modeldict["file"], self._gamecontroller.engine)
		
			if itemdict["typename"] == "GoldStack":
				newitem = GoldStack(self._gamecontroller, modeldict["model"], itemid)
				newitem.value = itemdict["value"]
			else:
				newitem = BaseItem(self._gamecontroller, modeldict["model"], itemid)
			
			newitem.setMapPosition(float(itemdict["posx"]), float(itemdict["posy"]))	
			
		except KeyError, e:
			raise FileFormatError
			
		return newitem
		
	def loadActor(self, actorid):
		try:
			objdict = self._objectsettings.get("npcs", actorid, {})
			modeldict = self._modelsettings.get("models", objdict["typename"], {})
			
			loadImportFile(modeldict["file"], self._gamecontroller.engine)
				
			if modeldict["type"] == "QUESTGIVER":
				actor = QuestGiver(self._gamecontroller, modeldict["model"], actorid, True)
				questcount = self._modelsettings.get(actorid, "questcount", 0)
				for x in range(1,questcount+1):
					quest = "quest" + str(x)
					questdict = self._modelsettings.get(actorid, quest, {})
					quest = Quest(actor, questdict['name'], questdict['desc'])
					
					for ritem in questdict['items'].split(" , "):
						if ritem == "GoldStack":
							quest.addRequiredGold(int(questdict['value']))
						else:
							quest.addRequiredItem(ritem)
						
					actor.addQuest(quest)
						
			elif modeldict["type"] == "NPC":
				actor = Actor(self._gamecontroller, modeldict["model"], npc, True)

			actor.setMapPosition(float(objdict["posx"]), float(objdict["posy"]))
		
		except KeyError, e:
			raise ObjectNotFoundError
		
		return actor

	def loadItems(self, mapfilename):
		for item in self._objectsettings.get("items", "itemlist", []):
			try:
				newitem = self.loadItem(item)
			except ObjectNotFoundError, e:
				print "Error while loading item:", item
				continue
				
			self.addObjectToScene(newitem)
						
	def loadActors(self, mapfilename):
		for npc in self._objectsettings.get("npcs", "npclist", []):
			try:
				actor = self.loadActor(npc)
			except ObjectNotFoundError, e:
				print "Error while loading actor:", actor
				continue
				
			self.addObjectToScene(actor)
			
	def loadPlayer(self):
		"""
		@todo: once we have all art assets this should be able to load one of 3 player models
		"""
		modelfile = self._gamecontroller.settings.get("RPG", "AllObjectFile", "maps/allobjects.xml")
		modelsettings = Setting(app_name="", settings_file=modelfile)
		modeldict = modelsettings.get("models", "Player", {})
	
		loadImportFile(modeldict["file"], self._gamecontroller.engine)
		self._player = Player(self._gamecontroller, "warrior")

	def createScene(self, mapfilename):
		if not self._map:
			self._map = loadMapFile(mapfilename, self._gamecontroller.engine)
		else:
			self.destroyScene()
			
		self._mapname = os.path.splitext(os.path.basename(mapfilename))[0]
		objectfile = "maps/" + self._mapname + "_objects.xml"
		modelfile = self._gamecontroller.settings.get("RPG", "AllObjectFile", "maps/allobjects.xml")
		
		self._objectsettings = Setting(app_name="",settings_file=objectfile)
		self._modelsettings = Setting(app_name="", settings_file=modelfile)		

		for cam in self._map.getCameras():
			self._cameras[cam.getId()] = cam
		
		self._cameras[self._maincameraname].setZoom(self._gamecontroller.settings.get("RPG", "DefaultZoom", 2.0))
		
		self._actorlayer = self._map.getLayer(self._gamecontroller.settings.get("RPG", "ActorLayer", "actor_layer"))
		self._itemlayer = self._map.getLayer(self._gamecontroller.settings.get("RPG", "ItemLayer", "item_layer"))
		
		self.loadItems(mapfilename)
		self.loadActors(mapfilename)		
		
		#finally load the player
		self.loadPlayer()
	
		
	def destroyScene(self):
		self._cameras.clear()
		
		self._player.destroy()
		
		for obj in self._objectlist.values():
			obj.destroy()

		if self._map:
			self._gamecontroller.engine.getModel().deleteMap(self._map)
				
		self._map = None
		self._mapname = None
		
		self._actorlayer = None
		self._itemlayer = None
		
		self._player = None
		self._objectlist.clear()
		
		self._objectsettings = None
		self._modelsettings = None
			
	def getInstancesAt(self, clickpoint, layer):
		"""
		Query the main camera for instances on the specified layer.
		"""
		return self.cameras[self._maincameraname].getMatchingInstances(clickpoint, layer)

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
	
	def addObjectToScene(self, obj):
		if not self._objectlist.has_key(obj.id):
			self._objectlist[obj.id] = obj
	
	def removeObjectFromScene(self, obj):
		obj.destroy()
		del self._objectlist[obj.id]
	
	def updateScene(self):
		pass
		
	def _getActorLayer(self):
		return self._actorlayer
		
	def _getItemLayer(self):
		return self._itemlayer
	
	def _getCameras(self):
		return self._cameras
	
	def _getPlayer(self):
		return self._player
	
	def _getObjectList(self):
		return self._objectlist
	
	def _getMap(self):
		return self._map
	
	actorlayer = property(_getActorLayer)
	itemlayer = property(_getItemLayer)
	cameras = property(_getCameras)
	player = property(_getPlayer)
	objectlist = property(_getObjectList)
	map = property(_getMap)
		
