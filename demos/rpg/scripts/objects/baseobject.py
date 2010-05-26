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

class ObjectActionListener(fife.InstanceActionListener):
	def __init__(self, gamecontroller, obj):
		fife.InstanceActionListener.__init__(self)
		obj.instance.addActionListener(self)
		self._gamecontroller = gamecontroller
		self._object = obj

	def onInstanceActionFinished(self, instance, action):
		pass


class BaseGameObject(object):
	def __init__(self, gamecontroller, instancename, instanceid=None, createInstance=False):
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
		self._name = instancename
		if instanceid:
			self._id = instanceid
		else:
			self._id = self._name
			
		self._instance = None
		
		if createInstance:
			self._createFIFEInstance()
		else:
			self._instance = self._gamecontroller.scene.actorlayer.getInstance(self._id)
			self._instance.thisown = 0
			
	def destroy(self):
		"""
		Deletes the FIFE instance from the actor layer on the map.
		"""
		if self._instance :
			self._gamecontroller.scene.actorlayer.deleteInstance(self._instance)
			self._instance = None	
			
	def setMapPosition(self, x, y):
		curloc = self.location
	
		exactloc = self.location.getExactLayerCoordinates()
		exactloc.x = x
		exactloc.y = y
				
		curloc.setExactLayerCoordinates(exactloc)
		self.location = curloc

	def _createFIFEInstance(self):
		"""
		Should not be called directly.  Use the constructor!
		"""
		mapmodel = self._gamecontroller.engine.getModel()
		self._fifeobject = mapmodel.getObject(self._name, self._gamecontroller.settings.get("RPG", "ObjectNamespace", "http://www.fifengine.de/xml/rpg"))
		
		self._instance = self._gamecontroller.scene.actorlayer.createInstance(self._fifeobject, fife.ModelCoordinate(0,0), self._id)
		fife.InstanceVisual.create(self._instance)
		self._instance.thisown = 0

	def _getLocation(self):
		return self._instance.getLocation()
			
	def _setLocation(self, loc):
		self._instance.setLocation(loc)
				
	def _getInstance(self):
		return self._instance
				
	location = property(_getLocation, _setLocation)
	instance = property(_getInstance)
