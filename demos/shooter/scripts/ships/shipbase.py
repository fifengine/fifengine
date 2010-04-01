# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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

from math import sqrt

from fife import fife
from scripts.common.baseobject import SpaceObject
from scripts.weapons import Weapon


class Ship(SpaceObject):
	def __init__(self, model, name, layer, findInstance=True):
		super(Ship, self).__init__(model, name, layer, findInstance)

		self._maxvelocity = 0.025/sqrt(self._xscale * self._yscale)
		self._timedelta = 0
		self._weapon = None
	
	def _setWeapon(self, weapon):
		self._weapon = weapon
		
	def _getWeapon(self, weapon):
		return self._weapon
		
	def fire(self, curtime):
		if self._weapon:
			return self._weapon.fire(curtime)
		
		return None
	
	weapon = property(_getWeapon, _setWeapon)
	