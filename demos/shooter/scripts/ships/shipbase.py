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

from fife import fife
from scripts.common.helpers import normalize
from scripts.weapons import Weapon

class Ship(object):
	def __init__(self, scene, shipName, layer, uniqInMap=True):
		self._scene = scene
		self._name = shipName
		self._layer = layer
		if uniqInMap:
			self._instance = self._layer.getInstance(self._name)
		else:
			#have to create instance here
			self._instance = None
		
		#velocity as a vector
		self._velocity = fife.DoublePoint(0,0)
		self._maxvelocity = 0.025
		self._timedelta = 0
		self._weapon = None

	def _getMaxVelocity(self):
		return self._maxvelocity
		
	def _setMaxVelocity(self, maxvel):
		self._maxvelocity = maxvel

	def _getLocation(self):
		return self._instance.getLocation()
		
	def _setLocation(self, loc):
		self._instance.setLocation(loc)

	def _getInstance(self):
		return self._instance
	
	def _getVelocity(self):
		return self._velocity
	
	def _setWeapon(self, weapon):
		self._weapon = weapon
		
	def _getWeapon(self, weapon):
		return self._weapon
		
	def applyThrust(self, vector):
		self._velocity.x += vector.x * (self._timedelta/1000.0)
		self._velocity.y += vector.y * (self._timedelta/1000.0)
		
		if self._velocity.length() > self._maxvelocity:
			norm = normalize(self._velocity)
			self._velocity.x = norm.x * self._maxvelocity
			self._velocity.y = norm.y * self._maxvelocity
		
	
	def applyBrake(self, brakingForce):

		if self._velocity.length() <= .001:
			self._velocity.x = 0
			self._velocity.y = 0			
			return
		
		#first normalize to get a unit vector of the direction we are traveling
		norm = normalize(self._velocity)
		if norm.length() == 0:
			self._velocity.x = 0
			self._velocity.y = 0
			return
			
		#negate to get opposite direction
		norm.x = norm.x * -1
		norm.y = norm.y * -1
		
		#apply braking deceleration  
		norm.x *= brakingForce
		norm.y *= brakingForce
		
		self._velocity.x += norm.x * (self._timedelta/1000.0)
		self._velocity.y += norm.y * (self._timedelta/1000.0)
		
	def fire(self, curtime):
		if self._weapon:
			return self._weapon.fire(curtime)
		
		return None
	
	def start(self):
		pass

	def update(self, timedelta):
		self._timedelta = timedelta
		
		shiploc = self.location
		exactloc = shiploc.getExactLayerCoordinates()
		
		exactloc.x += self._velocity.x
		exactloc.y += self._velocity.y
				
		shiploc.setExactLayerCoordinates(exactloc)
		self.location = shiploc
	
	def stop(self):
		pass
		
	def destroy(self):
		pass
		
	location = property(_getLocation,_setLocation)
	instance = property(_getInstance)
	velocity = property(_getVelocity)
	maxvelocity = property(_getMaxVelocity, _setMaxVelocity)
	weapon = property(_getWeapon, _setWeapon)
	