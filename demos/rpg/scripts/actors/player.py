#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Player actor and listener implementations for the RPG demo."""

from scripts.actors.baseactor import Actor, ActorActionListener


class PlayerActionListener(ActorActionListener):
    """Action listener adapter for player-specific instance actions."""

    def __init__(self, gamecontroller, obj):
        """Initialize the player action listener with controller and object."""
        super().__init__(gamecontroller, obj)

    def onInstanceActionFinished(self, instance, action):
        """Handle finished instance actions for the player (no-op by default)."""
        super().onInstanceActionFinished(instance, action)
        if action.getId() == "walk":
            pass

    def onInstanceActionCancelled(self, instance, action):
        """Handle cancelled instance actions for the player (no-op)."""
        pass


class Player(Actor):
    """Player actor class with quest serialization helpers."""

    def __init__(self, gamecontroller, layer, playermodelname):
        super().__init__(
            gamecontroller, layer, "PLAYER", "player", playermodelname, "player", True
        )

        self._actionlistener = PlayerActionListener(self._gamecontroller, self)
        self._actionlistener.attachActionListener()

        self._quests = []

    def serialize(self):
        """Serialize player-specific state (active and completed quests).

        Returns
        -------
        dict
            A mapping of serialized player state values suitable for saving.
        """
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
        """Deserialize player-specific state and restore quests by id."""
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
