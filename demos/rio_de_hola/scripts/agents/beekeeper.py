# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Simple beekeeper agent for rio_de_hola demo."""

import random

from .agent import Agent

_STATE_NONE, _STATE_TALK = 0, 1


class Beekeeper(Agent):
    """Beekeeper agent that occasionally talks in place."""

    def __init__(self, settings, model, agentName, layer, uniqInMap=True):
        """Initialize the beekeeper agent."""
        super().__init__(settings, model, agentName, layer, uniqInMap)
        self.state = _STATE_NONE

    def onInstanceActionFinished(self, instance, action):
        """Handle finished instance actions by starting talk behavior."""
        self.talk()

    def onInstanceActionCancelled(self, instance, action):
        """Handle cancelled instance actions."""
        pass

    def start(self):
        """Start the agent by choosing a nearby facing location and talking."""
        self.facingLoc = self.agent.getLocation()
        c = self.facingLoc.getExactLayerCoordinatesRef()
        c.x += random.randint(-1, 1)
        c.y += random.randint(-1, 1)
        self.talk()

    def talk(self):
        """Set state to talk and repeat the talk action."""
        self.state = _STATE_TALK
        self.agent.actRepeat("talk", self.facingLoc)  # never calls back
