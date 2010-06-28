#!/usr/bin/env python

# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################
# This is the rio de hola client for FIFE.

from fife import fife

from fife.extensions.serializers.simplexml import SimpleXMLSerializer
from scripts.quests.basequest import Quest, ReturnItemQuest, QuestTypes
from scripts.misc.serializer import Serializer

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
		questfile = self._gamecontroller.settings.get("RPG", "QuestFile", "maps/quests.xml")
		
		self._questsettings = SimpleXMLSerializer(questfile)
		
		for identifier in self._questsettings.get("QuestGivers", "list", []):
			for quest in self._questsettings.get(identifier, "questlist", []):
					questdict = self._questsettings.get(identifier, quest, {})
					if questdict['type'] == "RETURN_ITEM":
						questobj = ReturnItemQuest(identifier, quest, questdict['name'], questdict['desc'])
						for ritem in self._questsettings.get(quest+"_items", "itemlist", []):
							itemdict = self._questsettings.get(quest+"_items", ritem, {})
							if itemdict["name"] == "GOLD_COINS":
								questobj.addRequiredGold(int(itemdict['value']))
							else:
								questobj.addRequiredItem(ritem)
					else:
						questobj = Quest(identifier, quest, questdict['name'], questdict['desc'])

					if questdict.has_key("quest_incomplete_dialog"):
						questobj._incomplete_dialog = questdict['quest_incomplete_dialog']
						
					if questdict.has_key("quest_complete_dialog"):
						questobj._complete_dialog = questdict['quest_complete_dialog']

					self._gamecontroller.questmanager.addQuest(questobj)
	
	def reset(self):
		self._quests = {}
		self._activequests = []
		self._completedquests = []
		
	def addQuest(self, quest):
		if self._quests.has_key(quest.ownerid):
			if not quest in self._quests[quest.ownerid]:
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
		if self._quests.has_key(ownerid):
			for quest in self._quests[ownerid]:
				if not quest in self._activequests and not quest in self._completedquests:
					return quest

		return None
		
	def activateQuest(self, quest):
		"""
		Adds the quest to the "active quests" list.  Note that this does NOT affect
		the quest in any way.  It's just a way of keeping track of which quests
		the player has accepted.
		"""
		if not quest in self._activequests:
			self._activequests.append(quest)
		
	def completeQuest(self, quest):
		"""
		Marks the quest as completed.  Note that this does NOT modify the quest in
		any way.  This is just a way to keep track of completed quests.
		"""
		if not quest in self._completedquests:
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
