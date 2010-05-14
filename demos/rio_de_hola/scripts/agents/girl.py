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
from fife.extensions.fife_settings import Setting

#TDS = Setting(app_name="rio_de_hola")

_STATE_NONE, _STATE_IDLE, _STATE_RUN, _STATE_FOLLOW = 0, 1, 2, 3

class Girl(Agent):
	def __init__(self, settings, model, agentName, layer, uniqInMap=True):
		super(Girl, self).__init__(settings, model, agentName, layer, uniqInMap)
		self.state = _STATE_NONE
		self.waypoints = ((67, 80), (75, 44))
		self.waypoint_counter = 0
		self.hero = self.layer.getInstance('PC')
		
		self.GIRL_SPEED = 3 * float(self.settings.get("rio", "TestAgentSpeed"))

	def onInstanceActionFinished(self, instance, action):
		if self.state in (_STATE_RUN, _STATE_FOLLOW):
			self.idle()
		else:
			if self.waypoint_counter % 3:
				self.waypoint_counter += 1
				self.follow_hero()
			else:
				self.run(self.getNextWaypoint())

	def getNextWaypoint(self):
		self.waypoint_counter += 1
		l = fife.Location(self.layer)
		l.setLayerCoordinates(fife.ModelCoordinate(*self.waypoints[self.waypoint_counter % len(self.waypoints)]))
		return l

	def start(self):
		self.follow_hero()

	def idle(self):
		self.state = _STATE_IDLE
		self.agent.act('stand', self.agent.getFacingLocation(), False)

	def follow_hero(self):
		self.state = _STATE_FOLLOW
		self.agent.follow('run', self.hero, self.GIRL_SPEED)

	def run(self, location):
		self.state = _STATE_RUN
		self.agent.move('run', location, self.GIRL_SPEED)
