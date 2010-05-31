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

import sys, os, re, math, random, shutil, time
from datetime import datetime

from fife import fife

class Quest(object):
	def __init__(self, owner, questname, questtext):
		self._owner = owner
		self._name = questname
		self._text = questtext
		self._requireditems = []
		self._requiredgold = 0
		
	def addRequiredItem(self, itemid):
		self._requireditems.append(itemid)
		
	def addRequiredGold(self, goldcount):
		self._requiredgold += goldcount
		
	def checkQuestCompleted(self, actor):
		completed = False
		
		if self._requiredgold > 0:
			if actor.gold >= self._requiredgold:
				completed = True
				
		for item in self._requireditems:
			if item in actor.inventory:
				completed = True
				
		return completed
	
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
	
	def _getRequiredGold(self):
		return self._requiredgold
	
	def _getRequiredItems(self):
		return self._requireditems
	
	owner = property(_getOwner)
	name = property(_getName, _setName)
	text = property(_getText, _setText)
	requiredgold = property(_getRequiredGold)
	requireditems = property(_getRequiredItems)
	
