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
from scripts.common.baseobject import *
from scripts.weapons import Weapon


class ShipActionListener(fife.InstanceActionListener):
	def __init__(self, ship):
		fife.InstanceActionListener.__init__(self)

		self._ship = ship
		self._ship.instance.addActionListener(self)

	def onInstanceActionFinished(self, instance, action):
		if action.getId() == 'flash':
			if self._ship._flashing and self._ship._flashnumber > 0:
				self._ship.instance.act('flash', self._ship.instance.getFacingLocation())
				self._ship._flashnumber -= 1	
			else:	
				self._ship._flashing = False
				self._ship._flashnumber = 0
		
		if action.getId() == 'explode' and not self._ship.type == SHTR_PLAYER:
			self._ship.removeFromScene()

class Ship(SpaceObject):
	def __init__(self, scene, name, findInstance=True):
		super(Ship, self).__init__(scene, name, findInstance)
		
		self._weapon = None

		self._flashnumber = 0
		self._flashing = False
		
		self._hitpoints = 0
		self._scorevalue = 0
	
	def _setWeapon(self, weapon):
		self._weapon = weapon
		
	def _getWeapon(self):
		return self._weapon
	
	def flash(self, number):
		if self._running:
			self._instance.act('flash', self._instance.getFacingLocation())
			self._flashnumber = number
			self._flashing = True	
	
	def fire(self, direction):
		if self._weapon and self._hitpoints > 0:
			return self._weapon.fire(direction)
		
		return None
		
	def applyHit(self, hp):
		self._hitpoints -= hp
		if self._hitpoints <= 0:
			self.destroy()
		
	def destroy(self):
		if self._running:
			self._instance.act('explode', self._instance.getFacingLocation())
			super(Ship, self).destroy()
	
	def _getHitPoints(self):
		return self._hitpoints
		
	def _setHitPoints(self, hp):
		self._hitpoints = hp
		
	def _getScoreValue(self):
		return self._scorevalue
	
	def _setScoreValue(self, value):
		self._scorevalue = value
	
	weapon = property(_getWeapon, _setWeapon)
	hitpoints = property(_getHitPoints, _setHitPoints)
	scorevalue = property(_getScoreValue, _setScoreValue)