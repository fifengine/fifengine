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

import random
from agent import Agent
from fife.extensions.fife_settings import Setting

#TDS = Setting(app_name="rio_de_hola")

_STATE_NONE, _STATE_IDLE, _STATE_RUN, _STATE_KICK, _STATE_TALK = xrange(5)

class Hero(Agent):
	def __init__(self, settings, model, agentName, layer, uniqInMap=True):
		super(Hero, self).__init__(settings, model, agentName, layer, uniqInMap)
		self.state = _STATE_NONE
		self.idlecounter = 1

	def onInstanceActionFinished(self, instance, action):
		self.idle()
		if action.getId() != 'stand':
			self.idlecounter = 1
		else:
			self.idlecounter += 1
		if self.idlecounter % 7 == 0:
			heroTexts = self.settings.get("rio", "heroTexts")
			txtindex = random.randint(0, len(heroTexts) - 1)
			instance.say(heroTexts[txtindex], 2500)

	def start(self):
		self.idle()

	def idle(self):
		self.state = _STATE_IDLE
		self.agent.act('stand', self.agent.getFacingLocation())

	def run(self, location):
		self.state = _STATE_RUN
		self.agent.move('run', location, 4 * self.settings.get("rio", "TestAgentSpeed"))

	def kick(self, target):
		self.state = _STATE_KICK
		self.agent.act('kick', target)

	def talk(self, target):
		self.state = _STATE_TALK
		self.agent.act('talk', target)
