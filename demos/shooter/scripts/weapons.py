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

class Projectile(SpaceObject):
	def __init__(self, model, projectileName, layer, timeToLive):
		super(Projectile, self).__init__(model, projectileName, layer, False)
		self._name = projectileName
		self._layer = layer

		self._obj = self._model.getObject(self._name, "http://www.fifengine.de/xml/tutorial")
	
		self._ttl = timeToLive
		self._starttime = 0
		self._totaltime = 0
	
	def create(self, location):
		self._instance = self._layer.createInstance(self._obj, location.getExactLayerCoordinates(), "bullet")
		fife.InstanceVisual.create(self._instance)

	def run(self, velocity, location, time):
		if not self._running:
			self._velocity = velocity
			self._velocity.x /= self._xscale
			self._velocity.y /= self._yscale
		
			self.create(location)
			self._running = True
			
			self._starttime = time
		
	def destroy(self):
		if self._running and self._instance:
			self._layer.deleteInstance(self._instance)
			self._running = False
		
	def _getTTL(self):
		return self._ttl

	def update(self, timedelta):
		self._totaltime += timedelta
		if self._running and (self._totaltime - self._starttime) < self._ttl:
			super(Projectile, self).update(timedelta)
		else:
			self.destroy()
		
	ttl = property(_getTTL)
	
class Weapon(object):
	def __init__(self, model, layer, ship, firerate):
		self._model = model
		self._layer = layer
		self._ship = ship
		self._firerate = firerate
		self._lastfired = 0
		self._projectileVelocity = fife.DoublePoint(0.075,0)
		
	def fire(self, curtime):
		if (curtime - self._lastfired) > self._firerate:
			pjctl = Projectile(self._model, "bullet1", self._layer, 2000 )
			pjctl.width = 0.05
			pjctl.height = 0.05
			pjctl.run(fife.DoublePoint(self._projectileVelocity.x,self._projectileVelocity.y), self._ship.location, curtime)
			self._lastfired = curtime
			return pjctl
		
		return None
	
