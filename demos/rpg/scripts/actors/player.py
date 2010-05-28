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
from scripts.actors.baseactor import Actor, ActorStates
from scripts.objects.baseobject import ObjectActionListener, BaseGameObject, GameObjectTypes

class PlayerActionListener(ObjectActionListener):
	def __init__(self, gamecontroller, obj):
		super(PlayerActionListener, self).__init__(gamecontroller, obj)

	def onInstanceActionFinished(self, instance, action):
		super(PlayerActionListener, self).onInstanceActionFinished(instance, action)
		if action.getId() == 'walk':
			pass
			#self._object.completeAction()

class Player(Actor):
	def __init__(self, gamecontroller, playermodelname):
		self._type = GameObjectTypes["PLAYER"]
		super(Player, self).__init__(gamecontroller, playermodelname, "player", True)
		self._playermodelname = playermodelname
		
		self._playeractionlistener = PlayerActionListener(self._gamecontroller, self)
