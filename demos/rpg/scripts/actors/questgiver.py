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

import sys, os, re, math, random, shutil

from fife import fife

from scripts.objects.baseobject import BaseGameObject, GameObjectTypes
from scripts.actors.baseactor import Actor

class QuestGiver(Actor):
	def __init__(self, gamecontroller, layer, typename, baseobjectname, instancename, instanceid=None, createInstance=False):
		super(QuestGiver, self).__init__(gamecontroller, layer, typename, baseobjectname, instancename, instanceid, createInstance)
		self._type = GameObjectTypes["QUESTGIVER"]
	
		self._quests = []
		
		self._activequest = None
	
	def addQuest(self, quest):
		self._quests.append(quest)
		
	def offerNextQuest(self):
		if self._activequest:
			return
	
		if len(self._quests) > 0:
			self._gamecontroller.guicontroller.showQuestDialog(self)
		
	def getNextQuest(self):
		if len(self._quests) > 0:
			return self._quests[0]
		
		return None
		
	def activateQuest(self, quest):
		self._activequest = quest
			
	def completeQuest(self):
		if self._activequest in self._quests:
			if self._activequest.checkQuestCompleted(self._gamecontroller.scene.player):
				self.say("That everything I need.  Thank you!")
				self._gamecontroller.scene.player.gold = self._gamecontroller.scene.player.gold - self._activequest.requiredgold
				
				for itemid in self._activequest.requireditems:
					self._gamecontroller.scene.player.removeItemFromInventory(itemid)
					
				self._quests.remove(self._activequest)
				self._activequest = None
			else:
				self.say("Come back when you have all the items I requested!")
		else:
			#something went wrong
			self._activequest = None
	
	def haveQuest(self):
		return len(self._quests) > 0
	
	def _getActiveQuest(self):
		return self._activequest
		
	activequest = property(_getActiveQuest)
