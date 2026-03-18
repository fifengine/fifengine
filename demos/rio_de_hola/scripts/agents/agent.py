# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife

from scripts.common.common import ProgrammingError


class Agent(fife.InstanceActionListener):
    def __init__(self, settings, model, agentName, layer, uniqInMap=True):
        fife.InstanceActionListener.__init__(self)
        self.settings = settings
        self.model = model
        self.agentName = agentName
        self.layer = layer
        if uniqInMap:
            self.agent = layer.getInstance(agentName)
            self.agent.addActionListener(self)

    def onInstanceActionFinished(self, instance, action):
        raise ProgrammingError("No OnActionFinished defined for Agent")

    def onInstanceActionCancelled(self, instance, action):
        raise ProgrammingError("No OnActionFinished defined for Agent")

    def onInstanceActionFrame(self, instance, action, frame):
        raise ProgrammingError("No OnActionFrame defined for Agent")

    def start(self):
        raise ProgrammingError("No start defined for Agent")


def create_anonymous_agents(settings, model, objectName, layer, agentClass):
    agents = []
    instances = [a for a in layer.getInstances() if a.getObject().getId() == objectName]
    i = 0
    for a in instances:
        agentName = "%s:i:%d" % (objectName, i)
        i += 1
        agent = agentClass(settings, model, agentName, layer, False)
        agent.agent = a
        a.addActionListener(agent)
        agents.append(agent)
    return agents
