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
from scripts.objects.items import GoldStack

Actions = {'NONE':0,
		   'PICKUP':1,
		   'TALK':2,
		   'ATTACK':3,
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
		if self._dest.type == GameObjectTypes["QUESTGIVER"]:
			if self._dest.haveQuest():
				if not self._dest.activequest:
					self._dest.offerNextQuest()
				else:
					self._dest.completeQuest()
			else:
				self._dest.say("I've got nothing for you...  leave me alone.")
		else:
			self._dest.instance.say("Hello there!")
			
class PickUpItemAction(BaseAction):
	def __init__(self, actor, item):
		self._actiontype = Actions['PICKUP']
		self._actor = actor
		self._item = item
		
	def execute(self):
		self._actor.pickUpItem(self._item)
		
class EnterPortalAction(BaseAction):
	def __init__(self, actor, portal):
		self._actiontype = Actions["ENTER"]
		self._actor = actor
		self._portal = portal
		
	def execute(self):
		self._actor.enterPortal(self._portal)

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
	def __init__(self, gamecontroller, layer, typename, instancename, instanceid=None, createInstance=False):
		super(Actor, self).__init__(gamecontroller, layer, typename, instancename, instanceid, createInstance)

		self._type = GameObjectTypes["DEFAULT"]

		self._walkspeed = self._gamecontroller.settings.get("RPG", "DefaultActorWalkSpeed", 4.0)
		
		self._nextaction = None
		self._inventory = []
		self._maxinventoryitems = 20
		
		self._gold = 0
		
		self.stand()
		
	def stand(self):
		self._state = ActorStates["STAND"]
		self._instance.act('stand', self._instance.getFacingLocation())
		
	def walk(self, location):
		self._state = ActorStates["WALK"]
		self._instance.move('walk', location, self._walkspeed)
		
	def say(self, text):
		self._instance.say(text, 2500)
		
	def performNextAction(self):
		if self._nextaction:
			self._nextaction.execute()
			self._nextaction = None
			
	def pickUpItem(self, item):
		if len(self._inventory) >= self._maxinventoryitems:
			return
		else:
			if type(item) == GoldStack:
				self._gold += item.value
			else:
				self._inventory.append(item)
		
			item.onPickUp()
			
	def enterPortal(self, portal):
		if self._id == "player":
			self._gamecontroller.switchMap(portal.dest)
		else:
			self._gamecontroller.scene.removeObjectFromScene(self._id)
		
	def removeItemFromInventory(self, itemid):
		itemtoremove = None
		for item in self._inventory:
			if item.id == itemid:
				itemtoremove = item
		
		if itemtoremove:
			self._inventory.remove(itemtoremove)
		
		
	def _getState(self):
		return self._state
		
	def _setState(self, state):
		self._state = state
		
	def _getNextAction(self):
		return self._nextaction
		
	def _setNextAction(self, action):
		self._nextaction = action
	
	def _getGold(self):
		return self._gold
		
	def _setGold(self, gold):
		self._gold = gold
		
	def _getInventory(self):	
		return self._inventory
	
	state = property(_getState, _setState)
	nextaction = property(_getNextAction, _setNextAction)
	gold = property(_getGold, _setGold)
	inventory = property(_getInventory)
