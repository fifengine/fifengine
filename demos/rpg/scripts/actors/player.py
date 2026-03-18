#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from scripts.actors.baseactor import Actor, ActorActionListener


class PlayerActionListener(ActorActionListener):
    def __init__(self, gamecontroller, obj):
        super(PlayerActionListener, self).__init__(gamecontroller, obj)

    def onInstanceActionFinished(self, instance, action):
        super(PlayerActionListener, self).onInstanceActionFinished(instance, action)
        if action.getId() == "walk":
            pass

    def onInstanceActionCancelled(self, instance, action):
        pass


class Player(Actor):
    def __init__(self, gamecontroller, layer, playermodelname):
        super(Player, self).__init__(
            gamecontroller, layer, "PLAYER", "player", playermodelname, "player", True
        )

        self._actionlistener = PlayerActionListener(self._gamecontroller, self)
        self._actionlistener.attachActionListener()

        self._quests = []

    def serialize(self):
        lvars = super(Player, self).serialize()

        activequests = ""

        for quest in self._gamecontroller.questmanager.activequests:
            if activequests == "":
                activequests = quest.id
            else:
                activequests = activequests + "," + quest.id

        lvars["activequests"] = activequests

        completedquests = ""

        for quest in self._gamecontroller.questmanager.completedquests:
            if completedquests == "":
                completedquests = quest.id
            else:
                completedquests = completedquests + "," + quest.id

        lvars["completedquests"] = completedquests

        return lvars

    def deserialize(self, valuedict):
        super(Player, self).deserialize(valuedict)

        activequests = valuedict["activequests"].split(",")

        for questid in activequests:
            self._gamecontroller.questmanager.activateQuestById(questid)

        completedquests = valuedict["completedquests"].split(",")

        for questid in completedquests:
            self._gamecontroller.questmanager.completeQuestById(questid)
