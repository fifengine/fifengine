# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Agent base and helpers for rio_de_hola demo."""

from fife import fife

from scripts.common.common import ProgrammingError


class Agent(fife.InstanceActionListener):
    """Base agent that wraps FIFE instance action listening.

    Subclasses should override the action handlers and `start()`.
    """

    def __init__(self, settings, model, agentName, layer, uniqInMap=True):
        """Initialize the base agent and optionally attach to an instance."""
        fife.InstanceActionListener.__init__(self)
        self.settings = settings
        self.model = model
        self.agentName = agentName
        self.layer = layer
        if uniqInMap:
            self.agent = layer.getInstance(agentName)
            self.agent.addActionListener(self)

    def onInstanceActionFinished(self, instance, action):
        """Handle a finished instance action; override in subclasses.

        Raises
        ------
        ProgrammingError
            If not overridden in a subclass.
        """
        raise ProgrammingError("No OnActionFinished defined for Agent")

    def onInstanceActionCancelled(self, instance, action):
        """Handle a cancelled instance action; override in subclasses.

        Raises
        ------
        ProgrammingError
            If not overridden in a subclass.
        """
        raise ProgrammingError("No OnActionFinished defined for Agent")

    def onInstanceActionFrame(self, instance, action, frame):
        """Handle action frame updates; override in subclasses.

        Raises
        ------
        ProgrammingError
            If not overridden in a subclass.
        """
        raise ProgrammingError("No OnActionFrame defined for Agent")

    def start(self):
        """Start agent behavior; override in subclasses.

        Raises
        ------
        ProgrammingError
            If not overridden in a subclass.
        """
        raise ProgrammingError("No start defined for Agent")


def create_anonymous_agents(settings, model, objectName, layer, agentClass):
    """Create agents for each instance of `objectName` on `layer`.

    Returns
    -------
    list
        List of created agent instances.
    """
    agents = []
    instances = [a for a in layer.getInstances() if a.getObject().getId() == objectName]
    i = 0
    for a in instances:
        agentName = f"{objectName}:i:{i}"
        i += 1
        agent = agentClass(settings, model, agentName, layer, False)
        agent.agent = a
        a.addActionListener(agent)
        agents.append(agent)
    return agents
