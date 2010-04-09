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
	def __init__(self, scene, name, findInstance=True):
		super(Ship, self).__init__(scene, name, findInstance)
		
		self._weapon = None
		self._flashrate = 0
		self._flashnumber = 0
		self._flashing = False
		self._flashtime = 0
		
		#1 = on, 0 = invisible (off)
		self._flashstate = 1
	
	def _setWeapon(self, weapon):
		self._weapon = weapon
		
	def _getWeapon(self):
		return self._weapon
		
	def fire(self, direction):
		if self._weapon:
			return self._weapon.fire(direction)
		
		return None
		
	def flash(self, rate, number):
		"""
		Flash rate is measured in flashes per second.  A single flash
		would be an off and on cycle.
		"""
		self._flashing = True
		self._flashrate = rate * 2
		self._flashnumber = number * 2
	
	def destroy(self):
		self._scene.removeObjectFromScene(self)
		super(Ship, self).destroy()
	
	def update(self):
		if self._flashing:
			if self._flashnumber <= 0:
				self._flashing = False
				self._instance.get2dGfxVisual().setVisible(True)
			else:
				self._flashtime += self._scene.timedelta
				if self._flashtime >= 1000/self._flashrate:
					if self._flashstate == 1:
						self._flashstate = 0
						self._instance.get2dGfxVisual().setVisible(False)
					else:
						self._flashstate = 1
						self._instance.get2dGfxVisual().setVisible(True)
						
					self._flashtime = 0
					self._flashnumber -= 1
	
		super(Ship, self).update()
	
	weapon = property(_getWeapon, _setWeapon)
	