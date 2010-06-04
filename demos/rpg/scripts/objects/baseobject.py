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

GameObjectTypes = 	{
						"DEFAULT": 0,
						"ITEM":1,
						"QUESTGIVER":2,
						"PLAYER":3,
						"NPC":4,
						"ENEMY":5,
						"GOLD":6,
						"PORTAL":7
					}
					
def getModuleByType(objtype):
	if objtype == GameObjectTypes["ITEM"] or objtype == GameObjectTypes["GOLD"] or objtype == GameObjectTypes["PORTAL"]:
		module = "items"
	elif objtype == GameObjectTypes["QUESTGIVER"] or objtype == GameObjectTypes["ENEMY"]:
		module = "npcs"	
	else:
		module = "unknown"
		
	return module

class ObjectActionListener(fife.InstanceActionListener):
	def __init__(self, gamecontroller, obj):
		fife.InstanceActionListener.__init__(self)
		self._gamecontroller = gamecontroller
		self._object = obj
		
	def detachActionListener(self):
		self._object.instance.removeActionListener(self)
		
	def attachActionListener(self):
		self._object.instance.addActionListener(self)

	def onInstanceActionFinished(self, instance, action):
		pass


class BaseGameObject(object):
	def __init__(self, gamecontroller, layer, typename, baseobjectname, instancename, instanceid=None, createInstance=False):
		"""
		@param gamecontroller: A reference to the master game controller
		@param instancename: The name of the object to load.  The object's XML file must
		be part of the map file or added with fife.extensions.loaders.loadImportFile
		@param instanceid: used if you want to give a specific ID to the instance to
		differenciate it from other instances
		@param createInstance: If this is True it will attempt to be loaded from disk and not
		use one that has already been loaded from the map file.  See the note about the 
		instancename paramater above.
		"""
		self._gamecontroller = gamecontroller
		self._fifeobject = None
		
		self._typename = typename
		self._baseobjectname = baseobjectname
		
		self._name = instancename
		if instanceid:
			self._id = instanceid
		else:
			self._id = self._name
			
		self._instance = None
		self._position = fife.DoublePoint(0.0, 0.0)
		
		self._actionlistener = None
		
		self._type = GameObjectTypes["DEFAULT"]

		self._layer = layer

		if createInstance:
			self._createFIFEInstance(self._layer)
		else:
			self._findFIFEInstance(self._layer)
			
		self._activated = True
		
	def destroy(self):
		"""
		Deletes the FIFE instance from the actor layer on the map.
		"""
		if self._actionlistener:
			self._actionlistener.detachActionListener()	
			self._actionlistener = None
		
		if self._instance :
			self._layer.deleteInstance(self._instance)
			self._instance = None
		
		self._activated = False
			
	def setMapPosition(self, x, y):
		curloc = self.location
	
		self._position = self.location.getExactLayerCoordinates()
		self._position.x = x
		self._position.y = y
				
		curloc.setExactLayerCoordinates(self._position)
		self.location = curloc
		
	def serialize(self):
		lvars = {}
		(x,y) = self.position
		lvars['posx'] = x
		lvars['posy'] = y
		lvars['type'] = self._typename
		lvars['objectname'] = self._baseobjectname
		
		return lvars

	def deserialize(self, valuedict):
		if valuedict.has_key("posx"):
			x = float(valuedict['posx'])
		else:
			x = 0
			
		if valuedict.has_key("posy"):
			y = float(valuedict['posy'])
		else:
			y = 0
		
		self.setMapPosition(x,y)

	def _createFIFEInstance(self, layer):
		"""
		Should not be called directly.  Use the constructor!
		"""
		mapmodel = self._gamecontroller.engine.getModel()
		self._fifeobject = mapmodel.getObject(self._name, self._gamecontroller.settings.get("RPG", "ObjectNamespace", "http://www.fifengine.de/xml/rpg"))
		
		self._instance = layer.createInstance(self._fifeobject, fife.ModelCoordinate(0,0), self._id)
		fife.InstanceVisual.create(self._instance)
			
		self._instance.thisown = 0
		
		self.setMapPosition(self._position.x,self._position.y)
		
	def _findFIFEInstance(self, layer):
		"""
		@todo: throw InstanceNotFoundError
		"""
		self._instance = self._layer.getInstance(self._id)
		if self._instance:
			self._instance.thisown = 0		
		
	def _getLocation(self):
		return self._instance.getLocation()
			
	def _setLocation(self, loc):
		self._instance.setLocation(loc)
				
	def _getInstance(self):
		return self._instance
	
	def _getType(self):
		return self._type
	
	def _getId(self):
		return self._id
		
	def _getModelName(self):
		return self._name
		
	def _getPosition(self):
		#if there isn't a FIFE instance just return last known coordinates
		if self._instance:
			self._position = self.location.getExactLayerCoordinates()
		
		return (self._position.x, self._position.y)
		
	def _setPosition(self, tuplexy):
		self.setMapPosition(tuplexy[0],tuplexy[1])
		
	def _getActivated(self):
		return self._activated
		
	def _setActivated(self, activate):
		self._activated = activate
			

	location = property(_getLocation, _setLocation)
	instance = property(_getInstance)
	type = property(_getType)
	id = property(_getId)
	modelname = property(_getModelName)
	position = property(_getPosition, _setPosition)
	activated = property(_getActivated, _setActivated)
