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

from fife.extensions.fife_settings import Setting
from scripts.quests.basequest import Quest, ReturnItemQuest, QuestTypes

class QuestManager(object):
	def __init__(self, gamecontroller):
		self._gamecontroller = gamecontroller

		self._questsettings = None
		
		self._quests = {}
		self._activequests = []
		self._completedquests = []
		
	def initializeQuests(self):
		questfile = self._gamecontroller.settings.get("RPG", "QuestFile", "maps/quests.xml")
		
		self._questsettings = Setting(settings_file=questfile)
		
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

					self._gamecontroller.questmanager.addQuest(questobj)
	
	def destroy(self):
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
		for quest in self._quests[ownerid]:
			if not quest in self._activequests and not quest in self._completedquests:
				return quest
				
		return None
		
	def activateQuest(self, quest):
		if not quest in self._activequests:
			self._activequests.append(quest)
		
	def completeQuest(self, quest):
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
