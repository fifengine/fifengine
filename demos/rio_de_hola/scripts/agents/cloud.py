# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from .agent import Agent

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

    def onInstanceActionCancelled(self, instance, action):
        pass

    def start(self, x_dir, y_dir):
        self.x_dir = x_dir
        self.y_dir = y_dir
        self.loc = self.agent.getLocation()
        self.initialCoords = self.agent.getLocation().getExactLayerCoordinates()
        self.appear()

    def appear(self):
        self.state = _STATE_APPEAR
        self.agent.actOnce("appear", self.loc)

    def disappear(self):
        self.state = _STATE_DISAPPEAR
        self.agent.actOnce("disappear", self.loc)

    def move(self):
        self.state = _STATE_FLOATING
        self.agent.actOnce("default", self.loc)
