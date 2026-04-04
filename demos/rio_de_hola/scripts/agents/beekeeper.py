# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import random

from .agent import Agent

_STATE_NONE, _STATE_TALK = 0, 1


class Beekeeper(Agent):
    def __init__(self, settings, model, agentName, layer, uniqInMap=True):
        super().__init__(settings, model, agentName, layer, uniqInMap)
        self.state = _STATE_NONE

    def onInstanceActionFinished(self, instance, action):
        self.talk()

    def onInstanceActionCancelled(self, instance, action):
        pass

    def start(self):
        self.facingLoc = self.agent.getLocation()
        c = self.facingLoc.getExactLayerCoordinatesRef()
        c.x += random.randint(-1, 1)
        c.y += random.randint(-1, 1)
        self.talk()

    def talk(self):
        self.state = _STATE_TALK
        self.agent.actRepeat("talk", self.facingLoc)  # never calls back
