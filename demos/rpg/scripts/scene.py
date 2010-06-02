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
from scripts.quests.basequest import Quest, ReturnItemQuest, QuestTypes
from scripts.actors.player import Player
from scripts.objects.baseobject import GameObjectTypes, getModuleByType
from scripts.objects.items import BaseItem, GoldStack, Portal
from scripts.misc.exceptions import ObjectNotFoundError, ObjectAlreadyInSceneError

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
			print itemdict
			print modeldict
			loadImportFile(modeldict["file"], self._gamecontroller.engine)
		
			if modeldict["type"] == "GOLD":
				newitem = GoldStack(self._gamecontroller, self.itemlayer, itemdict["typename"], modeldict["model"], itemid)
				newitem.value = itemdict["value"]
			elif modeldict["type"] == "PORTAL":
				newitem = Portal(self._gamecontroller, self.itemlayer, itemdict["typename"], modeldict["model"], itemid)
				newitem.dest = itemdict["dest"]
			else:
				newitem = BaseItem(self._gamecontroller, self.itemlayer, itemdict["typename"], modeldict["model"], itemid)
			
			newitem.deserialize(self._objectsettings)
			
		except KeyError, e:
			raise ObjectNotFoundError
			
		return newitem
		
	def loadActor(self, actorid):
		try:
			objdict = self._objectsettings.get("npcs", actorid, {})
			modeldict = self._modelsettings.get("models", objdict["typename"], {})
			
			loadImportFile(modeldict["file"], self._gamecontroller.engine)
				
			if modeldict["type"] == "QUESTGIVER":
				actor = QuestGiver(self._gamecontroller, self.actorlayer, objdict["typename"], modeldict["model"], actorid, True)
				questcount = self._modelsettings.get(actorid, "questcount", 0)
				for x in range(1,questcount+1):
					quest = "quest" + str(x)
					questdict = self._modelsettings.get(actorid, quest, {})
					
					if questdict['type'] == "RETURN_ITEM":
						quest = ReturnItemQuest(actor, questdict['name'], questdict['desc'])
						for ritem in questdict['items'].split(" , "):
							if ritem == "GoldStack":
								quest.addRequiredGold(int(questdict['value']))
							else:
								quest.addRequiredItem(ritem)
					else:
						quest = Quest(actor, questdict['name'], questdict['desc'])
						
					actor.addQuest(quest)
						
			elif modeldict["type"] == "NPC":
				actor = Actor(self._gamecontroller, self.actorlayer, objdict["typename"], modeldict["model"], npc, True)

			actor.setMapPosition(float(objdict["posx"]), float(objdict["posy"]))
		
		except KeyError, e:
			raise ObjectNotFoundError
		
		return actor

	def loadItems(self, mapfilename):
		for item in self._objectsettings.get("items", "itemlist", []):
			try:
				newitem = self.loadItem(item)
				self._gamecontroller.logger.log_debug("Loaded item: " + item)
			except ObjectNotFoundError, e:
				self._gamecontroller.logger.log_error("Error while loading item: " + item)
				continue
			
			try:
				self.addObjectToScene(newitem)
			except ObjectAlreadyInSceneError, e:
				self._gamecontroller.logger.log_error("Item already part of scene: " + newitem)
				continue
						
	def loadActors(self, mapfilename):
		for npc in self._objectsettings.get("npcs", "npclist", []):
			try:
				actor = self.loadActor(npc)
				self._gamecontroller.logger.log_debug("Loaded actor: " + npc)
			except ObjectNotFoundError, e:
				self._gamecontroller.logger.log_error("Error while loading actor:" + npc)
				continue
			
			try:
				self.addObjectToScene(actor)
			except ObjectAlreadyInSceneError, e:
				self._gamecontroller.logger.log_error("Actor already part of scene:" + actor)
				continue
			
	def createPlayerObject(self):
		"""
		@todo: once we have all art assets this should be able to load one of 3 player models
		"""
		modeldict = self._modelsettings.get("models", "Player", {})
	
		loadImportFile(modeldict["file"], self._gamecontroller.engine)
		self._player = Player(self._gamecontroller, self.actorlayer, "warrior")
		
		playerfilename = os.path.join("saves", "player_save.xml")
		
		if os.path.isfile(playerfilename):
			player_settings = Setting(settings_file=playerfilename, copy_dist=False)
			self._player.deserialize(player_settings)

	def createScene(self, mapname):
		mapfilename = os.path.join("maps", mapname + ".xml")
		
		if self._map:
			self.destroyScene()
			
		self._map = loadMapFile(mapfilename, self._gamecontroller.engine)
			
		self._mapname = mapname

		if os.path.isfile(os.path.join("saves", mapname + "_save.xml")):
			objectfile = os.path.join("saves", mapname + "_save.xml")
		else:
			objectfile = os.path.join("maps", mapname + "_objects.xml")

		modelfile = self._gamecontroller.settings.get("RPG", "AllObjectFile", "maps/allobjects.xml")
		
		print objectfile
		self._objectsettings = Setting(app_name="",settings_file=objectfile)
		self._modelsettings = Setting(app_name="", settings_file=modelfile)		

		for cam in self._map.getCameras():
			self._cameras[cam.getId()] = cam
		
		self._cameras[self._maincameraname].setZoom(self._gamecontroller.settings.get("RPG", "DefaultZoom", 1.0))

		self._actorlayer = self._map.getLayer(self._gamecontroller.settings.get("RPG", "ActorLayer", "actor_layer"))
		self._itemlayer = self._map.getLayer(self._gamecontroller.settings.get("RPG", "ItemLayer", "item_layer"))
		
		self.loadItems(mapfilename)
		self.loadActors(mapfilename)		
		
		#finally load the player
		self.createPlayerObject()
		
	def destroyScene(self):
		for obj in self._objectlist.values():
			obj.destroy()

		self._objectlist.clear()
		self._objectlist = {}
		self._cameras.clear()
		self._cameras = {}
		
		self._player.destroy()
		self._player = None

		for cam in self._map.getCameras():
			cam.resetRenderers()

		if self._map:
			self._gamecontroller.engine.getModel().deleteMap(self._map)
			
		retval = self._gamecontroller.engine.getModel().deleteObjects()
				
		self._map = None
		self._mapname = None
		
		self._actorlayer = None
		self._itemlayer = None
		
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
		else:
			obj.destroy()
			raise ObjectAlreadyInSceneError
			
	def getObject(self, objid):
		if self._objectlist.has_key(objid):
			return self._objectlist[objid]
		else:
			return None
	
	def removeObjectFromScene(self, obj):
		obj.destroy()
		del self._objectlist[obj.id]
		
	def serialize(self):
		filename = os.path.join("saves", self._mapname + "_save.xml")
		playerfilename = os.path.join("saves", "player_save.xml")
		settings = Setting(settings_file=filename, copy_dist=False)
		player_settings = Setting(settings_file=playerfilename, copy_dist=False)
		
		itemlist = []
		npclist = []
		
		for obj in self._objectlist.values():
			obj.serialize(settings)
			module = getModuleByType(obj.type)
			if module == "items":
				itemlist.append(obj.id)
			elif module == "npcs":
				npclist.append(obj.id)
			
		settings.set("items", "itemlist", itemlist)
		settings.set("npcs", "npclist", npclist)
		
		self._player.serialize(player_settings)
		
		settings.saveSettings()
		player_settings.saveSettings()
		
	def deserialize(self):
		pass
	
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
		
	def _getMapName(self):
		return self._mapname
	
	actorlayer = property(_getActorLayer)
	itemlayer = property(_getItemLayer)
	cameras = property(_getCameras)
	player = property(_getPlayer)
	objectlist = property(_getObjectList)
	map = property(_getMap)
	mapname = property(_getMapName)
		
