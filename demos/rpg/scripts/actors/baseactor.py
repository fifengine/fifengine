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
from fife.extensions.loaders import loadMapFile

from scripts.objects.baseobject import ObjectActionListener, BaseGameObject, GameObjectTypes

Actions = {'NONE':0,
		   'PICKUP':1,
		   'TALK':2,
		   'HIT':3,
		   'OPEN':4,
		   'ENTER':5}

class BaseAction(object):
	def __init__(self):
		self._actiontype = Actions['NONE']
		
	def execute(self):
		pass
		
class TalkAction(BaseAction):
	def __init__(self, sourceobj, destobj):
		self._actiontype = Actions['TALK']
		self._source = sourceobj
		self._dest = destobj
		
	def execute(self):
		print "talking to: " + self._dest.instance.getId()
		
		if self._dest.haveQuest():
			if not self._dest.activequest:
				self._dest.offerNextQuest()
			else:
				self._dest.completeQuest()

ActorStates = {'STAND':0,
			   'WALK':1,
			   'ATTACK':2}

class ActorActionListener(ObjectActionListener):
	def __init__(self, gamecontroller, obj):
		super(ActorActionListener, self).__init__(gamecontroller, obj)

	def onInstanceActionFinished(self, instance, action):
		if action.getId() == 'walk':
			self._object.stand()
			self._object.performNextAction()

class Actor(BaseGameObject):
	def __init__(self, gamecontroller, instancename, instanceid=None, createInstance=False):
		super(Actor, self).__init__(gamecontroller, instancename, instanceid, createInstance)

		self._walkspeed = self._gamecontroller.settings.get("RPG", "DefaultActorWalkSpeed", 4.0)
		
		self._actionlistener = ActorActionListener(self._gamecontroller, self)
		
		self._nextaction = None
		
		self.stand()
		
		self._type = GameObjectTypes["NPC"]

	def stand(self):
		self._state = ActorStates["STAND"]
		self._instance.act('stand', self._instance.getFacingLocation())
		
	def walk(self, location):
		self._state = ActorStates["WALK"]
		self._instance.move('walk', location, self._walkspeed)
		
	def performNextAction(self):
		if self._nextaction:
			self._nextaction.execute()
			self._nextaction = None
		
	def _getState(self):
		return self._state
		
	def _setState(self, state):
		self._state = state
		
	def _getNextAction(self):
		return self._nextaction
		
	def _setNextAction(self, action):
		self._nextaction = action
	
	state = property(_getState, _setState)
	nextaction = property(_getNextAction, _setNextAction)

class Quest(object):
	def __init__(self, owner, questname, questtext):
		self._owner = owner
		self._name = questname
		self._text = questtext
	
	def _getOwner(self):
		return self._owner
	
	def _getName(self):
		return self._name
		
	def _setName(self, questname):
		self._name = questname
		
	def _getText(self):
		return self._text
		
	def _setText(self, questtext):
		self._text = questtext
	
	owner = property(_getOwner)
	name = property(_getName, _setName)
	text = property(_getText, _setText)

class QuestGiver(Actor):
	def __init__(self, gamecontroller, instancename, instanceid=None, createInstance=False):
		super(QuestGiver, self).__init__(gamecontroller, instancename, instanceid, createInstance)
	
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
		#@todo check to see if requirements are met
		if self._activequest in self._quests:
			print "quest completed"
			self._quests.remove(self._activequest)
			self._activequest = None
		else:
			#something went wrong
			self._activequest = None
	
	def haveQuest(self):
		return len(self._quests) > 0
	
	def _getActiveQuest(self):
		return self._activequest
		
	activequest = property(_getActiveQuest)
