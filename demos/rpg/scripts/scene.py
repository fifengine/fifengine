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

import sys, os, re, math, random, shutil, uuid

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
		self._questsettings = None

	def loadObject(self, objectname, objectid=None, valuedict=None):
		if objectid:
			identifier = objectid
		else:
			#identifier = uuid.uuid1()
			identifier = "blkdjfkdj"
	
		try:
			objdict = self._modelsettings.get("objects", objectname, {})
			modeldict = self._modelsettings.get("models", objdict["modelname"], {})
			
			print objdict
			print modeldict
			
			loadImportFile(modeldict["file"], self._gamecontroller.engine)
			
			if objdict["type"] == "GOLD":
				newobject = GoldStack(self._gamecontroller, self.itemlayer, objdict["type"], objectname, modeldict["model"], identifier)
			elif objdict["type"] == "PORTAL":
				newobject = Portal(self._gamecontroller, self.itemlayer, objdict["type"], objectname, modeldict["model"], identifier)
			elif objdict["type"] == "QUESTGIVER":
				newobject = QuestGiver(self._gamecontroller, self.actorlayer, objdict["type"], objectname, modeldict["model"], identifier, True)
				
				for quest in self._questsettings.get(identifier, "questlist", []):
					questdict = self._questsettings.get(identifier, quest, {})
					
					if questdict['type'] == "RETURN_ITEM":
						questobj = ReturnItemQuest(newobject, questdict['name'], questdict['desc'])
						for ritem in self._questsettings.get(quest+"_items", "itemlist", []):
							itemdict = self._questsettings.get(quest+"_items", ritem, {})
							if itemdict["name"] == "GOLD_COINS":
								questobj.addRequiredGold(int(itemdict['value']))
							else:
								questobj.addRequiredItem(ritem)
					else:
						questobj = Quest(actor, questdict['name'], questdict['desc'])
						
					newobject.addQuest(questobj)

			elif objdict["type"] == "NPC":
				newobject = Actor(self._gamecontroller, self.actorlayer, objdict["type"], objectname, modeldict["model"], identifier, True)
			else:
				return None
				
			if valuedict:
				newobject.deserialize(valuedict)
			else:
				newobject.deserialize(objdict)
		
		except KeyError, e:
			raise ObjectNotFoundError
			
		return newobject

	def loadObjects(self, mapfilename):
		for obj in self._objectsettings.get("objects", "objectlist", []):
			try:
				objdict = self._objectsettings.get("objects", obj, {})
				newobj = self.loadObject(objdict["objectname"], obj, objdict)
				self._gamecontroller.logger.log_debug("Loaded object: " + obj)
			except ObjectNotFoundError, e:
				self._gamecontroller.logger.log_error("Error while loading object: " + obj)
				continue
				
			try:
				if newobj:
					self.addObjectToScene(newobj)
			except ObjectAlreadyInSceneError, e:
				self._gamecontroller.logger.log_error("Object already part of scene:" + obj)
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
			pvals = player_settings.get("player", "player", {})
			self._player.deserialize(pvals)

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
		questfile = self._gamecontroller.settings.get("RPG", "QuestFile", "maps/quests.xml")
		
		self._objectsettings = Setting(settings_file=objectfile)
		self._modelsettings = Setting(settings_file=modelfile)
		self._questsettings = Setting(settings_file=questfile)

		for cam in self._map.getCameras():
			self._cameras[cam.getId()] = cam
		
		self._cameras[self._maincameraname].setZoom(self._gamecontroller.settings.get("RPG", "DefaultZoom", 1.0))

		self._actorlayer = self._map.getLayer(self._gamecontroller.settings.get("RPG", "ActorLayer", "actor_layer"))
		self._itemlayer = self._map.getLayer(self._gamecontroller.settings.get("RPG", "ItemLayer", "item_layer"))
		
		#self.loadItems(mapfilename)
		#self.loadActors(mapfilename)		
		self.loadObjects(mapfilename)
		
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
		self._questsettings = None
			
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
		"""
		@todo: throw ObjectNowFoundError
		"""
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
		map_settings = Setting(settings_file=filename, copy_dist=False)
		player_settings = Setting(settings_file=playerfilename, copy_dist=False)
		
		objectlist = []
		
		for obj in self._objectlist.values():
			ovals = obj.serialize()
			map_settings.set("objects", obj.id, ovals)
			objectlist.append(obj.id)
			
		map_settings.set("objects", "objectlist", objectlist)
		
		pvals = self._player.serialize()
		player_settings.set("player", "player", pvals)
		
		map_settings.saveSettings()
		player_settings.saveSettings()
		
	def deserialize(self):
		if self._mapname:
			createScene(self._mapname)
	
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
		
