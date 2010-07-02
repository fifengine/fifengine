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

import sys, os, re, math, random, shutil

from fife import fife
from fife.extensions.loaders import loadMapFile

from scripts.objects.baseobject import ObjectActionListener, BaseGameObject, GameObjectTypes
from scripts.objects.items import GoldStack
from scripts.misc.serializer import Serializer

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
				self._dest.showNoQuestDialog()
		else:
			self._dest.say("Hello there!")
			
class AttackAction(BaseAction):	
	def __init__(self, attacker, defender):
		self._actiontype = Actions['ATTACK']
		self._attacker = attacker
		self._defender = defender
		
	def execute(self):
		if self._defender.type == GameObjectTypes["ENEMY"]:
			self._defender.say("Ouch")
			
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

class ActorAttributes(Serializer):
	def __init__(self, strength=0, dexterity=0, intelligence=0, health=0, walkspeed=0):
		self._str = strength
		self._dex = dexterity
		self._int = intelligence
		self._hp = health
		self._walkspeed = walkspeed
		
	def serialize(self):
		lvars = {}
		
		lvars['str'] = self._str
		lvars['dex'] = self._dex
		lvars['int'] = self._int
		lvars['hp'] = self._hp
		lvars['walk_speed'] = self._walkspeed
		
		return lvars
		
	def deserialize(self, valuedict):
		if valuedict.has_key("str"):
			self._str = int(valuedict['str'])
		if valuedict.has_key("dex"):
			self._dex = int(valuedict['dex'])
		if valuedict.has_key("int"):
			self._int = int(valuedict['int'])
		if valuedict.has_key("hp"):
			self._hp = int(valuedict['hp'])
		if valuedict.has_key("walk_speed"):
			self._walkspeed = float(valuedict['walk_speed'])
			
	def _getStrength(self):
		return self._str
		
	def _setStrength(self, strength):
		self._str = strength
	
	def _getDexterity(self):
		return self._dexterity
		
	def _setDexterity(self, dexterity):
		self._dexterity = dexterity
		
	def _getIntelligence(self):
		return self._int
		
	def _setIntelligence(self, intelligence):
		self._int = intelligence
		
	def _getHealth(self):
		return self._hp
	
	def _setHealth(self, health):
		self._hp = health
	
	def _getWalkSpeed(self):
		return self._walkspeed
		
	def _setWalkSpeed(self, walkspeed):
		self._walkspeed = walkspeed
	
	
	strength = property(_getStrength, _setStrength)
	dexterity = property(_getDexterity, _setDexterity)
	intelligence = property(_getIntelligence, _setIntelligence)
	health = property(_getHealth, _setHealth)
	walkspeed = property(_getWalkSpeed, _setWalkSpeed)
			

class Actor(BaseGameObject):
	def __init__(self, gamecontroller, layer, typename, baseobjectname, instancename, instanceid=None, createInstance=False):
		super(Actor, self).__init__(gamecontroller, layer, typename, baseobjectname, instancename, instanceid, createInstance)

		self._nextaction = None
		self._inventory = []
		self._maxinventoryitems = 20

		self._gold = 0
		self._attributes = ActorAttributes()

		#set the default walkspeed
		self._attributes.walkspeed = self._gamecontroller.settings.get("RPG", "DefaultActorWalkSpeed", 4.0)
	
		self.stand()
		
	def stand(self):
		self._state = ActorStates["STAND"]
		self._instance.act('stand', self._instance.getFacingLocation())
		
	def walk(self, location):
		self._state = ActorStates["WALK"]
		self._instance.move('walk', location, self._attributes.walkspeed)
		
	def say(self, text):
		self._instance.say(text, 2500)
		
	def performNextAction(self):
		if self._nextaction:
			self._nextaction.execute()
			self._nextaction = None
			
	def pickUpItem(self, item):
		if self.addItemToInventory(item):
			item.onPickUp()		
		else:
			#could do something cool like throw the item back on the ground
			pass
			
	def enterPortal(self, portal):
		if self._id == "player":
			self._gamecontroller.switchMap(portal.dest)
		else:
			self._gamecontroller.scene.removeObjectFromScene(self._id)

	def addItemToInventory(self, item):
		if len(self._inventory) >= self._maxinventoryitems:
			return False
		else:
			if type(item) == GoldStack:
				self._gold += item.value
			else:
				self._inventory.append(item)
				
		return True
		
	def removeItemFromInventory(self, itemid):
		itemtoremove = None
		for item in self._inventory:
			if item.id == itemid:
				itemtoremove = item
		
		if itemtoremove:
			self._inventory.remove(itemtoremove)

	def serialize(self):
		lvars = super(Actor, self).serialize()

		lvars['gold'] = self._gold
		
		att_vars = self._attributes.serialize()
		for key, value in att_vars.items():
			lvars[key] = value
		
		return lvars

	def deserialize(self, valuedict):
		super(Actor, self).deserialize(valuedict)
		
		if valuedict.has_key("gold"):
			self._gold = int(valuedict['gold'])
		else:
			self._gold = 0
			
		self._attributes.deserialize(valuedict)
		
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
	
	def _getAttributes(self):
		return self._attributes
	
	state = property(_getState, _setState)
	nextaction = property(_getNextAction, _setNextAction)
	gold = property(_getGold, _setGold)
	inventory = property(_getInventory)
	attributes = property(_getAttributes)
