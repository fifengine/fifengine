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
from scripts.ships.shipbase import SpaceObject
from scripts.common.helpers import normalize

class Projectile(SpaceObject):
	def __init__(self, scene, owner, projectileName, timeToLive):
		super(Projectile, self).__init__(scene, projectileName, False)

		self._obj = self._model.getObject(self._name, "http://www.fifengine.de/xml/tutorial")
	
		self._ttl = timeToLive
		self._starttime = 0
		self._totaltime = 0
		
		self._owner = owner
		
		self.width = 0.025
		self.height = 0.025
	
	def create(self, location):
		self._instance = self._layer.createInstance(self._obj, location.getExactLayerCoordinates(), "bullet")
		fife.InstanceVisual.create(self._instance)

	def run(self, velocity, location):
		if not self._running:
			self._velocity = velocity
			self._velocity.x /= self._xscale
			self._velocity.y /= self._yscale
		
			self.create(location)
			self._running = True
			
			self._starttime = self._scene.time
		
	def destroy(self):
		if self._running and self._instance:
			self._layer.deleteInstance(self._instance)
			self._running = False
		
	def _getTTL(self):
		return self._ttl
	
	def _getOwner(self):
		return self._owner

	def update(self):
		self._totaltime += self._scene.timedelta
		if self._running and self._totaltime < self._ttl:
			super(Projectile, self).update()
		else:
			self.destroy()
		
	ttl = property(_getTTL)
	owner = property(_getOwner)
	
class Weapon(object):
	def __init__(self, scene, ship, firerate):
		self._scene = scene
		self._model = self._scene.model
		self._layer = self._scene.objectlayer
		self._ship = ship
		self._firerate = firerate
		self._lastfired = 0
		self._projectileVelocity = 0.75
		
	def fire(self, direction):
		return None
		
	def _getProjectileVelocity(self):
		return self._projectileVelocity
	
	def _setProjectileVelocity(self, vel):
		self._projectileVelocity = vel
		
	projectilevelocity = property(_getProjectileVelocity, _setProjectileVelocity)
	
class Cannon(Weapon):
	def __init__(self, scene, ship, firerate):
		super(Cannon, self).__init__(scene, ship, firerate)
		
		#cannon's projectile velocity
		self._projectileVelocity = 0.75

		
	def fire(self, direction):
		velocity = normalize(direction)
		velocity.x = velocity.x * self._projectileVelocity
		velocity.y = velocity.y * self._projectileVelocity
	
		if (self._scene.time - self._lastfired) > self._firerate:
			pjctl = Projectile(self._scene, self._ship, "bullet1", 6000 )
			pjctl.run(velocity, self._ship.location)
			self._lastfired = self._scene.time
			self._scene.addProjectileToScene(pjctl)
			

