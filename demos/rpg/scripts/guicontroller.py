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
from fife.extensions import pychan
from fife.extensions.pychan import widgets

class MainMenu(object):
	def __init__(self, guicontroller, gamecontroller, settings):
		self._guicontroller = guicontroller
		self._gamecontroller = gamecontroller
		self._settings = settings
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
		
	def _getWidget(self):
		return self._widget
		
	widget = property(_getWidget)
	
class Credits(object):
	def __init__(self, guicontroller):
		self._guicontroller = guicontroller
		self._widget = pychan.loadXML('gui/credits.xml')

		eventMap = {
			'close': self._guicontroller.hideCredits,
		}

		self._widget.mapEvents(eventMap)
	
	def _getWidget(self):
		return self._widget
		
	widget = property(_getWidget)		
		
class GUIController(object):
	def __init__(self, gamecontroller, engine, settings):
		self._gamecontroller = gamecontroller
		self._engine = engine
		self._settings = settings
		
		self._mainmenu = None
		self._credits = None
	
	def showMainMenu(self):
		if self._mainmenu:
			self._mainmenu.widget.show()
		else:
			#load and show the main menu
			self._mainmenu = MainMenu(self, self._gamecontroller, self._settings)
			self._mainmenu.widget.show()
		
	def hideMainMenu(self):
		if self._mainmenu:
			self._mainmenu.widget.hide()
			self._mainmenu = None
			
	def showCredits(self):
		if self._credits:
			self._credits.widget.show()
		else:
			self._credits = Credits(self)
			self._credits.widget.show()
		
	def hideCredits(self):
		if self._credits:
			self._credits.widget.hide()
			self._credits = None
