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

from agent import Agent
from fife import fife
import random

_STATE_NONE, _STATE_FLOATING, _STATE_DISAPPEAR, _STATE_APPEAR = 0, 1, 2, 3

class Cloud(Agent):
	def __init__(self, settings, model, agentName, layer, uniqInMap=False):
		super(Cloud, self).__init__(settings, model, agentName, layer, uniqInMap)
		self.state = _STATE_NONE

	def isOutOfBounds(self, c):
		return (c.x < 0) or (c.x > 100) or (c.y < 0) or (c.y > 100)

	def onInstanceActionFinished(self, instance, action):
		if self.state == _STATE_APPEAR:
			self.move()
		elif self.state == _STATE_FLOATING:
			c = self.agent.getLocationRef().getExactLayerCoordinatesRef()
			c.x += self.x_dir
			c.y += self.y_dir
			if self.isOutOfBounds(c):
				self.disappear()
			else:
				self.move()
		elif self.state == _STATE_DISAPPEAR:
			self.agent.getLocationRef().setExactLayerCoordinates(self.initialCoords)
			self.appear()

	def start(self, x_dir, y_dir):
		self.x_dir = x_dir
		self.y_dir = y_dir
		self.loc = self.agent.getLocation()
		self.initialCoords = self.agent.getLocation().getExactLayerCoordinates()
		self.appear()

	def appear(self):
		self.state = _STATE_APPEAR
		self.agent.act('appear', self.loc, False)

	def disappear(self):
		self.state = _STATE_DISAPPEAR
		self.agent.act('disappear', self.loc, False)

	def move(self):
		self.state = _STATE_FLOATING
		self.agent.act('default', self.loc, False)
