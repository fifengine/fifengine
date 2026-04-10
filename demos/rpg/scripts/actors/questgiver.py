#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""Quest giver actor implementations for the RPG demo."""

from scripts.actors.baseactor import Actor
from scripts.objects.baseobject import GameObjectTypes


class QuestGiver(Actor):
    """Actor that offers quests to players and manages quest lifecycle."""

    def __init__(
        self,
        gamecontroller,
        layer,
        typename,
        baseobjectname,
        instancename,
        instanceid=None,
        createInstance=False,
    ):
        super().__init__(
            gamecontroller,
            layer,
            typename,
            baseobjectname,
            instancename,
            instanceid,
            createInstance,
        )
        self._type = GameObjectTypes["QUESTGIVER"]

        self._noquest_dialog = "I've got nothing for you...  leave me alone."

    def offerNextQuest(self):
        """Show the quest dialog if there is a quest available for the player."""
        if self._gamecontroller.questmanager.getNextQuest(self.id):
            self._gamecontroller.guicontroller.showQuestDialog(self)

    def getNextQuest(self):
        """Return the next quest that will be offered by this QuestGiver.

        Returns
        -------
        Quest | None
            The next `Quest` to be offered, or `None` if there is no quest.
        """
        return self._gamecontroller.questmanager.getNextQuest(self.id)

    def activateQuest(self, quest):
        """Mark the given quest as active (called after player accepts it)."""
        self._gamecontroller.questmanager.activateQuest(quest)

    def showNoQuestDialog(self):
        """Show the default 'no quest available' dialog."""
        self.say(self._noquest_dialog)

    def completeQuest(self):
        """Check whether the active quest is complete and process completion."""
        for activequest in self._gamecontroller.questmanager.activequests:
            if activequest.ownerid == self.id:
                if activequest.checkQuestCompleted(self._gamecontroller.scene.player):
                    self.say(activequest._complete_dialog)

                    self._gamecontroller.scene.player.gold = (
                        self._gamecontroller.scene.player.gold - activequest.requiredgold
                    )

                    for itemid in activequest.requireditems:
                        self._gamecontroller.scene.player.removeItemFromInventory(itemid)

                    self._gamecontroller.questmanager.completeQuest(activequest)
                else:
                    self.say(activequest._incomplete_dialog)

    def haveQuest(self):
        """Return True if there is an active or available quest for this QuestGiver.

        Returns
        -------
        bool
            True when a quest is available or active, otherwise False.
        """
        return bool(self._gamecontroller.questmanager.getNextQuest(self.id)) or bool(
            self._getActiveQuest()
        )

    def serialize(self):
        """Serialize questgiver state to a dictionary.

        Returns
        -------
        dict
            Serialized questgiver state mapping.
        """
        lvars = super().serialize()

        lvars["noquest_dialog"] = self._noquest_dialog

        return lvars

    def deserialize(self, valuedict):
        """Deserialize questgiver state from a dictionary."""
        super().deserialize(valuedict)

        self._noquest_dialog = valuedict["noquest_dialog"]

    def _getActiveQuest(self):
        """Return the first active quest owned by this QuestGiver, or None.

        Returns
        -------
        Quest | None
            The first active `Quest` owned by this questgiver, or `None`.
        """
        for quest in self._gamecontroller.questmanager.activequests:
            if quest.ownerid == self.id:
                return quest

    activequest = property(_getActiveQuest)
