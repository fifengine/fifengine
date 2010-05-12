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

from fife import fife
from fife.extensions import pychan
from fife.extensions.pychan import widgets

from xml.sax.saxutils import XMLGenerator
from xml.sax.xmlreader import AttributesNSImpl

class MainMenu(object):
	def __init__(self, world, setting):
		self._world = world
		self._setting = setting
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
			'settings': self._setting.onOptionsPress,
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
		
	def isVisible(self):
		return self._widget.isVisible()
		
	
class HeadsUpDisplay(object):
	def __init__(self, world):
		self._world = world
		self._widget = pychan.loadXML('gui/hud.xml')
		
		self._fpstext = self._widget.findChild(name="fps")
		self._velocitytext = self._widget.findChild(name="velocity")
		self._positiontext = self._widget.findChild(name="position")
		self._scoretext = self._widget.findChild(name="score")
		self._livestext = self._widget.findChild(name="lives")	
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
		
	def setLivesText(self, text):
		self._livestext.text = text
		
class GameOverDisplay(object):
	def __init__(self):
		self._widget = pychan.loadXML('gui/gameover.xml')
		
	def show(self):
		self._widget.show()
		
	def hide(self):
		self._widget.hide()
		
class WinnerDisplay(object):
	def __init__(self):
		self._widget = pychan.loadXML('gui/winner.xml')
		
	def show(self):
		self._widget.show()
		
	def hide(self):
		self._widget.hide()
		
class HighScore(object):
	def __init__(self, name, score):
		self._name = name
		self._score = int(score)

