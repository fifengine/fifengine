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
from scripts.common.baseobject import *

class Projectile(SpaceObject):
	"""
	Represents a projectile (or bullet or fireball) in the scene.

	This is the entity that weapons fire.  Projectiles have an owner
	and a time to live.  They are also what cause damage to ships
	and other entities.
	"""
	def __init__(self, scene, owner, projectileName, timeToLive):
		"""
		@param scene: The scene
		@type scene: L{Scene}
		@param owner: The ship that fired the projectile
		@type owner: L{Ship}
		@param projectileName: The name of the FIFE object to load
		@type projectilName: C{string}
		@param timeToLive: The length of time in milliseconds the projectile will 
		remain active before destroying itself.
		@type timeToLive: C{int}
		
		"""
		super(Projectile, self).__init__(scene, projectileName, False)

		self._obj = self._model.getObject(self._name, "http://www.fifengine.de/xml/tutorial")
	
		self._type = SHTR_PROJECTILE
	
		self._ttl = timeToLive
		self._starttime = 0
		self._totaltime = 0
		
		self._owner = owner
		
		self.width = 0.025
		self.height = 0.025
		
		self._damage = 1
	
	def create(self, location):
		"""
		Spawns the projectile.
		
		@param location: The location to create the projectile
		@type location: L{fife.Location}
		
		@note: This is called by L{Projectile.run}
		"""
		self._instance = self._layer.createInstance(self._obj, location.getExactLayerCoordinates(), "bullet")
		fife.InstanceVisual.create(self._instance)
		self._instance.thisown = 0

	def run(self, velocity, location):
		"""
		Start the projectile on it's path.
		
		@param velocity: The starting velocity of the projectile
		@type velocity: L{fife.DoublePoint}
		@param location: The location to create the projectile
		@type location: L{fife.Location}
		
		"""
		if not self._running:
			self._velocity = velocity
			self._velocity.x /= self._xscale
			self._velocity.y /= self._yscale
		
			self.create(location)
			self._running = True
			
			self._starttime = self._scene.time
		
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
			
	def _getDamage(self):
		return self._damage
		
	def _setDamage(self, dam):
		self._damage = dam
		
	ttl = property(_getTTL)
	owner = property(_getOwner)
	damage = property(_getDamage, _setDamage)
	
class Weapon(object):
	"""
	Weapon
	
	This class is a super class and is meant to be inherited and
	not used directly.  You should implement fire() in the sub-
	class.  The Weapon class spawns Projectile(s) and fires them
	in the specified direction at a specified fire rate.
	"""
	def __init__(self, scene, ship, firerate):
		self._scene = scene
		self._model = self._scene.model
		self._layer = self._scene.objectlayer
		self._ship = ship
		self._firerate = firerate
		self._lastfired = 0
		self._projectileVelocity = 0.75
		self._soundclip = None
		
	def fire(self, direction):
		"""
		Fires the weapon in the specified direction.
		
		@param direction: A normalized vector specifying the direction to fire the projectile
		@type direction: L{fife.DoublePoint}
		"""
		pass
		
	def _getProjectileVelocity(self):
		return self._projectileVelocity
	
	def _setProjectileVelocity(self, vel):
		self._projectileVelocity = vel
		
	def _getLastFired(self):
		return self._lastfired
		
	def _setLastFired(self, time):
		self._lastfired = time
		
	projectilevelocity = property(_getProjectileVelocity, _setProjectileVelocity)
	lastfired = property(_getLastFired, _setLastFired)
	
class Cannon(Weapon):
	def __init__(self, scene, ship, firerate):
		super(Cannon, self).__init__(scene, ship, firerate)
		
		self._projectileVelocity = 0.75
		self._soundclip = scene.soundmanager.createSoundEmitter("sounds/cannon.ogg")

		
	def fire(self, direction):
		velocity = fife.DoublePoint(direction)
		velocity.normalize()
		velocity.x = velocity.x * self._projectileVelocity
		velocity.y = velocity.y * self._projectileVelocity
	
		if (self._scene.time - self._lastfired) > self._firerate:
			pjctl = Projectile(self._scene, self._ship, "bullet1", 3000 )
			pjctl.run(velocity, self._ship.location)
			self._lastfired = self._scene.time
			self._scene.addObjectToScene(pjctl)
			if self._soundclip:
				location = self._ship.location.getExactLayerCoordinates()
				self._soundclip.position = (location.x, location.y)
				self._soundclip.play()

class FireBall(Weapon):
	def __init__(self, scene, ship, firerate):
		super(FireBall, self).__init__(scene, ship, firerate)
		
		self._projectileVelocity = 0.50
		self._soundclip = scene.soundmanager.createSoundEmitter("sounds/fireball.ogg")		

	def fire(self, direction):
		velocity = fife.DoublePoint(direction)
		velocity.normalize()
		velocity.x = velocity.x * self._projectileVelocity
		velocity.y = velocity.y * self._projectileVelocity
	
		if (self._scene.time - self._lastfired) > self._firerate:
			pjctl = Projectile(self._scene, self._ship, "fireball", 6000 )
			pjctl.run(velocity, self._ship.location)
			self._lastfired = self._scene.time
			self._scene.addObjectToScene(pjctl)	
			if self._soundclip:
				self._soundclip.play()
			
