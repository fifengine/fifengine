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
from scripts.actors.baseactor import Actor, ActorStates, ActorActionListener
from scripts.objects.baseobject import BaseGameObject, GameObjectTypes

class PlayerActionListener(ActorActionListener):
	def __init__(self, gamecontroller, obj):
		super(PlayerActionListener, self).__init__(gamecontroller, obj)

	def onInstanceActionFinished(self, instance, action):
		super(PlayerActionListener, self).onInstanceActionFinished(instance, action)
		if action.getId() == 'walk':
			pass

class Player(Actor):
	def __init__(self, gamecontroller, layer, playermodelname):
		super(Player, self).__init__(gamecontroller, layer, "PLAYER", "player", playermodelname, "player", True)
	
		self._actionlistener = PlayerActionListener(self._gamecontroller, self)
		self._actionlistener.attachActionListener()
		
		self._quests = []
		
	def serialize(self):
		lvars = super(Player, self).serialize()

		activequests = ""

		for quest in self._gamecontroller.questmanager.activequests:
			if activequests == "":
				activequests = quest.id
			else:
				activequests = activequests + "," + quest.id

		lvars['activequests'] = activequests
		
		completedquests = ""

		for quest in self._gamecontroller.questmanager.completedquests:
			if completedquests == "":
				completedquests = quest.id
			else:
				completedquests = completedquests + "," + quest.id

		lvars['completedquests'] = completedquests
		
		return lvars

	def deserialize(self, valuedict):
		super(Player, self).deserialize(valuedict)
		
		activequests = valuedict['activequests'].split(",")
		
		for questid in activequests:
			self._gamecontroller.questmanager.activateQuestById(questid)
			
		completedquests = valuedict['completedquests'].split(",")
		
		for questid in completedquests:
			self._gamecontroller.questmanager.completeQuestById(questid)
