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
	
	def addQuest(self, quest):
		pass
		
	def offerNextQuest(self):
		if self._gamecontroller.questmanager.getNextQuest(self.id):
			self._gamecontroller.guicontroller.showQuestDialog(self)
		
	def getNextQuest(self):
		return self._gamecontroller.questmanager.getNextQuest(self.id)
		
	def activateQuest(self, quest):
		self._gamecontroller.questmanager.activateQuest(quest)
			
	def completeQuest(self):
		for activequest in self._gamecontroller.questmanager.activequests:
			if activequest.ownerid == self.id:
				if activequest.checkQuestCompleted(self._gamecontroller.scene.player):
					self.say("That everything I need.  Thank you!")
			
					self._gamecontroller.scene.player.gold = self._gamecontroller.scene.player.gold - activequest.requiredgold
				
					for itemid in activequest.requireditems:
						self._gamecontroller.scene.player.removeItemFromInventory(itemid)
			
					self._gamecontroller.questmanager.completeQuest(activequest)
				else:
					self.say("Come back when you have all the items I requested!")
	
	def haveQuest(self):
		return bool(self._gamecontroller.questmanager.getNextQuest(self.id)) or bool(self._getActiveQuest())
	
	def _getActiveQuest(self):
		"""
		Returns the first active quest in the list.  There should only be one
		active quest per questgiver anyway.
		"""
		for quest in self._gamecontroller.questmanager.activequests:
			if quest.ownerid == self.id:
				return quest
		
	activequest = property(_getActiveQuest)
