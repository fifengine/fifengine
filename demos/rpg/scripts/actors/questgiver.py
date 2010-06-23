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
		
		self._noquest_dialog = "I've got nothing for you...  leave me alone."
	
	def offerNextQuest(self):
		"""
		Brings up the quest dialog of there is a quest to be offered to the player.
		"""
		if self._gamecontroller.questmanager.getNextQuest(self.id):
			self._gamecontroller.guicontroller.showQuestDialog(self)
		
	def getNextQuest(self):
		"""
		Returns the next quest that will be offered by this QuestGiver.
		"""
		return self._gamecontroller.questmanager.getNextQuest(self.id)
		
	def activateQuest(self, quest):
		"""
		This is called after the player accepts a quest.  It marks it as active or "in progress".
		"""
		self._gamecontroller.questmanager.activateQuest(quest)
	
	def showNoQuestDialog(self):
		self.say(self._noquest_dialog)
	
	def completeQuest(self):
		"""
		Checks to see if the active quest owned by this QuestGiver is complete and 
		removes the required items or gold from the players inventory.
		"""
		for activequest in self._gamecontroller.questmanager.activequests:
			if activequest.ownerid == self.id:
				if activequest.checkQuestCompleted(self._gamecontroller.scene.player):
					self.say(activequest._complete_dialog)
			
					self._gamecontroller.scene.player.gold = self._gamecontroller.scene.player.gold - activequest.requiredgold
				
					for itemid in activequest.requireditems:
						self._gamecontroller.scene.player.removeItemFromInventory(itemid)
			
					self._gamecontroller.questmanager.completeQuest(activequest)
				else:
					self.say(activequest._incomplete_dialog)
	
	def haveQuest(self):
		"""
		Returns True if there is either an active quest or the QuestGiver has a new quest to give
		the player.  Returns False otherwise.
		"""
		return bool(self._gamecontroller.questmanager.getNextQuest(self.id)) or bool(self._getActiveQuest())

	def serialize(self):
		lvars = super(QuestGiver, self).serialize()

		lvars['noquest_dialog'] = self._noquest_dialog
	
		return lvars

	def deserialize(self, valuedict):
		super(QuestGiver, self).deserialize(valuedict)
		
		self._noquest_dialog = valuedict['noquest_dialog']
		
	
	def _getActiveQuest(self):
		"""
		Returns the first active quest in the list.  There should only be one
		active quest per questgiver anyway.
		"""
		for quest in self._gamecontroller.questmanager.activequests:
			if quest.ownerid == self.id:
				return quest
		
	activequest = property(_getActiveQuest)
