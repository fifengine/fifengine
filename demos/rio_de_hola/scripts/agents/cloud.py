# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Cloud agent that drifts across the map in rio_de_hola demo."""

from .agent import Agent

_STATE_NONE, _STATE_FLOATING, _STATE_DISAPPEAR, _STATE_APPEAR = 0, 1, 2, 3


class Cloud(Agent):
    """Cloud agent with simple appear/float/disappear lifecycle."""

    def __init__(self, settings, model, agentName, layer, uniqInMap=False):
        """Initialize the cloud agent."""
        super().__init__(settings, model, agentName, layer, uniqInMap)
        self.state = _STATE_NONE

    def isOutOfBounds(self, c):
        """Return True if coordinates `c` are outside the map bounds.

        Returns
        -------
        bool
            True when the coordinates are outside the map bounds, otherwise False.
        """
        return (c.x < 0) or (c.x > 100) or (c.y < 0) or (c.y > 100)

    def onInstanceActionFinished(self, instance, action):
        """Handle finished instance actions and advance lifecycle."""
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
        """Handle cancelled instance actions."""
        pass

    def start(self, x_dir, y_dir):
        """Start drifting the cloud with direction `(x_dir, y_dir)`."""
        self.x_dir = x_dir
        self.y_dir = y_dir
        self.loc = self.agent.getLocation()
        self.initialCoords = self.agent.getLocation().getExactLayerCoordinates()
        self.appear()

    def appear(self):
        """Make the cloud appear and play the appear action."""
        self.state = _STATE_APPEAR
        self.agent.actOnce("appear", self.loc)

    def disappear(self):
        """Make the cloud disappear and play the disappear action."""
        self.state = _STATE_DISAPPEAR
        self.agent.actOnce("disappear", self.loc)

    def move(self):
        """Advance the cloud by one float step using the default action."""
        self.state = _STATE_FLOATING
        self.agent.actOnce("default", self.loc)
