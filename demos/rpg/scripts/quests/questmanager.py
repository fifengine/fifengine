#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Quest manager utilities for the RPG demo."""

from fife.extensions.serializers.simplexml import SimpleXMLSerializer

from scripts.misc.serializer import Serializer
from scripts.quests.basequest import Quest, ReturnItemQuest


class QuestManager(Serializer):
    """Manage quests for the RPG demo."""

    def __init__(self, gamecontroller):
        """Initialize the QuestManager with a GameController reference."""
        self._gamecontroller = gamecontroller

        self._questsettings = None

        self._quests = {}
        self._activequests = []
        self._completedquests = []

    def serialize(self):
        """Serialize quest data (not implemented)."""
        pass

    def deserialize(self, valuedict=None):
        """Deserialize quests from configuration and populate the manager."""
        questfile = self._gamecontroller.settings.get(
            "RPG", "QuestFile", "maps/quests.xml"
        )

        self._questsettings = SimpleXMLSerializer(questfile)

        for identifier in self._questsettings.get("QuestGivers", "list", []):
            for quest in self._questsettings.get(identifier, "questlist", []):
                questdict = self._questsettings.get(identifier, quest, {})
                if questdict["type"] == "RETURN_ITEM":
                    questobj = ReturnItemQuest(
                        identifier, quest, questdict["name"], questdict["desc"]
                    )
                    for ritem in self._questsettings.get(
                        quest + "_items", "itemlist", []
                    ):
                        itemdict = self._questsettings.get(quest + "_items", ritem, {})
                        if itemdict["name"] == "GOLD_COINS":
                            questobj.addRequiredGold(int(itemdict["value"]))
                        else:
                            questobj.addRequiredItem(ritem)
                else:
                    questobj = Quest(
                        identifier, quest, questdict["name"], questdict["desc"]
                    )

                if "quest_incomplete_dialog" in questdict:
                    questobj._incomplete_dialog = questdict["quest_incomplete_dialog"]

                if "quest_complete_dialog" in questdict:
                    questobj._complete_dialog = questdict["quest_complete_dialog"]

                self._gamecontroller.questmanager.addQuest(questobj)

    def reset(self):
        """Reset all stored quests and state."""
        self._quests = {}
        self._activequests = []
        self._completedquests = []

    def addQuest(self, quest):
        """Add a quest to the manager, grouping by owner id."""
        if quest.ownerid in self._quests:
            if quest not in self._quests[quest.ownerid]:
                self._quests[quest.ownerid].append(quest)
        else:
            self._quests[quest.ownerid] = [quest]

    def getQuest(self, questid):
        """Return the quest with the given id, or None if not found.

        Returns
        -------
        Quest or None
            The matching quest object if found, otherwise ``None``.
        """
        for owner in self._quests:
            for quest in self._quests[owner]:
                if quest.id == questid:
                    return quest

        return None

    def getNextQuest(self, ownerid):
        """Return the next available quest for the given owner id.

        Returns
        -------
        Quest or None
            The next unstarted quest for the owner, or ``None`` if none are
            available.
        """
        if ownerid in self._quests:
            for quest in self._quests[ownerid]:
                if quest not in self._activequests and quest not in self._completedquests:
                    return quest

        return None

    def activateQuest(self, quest):
        """Activate a quest by adding it to the active quests list."""
        if quest not in self._activequests:
            self._activequests.append(quest)

    def completeQuest(self, quest):
        """Mark a quest as completed and remove it from active quests."""
        if quest not in self._completedquests:
            self._completedquests.append(quest)

        if quest in self._activequests:
            self._activequests.remove(quest)

    def activateQuestById(self, questid):
        """Activate the quest matching the given id, if present."""
        quest = self.getQuest(questid)
        if quest:
            self.activateQuest(quest)

    def completeQuestById(self, questid):
        """Mark the quest matching the given id as completed, if present."""
        quest = self.getQuest(questid)
        if quest:
            self.completeQuest(quest)

    def _getActiveQuests(self):
        """Return the list of active quests.

        Returns
        -------
        list
            The list of currently active `Quest` objects.
        """
        return self._activequests

    def _getCompletedQuests(self):
        """Return the list of completed quests.

        Returns
        -------
        list
            The list of completed `Quest` objects.
        """
        return self._completedquests

    def _getAllQuests(self):
        """Return the mapping of all quests grouped by owner.

        Returns
        -------
        dict
            A mapping from owner id to lists of `Quest` objects.
        """
        return self._quests

    activequests = property(_getActiveQuests)
    completedquests = property(_getCompletedQuests)
    quests = property(_getAllQuests)
