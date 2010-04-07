# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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

from fife import fife
from fife.extensions import pychan
from fife.extensions.pychan import widgets

class MainMenu(object):
	def __init__(self, world):
		self._world = world
		self._widget = pychan.loadXML('gui/mainmenu.xml')

		self._continue = self._widget.findChild(name="continue")
		self._newgame = self._widget.findChild(name="new_game")
		self._credits = self._widget.findChild(name="credits")
		self._highscores = self._widget.findChild(name="high_scores")
		self._quit = self._widget.findChild(name="quit")
		
		self._widget.position = (0,0)

		eventMap = {
			'continue': self._world.continueGame,
			'new_game': self._world.newGame,
			'credits': self._world.showCredits,
			'high_scores': self._world.showHighScores,
			'quit': self._world.quit,
		}

		self._widget.mapEvents(eventMap)		
		
		self._continueMinWidth = self._continue.min_width
		self._continueMinHeight = self._continue.min_height
		self._continueMaxWidth = self._continue.max_width
		self._continueMaxHeight = self._continue.max_height	

		
	def show(self, cont=False):
		if cont:
			self._continue.min_width = self._continueMinWidth
			self._continue.min_height = self._continueMinHeight
			self._continue.max_width = self._continueMaxWidth
			self._continue.max_height = self._continueMaxHeight
			
		else:
			self._continue.min_width = 0
			self._continue.min_height = 0
			self._continue.max_width = 0
			self._continue.max_height = 0

		self._continue.adaptLayout()
		self._widget.show()
		
	def hide(self):
		self._widget.hide()
		
class HeadsUpDisplay(object):
	def __init__(self, world):
		self._world = world
		self._widget = pychan.loadXML('gui/hud.xml')
		
		self._fpstext = self._widget.findChild(name="fps")
		self._velocitytext = self._widget.findChild(name="velocity")
		self._positiontext = self._widget.findChild(name="position")
		self._scoretext = self._widget.findChild(name="score")
		self._widget.position = (0,0)
		
	def show(self):
		self._widget.show()
		
	def hide(self):
		self._widget.hide()
		
	def setFPSText(self, text):
		self._fpstext.text = text
		
	def setPositionText(self, text):
		self._positiontext.text = text
		
	def setVelocityText(self, text):
		self._velocitytext.text = text
		
	def setScoreText(self, text):
		self._scoretext.text = text