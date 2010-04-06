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
from scripts.ships.shipbase import Ship
from scripts.common.helpers import Rect
from scripts.weapons import Weapon


class Saucer1(Ship):
	def __init__(self, scene, name, findInstance=True):
		super(Saucer1, self).__init__(scene, name, findInstance)
		self._dir = 0
		self._time = 500
		self.width = 0.075
		self.height = 0.075
		self.velocity.x = -0.5
		
		self.weapon = Weapon(self._scene, self, 1000)
				
	def update(self, timedelta):	
		if self._dir == 1:
			self.applyThrust(fife.DoublePoint(0,-0.5), timedelta)
		elif self._dir == 0:
			self.applyThrust(fife.DoublePoint(0,0.5), timedelta)
					
		if self._time >= 1000:
			if self._dir == 1:
				self._dir = 0
			elif self._dir == 0:
				self._dir = 1
				
			self._time = 0
		
		self._time += timedelta
		
		super(Saucer1, self).update(timedelta)
		
class Saucer2(Ship):
	def __init__(self, scene, name, findInstance=True):
		super(Saucer2, self).__init__(scene, name, findInstance)
		self._dir = 0
		self._time = 1000
		self.width = 0.2
		self.height = 0.2
		self.velocity.x = -0.1
		
		self.weapon = Weapon(self._scene, self, 2000)
				
	def update(self, timedelta):	
		if self._dir == 1:
			self.applyThrust(fife.DoublePoint(0,-0.25), timedelta)
		elif self._dir == 0:
			self.applyThrust(fife.DoublePoint(0,0.25), timedelta)
					
		if self._time >= 2000:
			if self._dir == 1:
				self._dir = 0
			elif self._dir == 0:
				self._dir = 1
				
			self._time = 0
		
		self._time += timedelta
		
		super(Saucer2, self).update(timedelta)