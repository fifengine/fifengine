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
from scripts.ships.shipbase import *
from scripts.common.baseobject import *
from fife.fife import FloatRect as Rect
from scripts.weapons import *
from fife.extensions import fife_timer


class EnemyActionListener(ShipActionListener):
	def __init__(self, ship):
		super(EnemyActionListener, self).__init__(ship)

	def onInstanceActionFinished(self, instance, action):
		super(EnemyActionListener, self).onInstanceActionFinished(instance, action)


class BossActionListener(ShipActionListener):
	def __init__(self, ship):
		super(BossActionListener, self).__init__(ship)

	def onInstanceActionFinished(self, instance, action):
		super(BossActionListener, self).onInstanceActionFinished(instance, action)

		if action.getId() == 'explode':
			self.delayed = fife_timer.delayCall(5000,self._ship.endLevel())

				
class Saucer1(Ship):
	def __init__(self, scene, name, instance, findInstance=True):
		super(Saucer1, self).__init__(scene, name, findInstance)
		self.instance = instance
		self._type = SHTR_ENEMYSHIP
		self._dir = 0
		self._time = 500
		self.width = 0.2
		self.height = 0.075
		self.velocity.x = -0.5
		
		self.weapon = Cannon(self._scene, self, 1000)
		self.weapon.projectilevelocity = 0.4
		
		self._actionlistener = EnemyActionListener(self)
		
		self.hitpoints = 1
		self.scorevalue = 50
				
	def update(self):	
		if self._dir == 1:
			self.applyThrust(fife.DoublePoint(0,-0.5))
		elif self._dir == 0:
			self.applyThrust(fife.DoublePoint(0,0.5))
					
		if self._time >= 1000:
			if self._dir == 1:
				self._dir = 0
			elif self._dir == 0:
				self._dir = 1
				
			self._time = 0
		
		self._time += self._scene.timedelta
		
		super(Saucer1, self).update()
	
		self.fire(fife.DoublePoint(-1,0))		
		
		
class Saucer2(Ship):
	def __init__(self, scene, name, instance, findInstance=True):
		super(Saucer2, self).__init__(scene, name, findInstance)
		self.instance = instance
		self._type = SHTR_ENEMYSHIP
		self._dir = 0
		self._time = 1000
		self.width = 0.2
		self.height = 0.2
		self.velocity.x = -0.1
		
		self.weapon = Cannon(self._scene, self, 2000)
		self.weapon.projectilevelocity = 0.4
		
		self._actionlistener = EnemyActionListener(self)
		
		self.hitpoints = 2
		self.scorevalue = 100

	def applyHit(self, hp):
		self.flash(1)
		super(Saucer2, self).applyHit(hp)
		
	def update(self):	
		if self._dir == 1:
			self.applyThrust(fife.DoublePoint(0,-0.25))
		elif self._dir == 0:
			self.applyThrust(fife.DoublePoint(0,0.25))
					
		if self._time >= 2000:
			if self._dir == 1:
				self._dir = 0
			elif self._dir == 0:
				self._dir = 1
				
			self._time = 0
		
		self._time += self._scene.timedelta
		
		super(Saucer2, self).update()
		
		self.fire(fife.DoublePoint(-1,0))		
		
class DiagSaucer(Ship):
	def __init__(self, scene, name, direction, instance, findInstance=True):
		super(DiagSaucer, self).__init__(scene, name, findInstance)
		self.instance = instance
		self._type = SHTR_ENEMYSHIP
		self.width = 0.2
		self.height = 0.075
		
		if direction == 0:
			self._ythrust = 0.25
		else:
			self._ythrust = -0.25
		
		self.weapon = Cannon(self._scene, self, 2000)
		self.weapon.projectilevelocity = 0.4
		
		self._actionlistener = EnemyActionListener(self)
		
		self.hitpoints = 1
		self.scorevalue = 50
				
	def update(self):	
		self.applyThrust(fife.DoublePoint(-0.25,self._ythrust))
		super(DiagSaucer, self).update()

		self.fire(fife.DoublePoint(-1,0))		

		
class Streaker(Ship):
	def __init__(self, scene, name, instance, findInstance=True):
		super(Streaker, self).__init__(scene, name, findInstance)
		self.instance = instance
		self._type = SHTR_ENEMYSHIP
		self.width = 0.2
		self.height = 0.2		
		
		self._maxvelocity = 2.0
		
		self.weapon = FireBall(self._scene, self, 2000)
		self.weapon.projectilevelocity = 0.25
		
		self._actionlistener = EnemyActionListener(self)
		
		self.hitpoints = 2
		self.scorevalue = 150

	def applyHit(self, hp):
		self.flash(1)
		super(Streaker, self).applyHit(hp)
		
	def update(self):	
		self.applyThrust(fife.DoublePoint(-0.40,0))
		super(Streaker, self).update()

		playerloc = self._scene.player.location.getExactLayerCoordinates()
		enemyloc = self.location.getExactLayerCoordinates()
		
		playerloc.x -= enemyloc.x
		playerloc.y -= enemyloc.y
		
		self.fire(fife.DoublePoint(playerloc.x,playerloc.y))

		
class Boss(Ship):
	def __init__(self, scene, name, instance, findInstance=True):
		super(Boss, self).__init__(scene, name, findInstance)
		self.instance = instance
		self._type = SHTR_LASTBOSS
		self.width = 0.85
		self.height = 0.25		
		
		self._maxvelocity = 2.0
		
		self.weapon = FireBall(self._scene, self, 1000)
		self.weapon.projectilevelocity = 0.5
		
		self._actionlistener = BossActionListener(self)
		
		self.hitpoints = 30
		self.scorevalue = 1000
		
		self._explodclip = self._scene.soundmanager.createSoundEmitter("sounds/bossexplode.ogg")
		
	def endLevel(self):
		self._scene.endLevel()
		
	def update(self):
		super(Boss, self).update()
		
		playerloc = self._scene.player.location.getExactLayerCoordinates()
		bossloc = self.location.getExactLayerCoordinates()
		
		playerloc.x -= bossloc.x
		playerloc.y -= bossloc.y
		
		self.fire(fife.DoublePoint(playerloc.x,playerloc.y))		
		
		
	def applyHit(self, hp):
		self.flash(2)
		super(Boss, self).applyHit(hp)
		
		if self.hitpoints == 20:
			self.weapon = FireBallBurst(self._scene, self, 2000, 100, 10)
			self.weapon.lastfired = self._scene.time
			
		elif self.hitpoints == 10:
			self.weapon = FireBallSpread(self._scene, self, 2000)
			self.weapon.lastfired = self._scene.time
			
