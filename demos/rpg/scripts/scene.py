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

import sys, os, re, math, random, shutil, uuid

from fife import fife
from fife.extensions.loaders import loadMapFile
from fife.extensions.serializers.xml_loader_tools import loadImportFile
from fife.extensions.serializers.simplexml import SimpleXMLSerializer
from fife.extensions.serializers.xmlobject import XMLObjectLoader

from scripts.actors.baseactor import Actor
from scripts.actors.questgiver import QuestGiver
from scripts.quests.basequest import Quest, ReturnItemQuest, QuestTypes
from scripts.actors.player import Player
from scripts.objects.baseobject import GameObjectTypes
from scripts.objects.items import BaseItem, GoldStack, Portal
from scripts.misc.exceptions import ObjectNotFoundError, ObjectAlreadyInSceneError
from scripts.misc.serializer import Serializer


class Scene(Serializer):
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

		self.obj_loader = XMLObjectLoader(
			gamecontroller.engine.getImagePool(),
			gamecontroller.engine.getAnimationPool(),
			gamecontroller.engine.getModel(),
			gamecontroller.engine.getVFS()
		)

	def loadObject(self, objectname, objectid=None, valuedict=None):
		if objectid:
			identifier = objectid
		else:
			identifier = str(uuid.uuid1())
			
		try:
			objdict = self._modelsettings.get("objects", objectname, {})
			modeldict = self._modelsettings.get("models", objdict["modelname"], {})
			
			loadImportFile(self.obj_loader, modeldict["file"], self._gamecontroller.engine)
			
			if objdict["type"] == "GOLD":
				newobject = GoldStack(self._gamecontroller, self.itemlayer, objdict["type"], objectname, modeldict["model"], identifier)
			elif objdict["type"] == "PORTAL":
				newobject = Portal(self._gamecontroller, self.itemlayer, objdict["type"], objectname, modeldict["model"], identifier)
			elif objdict["type"] == "QUESTGIVER":
				newobject = QuestGiver(self._gamecontroller, self.actorlayer, objdict["type"], objectname, modeldict["model"], identifier, True)
			elif objdict["type"] == "ENEMY":
				newobject = Actor(self._gamecontroller, self.actorlayer, objdict["type"], objectname, modeldict["model"], identifier, True)
			else:
				return None
			
			#if the valuedict is supplied it overrides the loaded default values
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
	
		loadImportFile(self.obj_loader, modeldict["file"], self._gamecontroller.engine)
		self._player = Player(self._gamecontroller, self.actorlayer, "warrior")
		
		playerfilename = os.path.join("saves", "player_save.xml")
		
		if os.path.isfile(playerfilename):
			player_settings = SimpleXMLSerializer(playerfilename)
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
		
		self._objectsettings = SimpleXMLSerializer(objectfile)
		self._modelsettings = SimpleXMLSerializer(modelfile)

		for cam in self._map.getCameras():
			self._cameras[cam.getId()] = cam
		
		self._cameras[self._maincameraname].setZoom(self._gamecontroller.settings.get("RPG", "DefaultZoom", 1.0))

		self._actorlayer = self._map.getLayer(self._gamecontroller.settings.get("RPG", "ActorLayer", "actor_layer"))
		self._itemlayer = self._map.getLayer(self._gamecontroller.settings.get("RPG", "ItemLayer", "item_layer"))
		
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
	
	def getObject(self, objid):
		"""
		Throws ObjectNotFoundError when an object cannot be found on the scene
		"""
		
		try:
			return self._objectlist[objid]
		except KeyError:
			raise ObjectNotFoundError(objid + " was not found on the scene.")
	
	def addObjectToScene(self, obj):
		if not self._objectlist.has_key(obj.id):
			self._objectlist[obj.id] = obj
		else:
			obj.destroy()
			raise ObjectAlreadyInSceneError

	def removeObjectFromScene(self, obj):
		"""
		Throws ObjectNotFoundError when an object cannot be found on the scene
		"""
		
		obj.destroy()
		
		try:
			del self._objectlist[obj.id]
		except KeyError:
			raise ObjectNotFoundError(obj.id + " could not be removed from the scene as it was not found in the scene.")
		
	def serialize(self):
		filename = os.path.join("saves", self._mapname + "_save.xml")
		playerfilename = os.path.join("saves", "player_save.xml")
		map_settings = SimpleXMLSerializer(filename)
		player_settings = SimpleXMLSerializer(playerfilename)
		
		objectlist = []
		
		for obj in self._objectlist.values():
			ovals = obj.serialize()
			map_settings.set("objects", obj.id, ovals)
			objectlist.append(obj.id)
			
		map_settings.set("objects", "objectlist", objectlist)
		
		pvals = self._player.serialize()
		player_settings.set("player", "player", pvals)
		
		map_settings.save()
		player_settings.save()
		
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
		
