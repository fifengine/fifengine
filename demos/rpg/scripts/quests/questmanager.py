#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife.extensions.serializers.simplexml import SimpleXMLSerializer

from scripts.misc.serializer import Serializer
from scripts.quests.basequest import Quest, ReturnItemQuest


class QuestManager(Serializer):
    def __init__(self, gamecontroller):
        self._gamecontroller = gamecontroller

        self._questsettings = None

        self._quests = {}
        self._activequests = []
        self._completedquests = []

    def serialize(self):
        pass

    def deserialize(self, valuedict=None):
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
        self._quests = {}
        self._activequests = []
        self._completedquests = []

    def addQuest(self, quest):
        if quest.ownerid in self._quests:
            if quest not in self._quests[quest.ownerid]:
                self._quests[quest.ownerid].append(quest)
        else:
            self._quests[quest.ownerid] = [quest]

    def getQuest(self, questid):
        for owner in self._quests:
            for quest in self._quests[owner]:
                if quest.id == questid:
                    return quest

        return None

    def getNextQuest(self, ownerid):
        if ownerid in self._quests:
            for quest in self._quests[ownerid]:
                if quest not in self._activequests and quest not in self._completedquests:
                    return quest

        return None

    def activateQuest(self, quest):
        """
        Adds the quest to the "active quests" list.  Note that this does NOT affect
        the quest in any way.  It's just a way of keeping track of which quests
        the player has accepted.
        """
        if quest not in self._activequests:
            self._activequests.append(quest)

    def completeQuest(self, quest):
        """
        Marks the quest as completed.  Note that this does NOT modify the quest in
        any way.  This is just a way to keep track of completed quests.
        """
        if quest not in self._completedquests:
            self._completedquests.append(quest)

        if quest in self._activequests:
            self._activequests.remove(quest)

    def activateQuestById(self, questid):
        quest = self.getQuest(questid)
        if quest:
            self.activateQuest(quest)

    def completeQuestById(self, questid):
        quest = self.getQuest(questid)
        if quest:
            self.completeQuest(quest)

    def _getActiveQuests(self):
        return self._activequests

    def _getCompletedQuests(self):
        return self._completedquests

    def _getAllQuests(self):
        return self._quests

    activequests = property(_getActiveQuests)
    completedquests = property(_getCompletedQuests)
    quests = property(_getAllQuests)
