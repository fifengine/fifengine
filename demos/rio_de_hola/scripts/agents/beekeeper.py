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

_STATE_NONE, _STATE_TALK = 0, 1

class Beekeeper(Agent):
	def __init__(self, settings, model, agentName, layer, uniqInMap=True):
		super(Beekeeper, self).__init__(settings, model, agentName, layer, uniqInMap)
		self.state = _STATE_NONE

	def onInstanceActionFinished(self, instance, action):
		self.talk()

	def start(self):
		self.facingLoc = self.agent.getLocation()
		c = self.facingLoc.getExactLayerCoordinatesRef()
		c.x += random.randint(-1, 1)
		c.y += random.randint(-1, 1)
		self.talk()

	def talk(self):
		self.state = _STATE_TALK
		self.agent.act('talk', self.facingLoc, True) # never calls back