class FireBallBurst(Weapon):
	def __init__(self, scene, ship, firerate, burstrate, burstnumber):
		super(FireBallBurst, self).__init__(scene, ship, firerate)
		
		self._projectileVelocity = 0.50
		self._soundclip = scene.soundmanager.createSoundEmitter("sounds/fireball.ogg")	

		self._burstrate = burstrate
		self._burstnumber = int(burstnumber)
		self._burstcount = int(burstnumber)
		
		self._lastburstfired = 0

		
	def fire(self, direction):
		velocity = fife.DoublePoint(direction)
		velocity.normalize()
		velocity.x = velocity.x * self._projectileVelocity
		velocity.y = velocity.y * self._projectileVelocity
	
		if (self._scene.time - self._lastfired) > self._firerate:
			if (self._scene.time - self._lastburstfired) > self._burstrate and self._burstcount > 0:
				pjctl = Projectile(self._scene, self._ship, "fireball", 6000 )
				pjctl.run(velocity, self._ship.location)
				self._scene.addObjectToScene(pjctl)
			
				if self._soundclip:
					self._soundclip.play()		
				
				self._lastburstfired = self._scene.time
				self._burstcount -= 1
				
			if self._burstcount <= 0:
				self._lastfired = self._scene.time
				self._burstcount = int(self._burstnumber)
				self._lastburstfired = 0
				
				
class FireBallSpread(Weapon):
	def __init__(self, scene, ship, firerate):
		super(FireBallSpread, self).__init__(scene, ship, firerate)
		
		self._projectileVelocity = 0.50
		self._soundclip = scene.soundmanager.createSoundEmitter("sounds/fireball.ogg")
		
	def fire(self, direction):
	
		if (self._scene.time - self._lastfired) > self._firerate:
			velocity = fife.DoublePoint(direction)
			velocity.normalize()
			velocity.x = velocity.x * self._projectileVelocity
			velocity.y = velocity.y * self._projectileVelocity

			origin = fife.DoublePoint(0,0)
		
			p1 = fife.DoublePoint(velocity)
			p2 = fife.DoublePoint(velocity)
			p3 = fife.DoublePoint(velocity)
			p4 = fife.DoublePoint(velocity)
			p5 = fife.DoublePoint(velocity)
			p6 = fife.DoublePoint(velocity)
			p7 = fife.DoublePoint(velocity)
			
			p1.rotate(origin, -30)
			p2.rotate(origin, -20)
			p3.rotate(origin, -10)
			p4.rotate(origin, 0)
			p5.rotate(origin, 10)
			p6.rotate(origin, 20)
			p7.rotate(origin, 30)
			
			pjctl1 = Projectile(self._scene, self._ship, "fireball", 6000 )
			pjctl1.run(p1, self._ship.location)
			self._scene.addObjectToScene(pjctl1)
			
			pjctl2 = Projectile(self._scene, self._ship, "fireball", 6000 )
			pjctl2.run(p2, self._ship.location)
			self._scene.addObjectToScene(pjctl2)
	
			pjctl3 = Projectile(self._scene, self._ship, "fireball", 6000 )
			pjctl3.run(p3, self._ship.location)
			self._scene.addObjectToScene(pjctl3)
			
			pjctl4 = Projectile(self._scene, self._ship, "fireball", 6000 )
			pjctl4.run(p4, self._ship.location)
			self._scene.addObjectToScene(pjctl4)
	
			pjctl5 = Projectile(self._scene, self._ship, "fireball", 6000 )
			pjctl5.run(p5, self._ship.location)
			self._scene.addObjectToScene(pjctl5)
			
			pjctl6 = Projectile(self._scene, self._ship, "fireball", 6000 )
			pjctl6.run(p6, self._ship.location)
			self._scene.addObjectToScene(pjctl6)
			
			pjctl7 = Projectile(self._scene, self._ship, "fireball", 6000 )
			pjctl7.run(p7, self._ship.location)
			self._scene.addObjectToScene(pjctl7)
			
			if self._soundclip:
				self._soundclip.play()

			self._lastfired = self._scene.time
			
class CannonSpread5(Weapon):
	def __init__(self, scene, ship, firerate):
		super(CannonSpread5, self).__init__(scene, ship, firerate)
		
		self._projectileVelocity = 1
		self._soundclip = scene.soundmanager.createSoundEmitter("sounds/cannon.ogg")

	def fire(self, direction):
	
		if (self._scene.time - self._lastfired) > self._firerate:
			velocity = fife.DoublePoint(direction)
			velocity.normalize()
			velocity.x = velocity.x * self._projectileVelocity
			velocity.y = velocity.y * self._projectileVelocity

			origin = fife.DoublePoint(0,0)
			
			p2 = fife.DoublePoint(velocity)
			p3 = fife.DoublePoint(velocity)
			p4 = fife.DoublePoint(velocity)
			p5 = fife.DoublePoint(velocity)
			p6 = fife.DoublePoint(velocity)
			
			p2.rotate(origin, -10)
			p3.rotate(origin, -5)
			p4.rotate(origin, 0)
			p5.rotate(origin, 5)
			p6.rotate(origin, 10)
			
			pjctl2 = Projectile(self._scene, self._ship, "bullet1", 3000 )
			pjctl2.run(p2, self._ship.location)
			self._scene.addObjectToScene(pjctl2)
	
			pjctl3 = Projectile(self._scene, self._ship, "bullet1", 3000 )
			pjctl3.run(p3, self._ship.location)
			self._scene.addObjectToScene(pjctl3)
			
			pjctl4 = Projectile(self._scene, self._ship, "bullet1", 3000 )
			pjctl4.run(p4, self._ship.location)
			self._scene.addObjectToScene(pjctl4)
	
			pjctl5 = Projectile(self._scene, self._ship, "bullet1", 3000 )
			pjctl5.run(p5, self._ship.location)
			self._scene.addObjectToScene(pjctl5)
			
			pjctl6 = Projectile(self._scene, self._ship, "bullet1", 3000 )
			pjctl6.run(p6, self._ship.location)
			self._scene.addObjectToScene(pjctl6)
			
			if self._soundclip:
				self._soundclip.play()
			
			self._lastfired = self._scene.time

