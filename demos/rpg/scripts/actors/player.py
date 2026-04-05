#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from scripts.actors.baseactor import Actor, ActorActionListener


class PlayerActionListener(ActorActionListener):
    def __init__(self, gamecontroller, obj):
        super().__init__(gamecontroller, obj)

    def onInstanceActionFinished(self, instance, action):
        super().onInstanceActionFinished(instance, action)
        if action.getId() == "walk":
            pass

    def onInstanceActionCancelled(self, instance, action):
        pass


class Player(Actor):
    def __init__(self, gamecontroller, layer, playermodelname):
        super().__init__(
            gamecontroller, layer, "PLAYER", "player", playermodelname, "player", True
        )

        self._actionlistener = PlayerActionListener(self._gamecontroller, self)
        self._actionlistener.attachActionListener()

        self._quests = []

    def serialize(self):
        lvars = super().serialize()

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
        super().deserialize(valuedict)

        activequests_raw = valuedict.get("activequests", "")
        activequests = activequests_raw.split(",") if activequests_raw else []

        for questid in activequests:
            if questid:
                self._gamecontroller.questmanager.activateQuestById(questid)

        completedquests_raw = valuedict.get("completedquests", "")
        completedquests = completedquests_raw.split(",") if completedquests_raw else []

        for questid in completedquests:
            if questid:
                self._gamecontroller.questmanager.completeQuestById(questid)
