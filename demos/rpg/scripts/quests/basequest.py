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

import sys, os, re, math, random, shutil, time
from datetime import datetime

from fife import fife

QuestTypes = {'DEFAULT':0,
		   'RETURN_ITEM':1}

class Quest(object):
	def __init__(self, ownerid, questid, questtitle, questtext):
		self._ownerid = ownerid
		self._questid = questid
		self._name = questtitle
		self._text = questtext
		self._complete_dialog = "That everything I need.  Thank you!"
		self._incomplete_dialog = "Come back when you have all the items I requested!"
		
	def __eq__(self, other):
		return self._questid == other.id
		
	def checkQuestCompleted(self, actor):
		pass

	def _getOwnerID(self):
		return self._ownerid
	
	def _getName(self):
		return self._name
		
	def _setName(self, questtitle):
		self._name = questtitle
		
	def _getText(self):
		return self._text
		
	def _setText(self, questtext):
		self._text = questtext

	def _getID(self):
		return self._questid
		
	ownerid = property(_getOwnerID)
	name = property(_getName, _setName)
	text = property(_getText, _setText)
	id = property(_getID)

class ReturnItemQuest(Quest):
	def __init__(self, ownerid, questid, questtitle, questtext):
		super(ReturnItemQuest, self).__init__(ownerid, questid, questtitle, questtext)

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
	
	def _getRequiredGold(self):
		return self._requiredgold
	
	def _getRequiredItems(self):
		return self._requireditems
	
	requiredgold = property(_getRequiredGold)
	requireditems = property(_getRequiredItems)
