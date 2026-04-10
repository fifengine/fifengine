#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Quest classes for the RPG demo."""

QuestTypes = {"DEFAULT": 0, "RETURN_ITEM": 1}


class Quest:
    """Base quest representation."""

    def __init__(self, ownerid, questid, questtitle, questtext):
        """Initialize a quest with owner, id, title, and text."""
        self._ownerid = ownerid
        self._questid = questid
        self._name = questtitle
        self._text = questtext
        self._complete_dialog = "That everything I need.  Thank you!"
        self._incomplete_dialog = "Come back when you have all the items I requested!"

    def __eq__(self, other):
        """Return True if this quest equals another by id.

        Returns
        -------
        bool
            True if the quest identifiers are equal, otherwise False.
        """
        return self._questid == other.id

    def checkQuestCompleted(self, actor):
        """Return whether the quest is completed for the given actor.

        Returns
        -------
        bool
            True if the quest is completed for `actor`, otherwise False.
        """
        pass

    def _getOwnerID(self):
        """Return the owner id of the quest.

        Returns
        -------
        str
            The owner identifier for the quest.
        """
        return self._ownerid

    def _getName(self):
        """Return the quest name.

        Returns
        -------
        str
            The quest's display name.
        """
        return self._name

    def _setName(self, questtitle):
        """Set the quest name."""
        self._name = questtitle

    def _getText(self):
        """Return the quest text/description.

        Returns
        -------
        str
            The quest description text.
        """
        return self._text

    def _setText(self, questtext):
        """Set the quest text/description."""
        self._text = questtext

    def _getID(self):
        """Return the quest identifier.

        Returns
        -------
        str
            The unique quest identifier.
        """
        return self._questid

    ownerid = property(_getOwnerID)
    name = property(_getName, _setName)
    text = property(_getText, _setText)
    id = property(_getID)


class ReturnItemQuest(Quest):
    """Quest that completes when required items or gold are provided."""

    def __init__(self, ownerid, questid, questtitle, questtext):
        """Initialize a ReturnItemQuest with required items and gold counters."""
        super().__init__(ownerid, questid, questtitle, questtext)

        self._requireditems = []
        self._requiredgold = 0

    def addRequiredItem(self, itemid):
        """Add a required item id for the quest."""
        self._requireditems.append(itemid)

    def addRequiredGold(self, goldcount):
        """Add required gold amount for the quest."""
        self._requiredgold += goldcount

    def checkQuestCompleted(self, actor):
        """Return True if the actor meets required items or gold.

        Returns
        -------
        bool
            True if the actor satisfies the quest requirements, otherwise False.
        """
        completed = False

        if self._requiredgold > 0:
            if actor.gold >= self._requiredgold:
                completed = True

        for item in self._requireditems:
            if item in actor.inventory:
                completed = True

        return completed

    def _getRequiredGold(self):
        """Return the required gold amount.

        Returns
        -------
        int
            The amount of gold required to complete the quest.
        """
        return self._requiredgold

    def _getRequiredItems(self):
        """Return the list of required item ids.

        Returns
        -------
        list
            A list of item identifier strings required for the quest.
        """
        return self._requireditems

    requiredgold = property(_getRequiredGold)
    requireditems = property(_getRequiredItems)
