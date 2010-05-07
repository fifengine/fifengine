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

from fife import fife
from scripts.ships.shipbase import SpaceObject
from scripts.weapons import *

class PowerUp(SpaceObject):
	def __init__(self, scene, powerupName, instance, findInstance=True):
		super(PowerUp, self).__init__(scene, powerupName, findInstance)
		
		self.instance = instance
		self._type = SHTR_POWERUP		

		self.width = 0.025
		self.height = 0.025
			
	def applyPowerUp(self, ship):
		self.destroy()
		self._scene.queueObjectForRemoval(self)	
		
class CannonSpread5PU(PowerUp):
	def __init__(self, scene, powerupName, instance, findInstance=True):
		super(CannonSpread5PU, self).__init__(scene, powerupName, instance, findInstance)
		
		self._dir = 0
		self._time = 1500
		self._velocity.x = -0.25
		self._velocity.y = 0
		
		self._pickupclip = self._scene.soundmanager.createSoundEmitter("sounds/pickup.ogg")		
	
	def applyPowerUp(self, ship):
		ship.weapon = CannonSpread5(self._scene, ship, 300)
		self._pickupclip.play()
		self.destroy()
		self._scene.queueObjectForRemoval(self)
		
	def update(self):	
		if self._dir == 1:
			self._velocity.y = -0.25
		elif self._dir == 0:
			self._velocity.y = 0.25
					
		if self._time >= 3000:
			if self._dir == 1:
				self._dir = 0
			elif self._dir == 0:
				self._dir = 1
				
			self._time = 0
		
		self._time += self._scene.timedelta	
		
		super(CannonSpread5PU, self).update()
		
class ExtraLifePU(PowerUp):
	def __init__(self, scene, powerupName, instance, findInstance=True):
		super(ExtraLifePU, self).__init__(scene, powerupName, instance, findInstance)
		
		self._pickupclip = self._scene.soundmanager.createSoundEmitter("sounds/pickup.ogg")	
	
	def applyPowerUp(self, ship):
		ship.lives += 1
		self._pickupclip.play()
		self.destroy()
		self._scene.queueObjectForRemoval(self)	
		
