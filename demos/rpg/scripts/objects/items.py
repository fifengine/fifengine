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

import sys, os, re, math, random, shutil

from fife import fife

from scripts.objects.baseobject import BaseGameObject, GameObjectTypes


class BaseItem(BaseGameObject):
	def __init__(self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname):
		super(BaseItem, self).__init__(gamecontroller, layer, typename, baseobjectname, itemtype, itemname, True)
		
	def _getItemType(self):
		return self._name
		
	def _getItemName(self):
		return self._id
	
	itemtype = property(_getItemType)
	itemname = property(_getItemName)

class PickableItem(BaseItem):
	def __init__(self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname):
		super(PickableItem, self).__init__(gamecontroller, layer, typename, baseobjectname, itemtype, itemname)
		self._type = GameObjectTypes["ITEM"]

	def onPickUp(self):
		#remove item from the scene
		self._gamecontroller.scene.removeObjectFromScene(self)
	
	def onDrop(self, dropx, dropy):
		#recreate object
		self._createFIFEInstance(self, self._gamecontroller.scene.itemlayer)
		self.setMapPosition(dropx, dropy)
		
		self._gamecontroller.scene.addObjectToScene(self)		
	
class GoldStack(PickableItem):
	def __init__(self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname):
		super(GoldStack, self).__init__(gamecontroller, layer, typename, baseobjectname, itemtype, itemname)
		
		self._value = 0
		
	def serialize(self):
		lvars = super(GoldStack, self).serialize()
		lvars['value'] = self._value

		return lvars

	def deserialize(self, valuedict):
		super(GoldStack, self).deserialize(valuedict)
		
		if valuedict.has_key("value"):
			self._value = int(valuedict['value'])
		else:
			self._value = 0
		
	def _getValue(self):
		return self._value
		
	def _setValue(self, value):
		self._value = int(value)
		
	value = property(_getValue, _setValue)
	
class Portal(BaseItem):
	def __init__(self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname):
		super(Portal, self).__init__(gamecontroller, layer, typename, baseobjectname, itemtype, itemname)
		self._type = GameObjectTypes["PORTAL"]
		
		self._dest = None
	
	def serialize(self):
		lvars = super(Portal, self).serialize()
		lvars['dest'] = self._dest
		
		return lvars

	def deserialize(self, valuedict):
		super(Portal, self).deserialize(valuedict)

		if valuedict.has_key("dest"):
			self._dest = valuedict['dest']
		else:
			self._dest = "town"

	def _getDest(self):
		return self._dest
		
	def _setDest(self, dest):
		self._dest = dest
		
	dest = property(_getDest, _setDest)
