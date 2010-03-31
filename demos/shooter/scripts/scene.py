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
from scripts.ships.player import Player
from scripts.common.helpers import Rect




class Scene(object):
	def __init__(self, engine, objectLayer):
		self._engine = engine
		self._model = engine.getModel()
		self._layer = objectLayer
		
		self._player = Player(self._model, 'player', self._layer)
		self._projectiles = list()
		self._lasttime = 0
		
	def attachCamera(self, cam):
		self._camera = cam
		self._camera.setLocation(self._player.location)
		
	def _getPlayer(self):
		return self._player

	def update(self, time, keystate):
		timedelta = time - self._lasttime
		self._lasttime = time
		
		#update camera location
		loc = self._camera.getLocation()
		exactloc = self._camera.getLocation().getExactLayerCoordinates()
		exactloc.x += timedelta * 0.001
		loc.setExactLayerCoordinates(exactloc)
		self._camera.setLocation(loc)

		self._player.update(timedelta, keystate, self._camera)
		
		#update the list of projectiles
		todelete = list()
		for p in self._projectiles:
			p.update(time)
			if not p.running:
				todelete.append(p)
		for p in todelete:
			self._projectiles.remove(p)
		
		#fire the currently selected gun
		if keystate['SPACE']:
			prjct = self._player.fire(time)
			if prjct:
				self._projectiles.append(prjct)

	player = property(_getPlayer)
		
