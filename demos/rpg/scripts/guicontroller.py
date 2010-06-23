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
from fife.extensions import pychan
from fife.extensions.pychan import widgets

class Window(object):
	def __init__(self, gamecontroller):
		self._guicontroller = gamecontroller.guicontroller
		self._gamecontroller = gamecontroller
		self._settings = gamecontroller.settings
		
		self._widget = None
		
	def _getWidget(self):
		return self._widget
		
	widget = property(_getWidget)

class MainMenu(Window):
	def __init__(self, gamecontroller):
		super(MainMenu, self).__init__(gamecontroller)
		self._widget = pychan.loadXML('gui/mainmenu.xml')

		self._newgame = self._widget.findChild(name="new_game")
		self._credits = self._widget.findChild(name="credits")
		self._quit = self._widget.findChild(name="quit")
		
		self._widget.position = (0,0)

		eventMap = {
			'new_game': self._gamecontroller.newGame,
			'settings': self._settings.onOptionsPress,
			'credits': self._guicontroller.showCredits,
			'quit': self._gamecontroller.quit,
		}

		self._widget.mapEvents(eventMap)
	
class Credits(Window):
	def __init__(self, gamecontroller):
		super(Credits, self).__init__(gamecontroller)
		self._widget = pychan.loadXML('gui/credits.xml')

		eventMap = {
			'close': self._guicontroller.hideCredits,
		}

		self._widget.mapEvents(eventMap)
	
class QuestDialog(Window):
	def __init__(self, guicontroller, questgiver):
		super(QuestDialog, self).__init__(guicontroller)
		self._widget = pychan.loadXML('gui/quest.xml')
		self._questgiver = questgiver
		self._quest = questgiver.getNextQuest()
		
		self._questname = self._widget.findChild(name="questname")
		self._questname.text = unicode(self._quest.name)
		
		self._questtext = self._widget.findChild(name="questtext")
		self._questtext.text = unicode(self._quest.text)

		eventMap = {
			'accept': self.questAccepted,
			'decline': self._widget.hide,
		}
		
		self._widget.mapEvents(eventMap)
		
	def questAccepted(self):
		self._guicontroller._gamecontroller.logger.log_debug("Quest [" + self._quest.name + "] has been accepted")
		self._questgiver.activateQuest(self._quest)
		self._widget.hide()
	
class GUIController(object):
	def __init__(self, gamecontroller):
		self._gamecontroller = gamecontroller
		self._engine = gamecontroller.engine
		self._settings = gamecontroller.settings
		
		self._mainmenu = None
		self._credits = None
	
	def showMainMenu(self):
		if self._mainmenu:
			self._mainmenu.widget.show()
		else:
			#load and show the main menu
			self._mainmenu = MainMenu(self._gamecontroller)
			self._mainmenu.widget.show()
		
	def hideMainMenu(self):
		if self._mainmenu:
			self._mainmenu.widget.hide()
			self._mainmenu = None
			
	def showCredits(self):
		if self._credits:
			self._credits.widget.show()
		else:
			self._credits = Credits(self._gamecontroller)
			self._credits.widget.show()
		
	def hideCredits(self):
		if self._credits:
			self._credits.widget.hide()
			self._credits = None
			
	def showQuestDialog(self, questgiver):
		questdlg = QuestDialog(self._gamecontroller, questgiver)
		questdlg.widget.show()
