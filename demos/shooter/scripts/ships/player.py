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


class Player(Ship):
	def __init__(self, scene, shipName, layer):
		super(Player, self).__init__(scene, shipName, layer, True)
		self._bounds = Rect(-100,-100,200,200)
		
	def update(self, timedelta, keystate, camera):
		key = False
		
		oldpos = self.location
		
		if keystate['UP']:
			self.applyThrust(fife.DoublePoint(0,-0.075))
			key = True
		if keystate['DOWN']:
			self.applyThrust(fife.DoublePoint(0,0.075))
			key = True
		if keystate['LEFT']:
			self.applyThrust(fife.DoublePoint(-0.075,0))
			key = True
		if keystate['RIGHT']:
			self.applyThrust(fife.DoublePoint(0.075,0))
			key = True
			
		if not key and self._velocity.length() > 0:
			self.applyBrake(0.075)
		
		super(Player, self).update(timedelta)
		
		#set up the players camera bounds
		#TODO: grab screen resolution from somewhere
		topleft = camera.toMapCoordinates(fife.ScreenPoint(0,0))
		bottomright = camera.toMapCoordinates(fife.ScreenPoint(1024,768))

		#add a little padding to the left edge
		topleft.x += 0.1
		
		camrect = Rect(topleft.x, topleft.y, bottomright.x - topleft.x, bottomright.y - topleft.y)
	
		#player bounding box
		#TODO: make this configurable
		pos = self.location.getExactLayerCoordinates()
		bbox = Rect()
		bbox.x = pos.x - 0.005
		bbox.y = pos.y - 0.005
		bbox.w = 0.01
		bbox.h = 0.01
		
		if not bbox.intersects(camrect):
			if pos.x < topleft.x:
				pos.x += timedelta * 0.0005
				oldpos.setExactLayerCoordinates(pos)
				self._velocity.x = timedelta * 0.0005
			else:
				self._velocity.x = 0

			self._velocity.y = 0

			self.location = oldpos

