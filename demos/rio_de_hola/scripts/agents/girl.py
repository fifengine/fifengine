# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife

from .agent import Agent

# TDS = Setting(app_name="rio_de_hola")

_STATE_NONE, _STATE_IDLE, _STATE_RUN, _STATE_FOLLOW = 0, 1, 2, 3


class Girl(Agent):
    def __init__(self, settings, model, agentName, layer, uniqInMap=True):
        super().__init__(settings, model, agentName, layer, uniqInMap)
        self.state = _STATE_NONE
        self.waypoints = ((67, 80), (75, 44))
        self.waypoint_counter = 0
        self.hero = self.layer.getInstance("PC")

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

    def onInstanceActionCancelled(self, instance, action):
        pass

    def getNextWaypoint(self):
        self.waypoint_counter += 1
        location = fife.Location(self.layer)
        location.setLayerCoordinates(
            fife.Point3D(*self.waypoints[self.waypoint_counter % len(self.waypoints)], 0)
        )
        return location

    def start(self):
        self.follow_hero()

    def idle(self):
        self.state = _STATE_IDLE
        self.agent.actOnce("stand")

    def follow_hero(self):
        self.state = _STATE_FOLLOW
        self.agent.follow("run", self.hero, self.GIRL_SPEED)

    def run(self, location):
        self.state = _STATE_RUN
        self.agent.move("run", location, self.GIRL_SPEED)
