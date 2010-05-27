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

from scripts.objects.baseobject import BaseGameObject, GameObjectTypes


class BaseItem(BaseGameObject):
	def __init__(self, gamecontroller, itemname, itemtype="unknown"):
		self._type = GameObjectTypes["ITEM"]
		super(BaseItem, self).__init__(gamecontroller, itemtype, itemname, True)
		
	def onPickUp(self):
		#remove item from the map
		self.destroy()
	
	def onDrop(self, dropx, dropy):
		#recreate object
		self._createFIFEInstance(self, self._gamecontroller.scene.itemlayer)
		self.setMapPosition(dropx, dropy)
		
	def _getItemType(self):
		return self._name
		
	def _getItemName(self):
		return self._id
	
	itemtype = property(_getItemType)
	itemname = property(_getItemName)
