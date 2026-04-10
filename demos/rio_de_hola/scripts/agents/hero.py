# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Hero agent behaviors for the rio_de_hola demo.

Defines the Hero agent class and its simple behaviors.
"""

import random

from .agent import Agent

# TDS = Setting(app_name="rio_de_hola")

_STATE_NONE, _STATE_IDLE, _STATE_RUN, _STATE_KICK, _STATE_TALK = range(5)


class Hero(Agent):
    """Hero agent with simple idle/run/kick/talk behaviors."""

    def __init__(self, settings, model, agentName, layer, uniqInMap=True):
        """Initialize the hero agent."""
        super().__init__(settings, model, agentName, layer, uniqInMap)
        self.state = _STATE_NONE
        self.idlecounter = 1

    def onInstanceActionFinished(self, instance, action):
        """Handle a finished instance action and occasionally speak."""
        self.idle()
        if action.getId() != "stand":
            self.idlecounter = 1
        else:
            self.idlecounter += 1
        if self.idlecounter % 7 == 0:
            heroTexts = self.settings.get("rio", "heroTexts")
            txtindex = random.randint(0, len(heroTexts) - 1)
            instance.say(heroTexts[txtindex], 2500)

    def onInstanceActionCancelled(self, instance, action):
        """Handle a cancelled instance action."""
        pass

    def start(self):
        """Start the hero."""
        self.idle()

    def idle(self):
        """Set the hero to idle and play the stand action."""
        self.state = _STATE_IDLE
        self.agent.actOnce("stand")

    def run(self, location):
        """Run the hero to the given location."""
        self.state = _STATE_RUN
        self.agent.move("run", location, 4 * self.settings.get("rio", "TestAgentSpeed"))

    def kick(self, target):
        """Kick the given target."""
        self.state = _STATE_KICK
        self.agent.actOnce("kick", target)

    def talk(self, target):
        """Talk to the given target."""
        self.state = _STATE_TALK
        self.agent.actOnce("talk", target)
