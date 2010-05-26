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

from scripts.objects.baseobject import ObjectActionListener, BaseGameObject

ActorStates = {'STAND':0,
			   'WALK':1,
			   'ATTACK':2}

class ActorActionListener(ObjectActionListener):
	def __init__(self, gamecontroller, obj):
		super(ActorActionListener, self).__init__(gamecontroller, obj)

	def onInstanceActionFinished(self, instance, action):
		if action.getId() == 'walk':
			self._object.stand()

class Actor(BaseGameObject):
	def __init__(self, gamecontroller, instancename, instanceid=None, createInstance=False):
		super(Actor, self).__init__(gamecontroller, instancename, instanceid, createInstance)

		self._walkspeed = self._gamecontroller.settings.get("RPG", "DefaultActorWalkSpeed", 4.0)
		
		self._actionlistener = ActorActionListener(self._gamecontroller, self)
		
		self.stand()

	def stand(self):
		self._state = ActorStates["STAND"]
		self._instance.act('stand', self._instance.getFacingLocation())
		
	def walk(self, location):
		self._state = ActorStates["WALK"]
		self._instance.move('walk', location, self._walkspeed)
		
	def _getState(self):
		return self._state
		
	def _setState(self, state):
		self._state = state
	
	state = property(_getState, _setState)