class HighScores(object):
	"""
	Handles all the high scores.  It saves and loads the high score file.
	"""
	def __init__(self, world):
		self._world = world
		self.load()
		
	def load(self):
		self._widget = pychan.loadXML('gui/highscores.xml')
		self._scores = list()
		
		for i in range(1,11):
			place = self._widget.findChild(name=str(i))
			name = self._widget.findChild(name="%iname" % i)
			score = self._widget.findChild(name="%iscore" % i)
			highscore = HighScore(name.text, int(score.text))
			self._scores.append(highscore)
		
		eventMap = {
			'close': self.hide,
		}

		self._widget.mapEvents(eventMap)
		
	def isHighScore(self, score):
		for highscore in self._scores:
			if score > highscore._score:
				return True
				
		return False
		
	def addHighScore(self, score):
		if not self.isHighScore(score._score):
			return
		
		i = 0
		element = 0
		for highscore in self._scores:
			if score._score > highscore._score:
				element = i
				break
			i += 1
				
		self._scores.insert(element, score)
		#remove last element
		self._scores.pop()
		
		self.saveHighScores()
		self.load()

	def startElement(self, name, attrs):
		self._file.write(self._indent_level)
		self._xmlout.startElementNS((None, name), name, attrs)
		self._file.write('\n')
		self._indent_level = self._indent_level + '\t'

	def endElement(self, name):
		self._indent_level = self._indent_level[0:(len(self._indent_level) - 1)]
		self._file.write(self._indent_level)
		self._xmlout.endElementNS((None, name), name)
		self._file.write('\n')
		
	def saveHighScores(self):
		self._file = open('gui/highscores.xml', 'w')
		self._xmlout = XMLGenerator(self._file, 'ascii')
		self._xmlout.startDocument()

		self._indent_level = ''

		#<Container name="HighScores" position="0,0" size="1024,768" border_size="0" base_color="0,0,0" opaque="0">
	
		attr_values = {
			(None, 'name'): 'HighScores',
			(None, 'position'): '0,0',
			(None, 'size'): '1024,768',
			(None, 'border_size'): '0',
			(None, 'base_color'): '0,0,0',
			(None, 'opaque'): '0',
		}
		attr_names = {
			(None, 'name'): 'name',
			(None, 'position'): 'position',
			(None, 'size'): 'size',
			(None, 'border_size'): 'border_size',
			(None, 'base_color'): 'base_color',
			(None, 'opaque'): 'opaque',
		}
		
		container_attrs = AttributesNSImpl(attr_values, attr_names)
		self.startElement('Container', container_attrs)

		#<VBox opaque='1' base_color="188, 0, 0" position="412,200" name="high_score">
		
		attr_values = {
			(None, 'opaque'): '1',
			(None, 'base_color'): '188,0,0',
			(None, 'position'): '412,200',
			(None, 'name'): 'high_score',
		}
		
		attr_names = {
			(None, 'opaque'): 'opaque',
			(None, 'base_color'): 'base_color',
			(None, 'position'): 'position',
			(None, 'name'): 'name',
		}
		
		vbox_attrs = AttributesNSImpl(attr_values, attr_names)
		self.startElement('VBox', vbox_attrs)
		
		#<Label name="high_scores" text="High Scores" min_size="100,0" border_size="0"/>
		
		self.startElement('HBox', AttributesNSImpl({}, {}))
		attr_values = {
			(None, 'name'): 'high_scores',
			(None, 'text'): 'High Scores',
			(None, 'min_size'): '100,0',
			(None, 'border_size'): '0',
		}
		
		attr_names = {
			(None, 'name'): 'name',
			(None, 'text'): 'text',
			(None, 'min_size'): 'min_size',
			(None, 'border_size'): 'border_size',
		}
		self.startElement('Label', AttributesNSImpl(attr_values, attr_names))
		self.endElement('Label')
		
		self.endElement('HBox')
		
		i = 1
		for score in self._scores:
			self.startElement('HBox', AttributesNSImpl({}, {}))
			
			plc_attr_values = {
				(None, 'name'): str(i),
				(None, 'text'): str(i),
				(None, 'min_size'): '20,0',
				(None, 'border_size'): '5',
			}
			
			plc_attr_names = {
				(None, 'name'): 'name',
				(None, 'text'): 'text',
				(None, 'min_size'): 'min_size',
				(None, 'border_size'): 'border_size',
			}
			
			name_attr_values = {
				(None, 'name'): str(i) + 'name',
				(None, 'text'): score._name,
				(None, 'min_size'): '50,0',
				(None, 'border_size'): '5',
			}
						
			name_attr_names = {
				(None, 'name'): 'name',
				(None, 'text'): 'text',
				(None, 'min_size'): 'min_size',
				(None, 'border_size'): 'border_size',
			}
			
			score_attr_values = {
				(None, 'name'): str(i) + 'score',
				(None, 'text'): str(score._score),
				(None, 'min_size'): '150,0',
				(None, 'border_size'): '5',
			}
			
			score_attr_names = {
				(None, 'name'): 'name',
				(None, 'text'): 'text',
				(None, 'min_size'): 'min_size',
				(None, 'border_size'): 'border_size',
			}
			
			self.startElement('Label', AttributesNSImpl(plc_attr_values, plc_attr_names))
			self.endElement('Label')
			self.startElement('Label', AttributesNSImpl(name_attr_values, name_attr_names))
			self.endElement('Label')
			self.startElement('Label', AttributesNSImpl(score_attr_values, score_attr_names))
			self.endElement('Label')
			
			
			self.endElement('HBox')
			i += 1

	
		#<Button name="close" text="Close" min_size="100,0" border_size="0"/>
	
		attr_values = {
			(None, 'name'): 'close',
			(None, 'text'): 'Close',
			(None, 'min_size'): '100,0',
			(None, 'border_size'): '0',
		}
		
		attr_names = {
			(None, 'name'): 'name',
			(None, 'text'): 'text',
			(None, 'min_size'): 'min_size',
			(None, 'border_size'): 'border_size',
		}
		
		self.startElement('Button', AttributesNSImpl(attr_values, attr_names))
		self.endElement('Button')
	
		self.endElement('VBox')
		self.endElement('Container')	
		
		self._xmlout.endDocument()
		self._file.close()
		
	def show(self):
		self._widget.show()
		
	def hide(self):
		self.saveHighScores()
		self._widget.hide()

	def isVisible(self):
		return self._widget.isVisible()
		
class CreditsDisplay(object):
	def __init__(self, world):
		self._world = world
		self._widget = pychan.loadXML('gui/credits.xml')

		eventMap = {
			'close': self.hide,
		}

		self._widget.mapEvents(eventMap)
		
	def show(self):
		self._widget.show()
		
	def hide(self):
		self._widget.hide()
