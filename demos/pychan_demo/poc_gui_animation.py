# -*- coding: utf-8 -*-
# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
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

""" proof-of-concept pychan demo app to test gui animations """

from pychan_demo import PyChanExample
from fife.extensions import pychan

from fife.extensions.pychan.tools import callbackWithArguments as cbwa
from fife.extensions.fife_timer import Timer

import time
import random

ACTION_MOVE = 1
ACTION_RESIZE = 2
ACTION_COLOR = 3
ACTIONS = [ACTION_MOVE, ACTION_RESIZE, ACTION_COLOR]

DEFAULT_DELAY = 10

class PocAnimations(PyChanExample):
	""" a small app (^^) to show how gui animations ~could~ be
		made by using B{fife.TimeEvent}s	
	"""
	def __init__(self):
		super(PocAnimations,self).__init__('gui/poc_guianimation.xml')
		
		self._move_timer = None
		self._resize_timer = None
		self._color_timer = None
		
	def start(self):
		""" 
		load XML file and setup callbacks
		"""
		self.widget = pychan.loadXML(self.xmlFile)
		
		self.widget.mapEvents({
			'closeButton'	:	self.stop,
			
			'example_move'	:	cbwa(self._start_anim, type=ACTION_MOVE),
			'example_color'	:	cbwa(self._start_anim, type=ACTION_COLOR),
			'example_resize':	cbwa(self._start_anim, type=ACTION_RESIZE),
			'example_all'	: 	self._anim_all,			
			'delay'			:	self._set_delay_display,
		})
		
		self.move_example_widget = self.widget.findChild(name="move")
		self.mew = self.move_example_widget

		self.resize_example_widget = self.widget.findChild(name="resize")
		self.rew = self.resize_example_widget		

		self.color_example_widget = self.widget.findChild(name="color")
		self.cew = self.color_example_widget
		
		self.delay_slider = self.widget.findChild(name="delay")
		self.delay_slider.setValue(float(DEFAULT_DELAY))
		
		self.delay_display = self.widget.findChild(name="delay_label")
		self.delay_display.text = unicode(str(DEFAULT_DELAY))
		
		self.little_matrix = []
		for x in range(1,4):
			for y in range(1,4):
				name = "color_%s_%s" % (x, y)
				widget = self.widget.findChild(name=name)
				self.little_matrix.append(widget)
		
		self.widget.adaptLayout(True)
		self.widget.show()
		
	def _set_delay_display(self):
		""" set delay display according to slider value """
		value = self.delay_slider.getValue()
		self.delay_display.text = unicode(str(int(value)))		
		
	def _anim_all(self):
		""" fire all animations """
		for action in ACTIONS:
			self._start_anim(type=action)
		
	def _start_anim(self, type=None):
		""" start the animation of the given type """
		self._reset_anim(type)
		kwargs = {
			'delay' : int(self.delay_slider.getValue()),
			'callback' : None,
			'repeat' : 0,
		}
		if type == ACTION_MOVE:
			kwargs['callback'] = self._move
			self._move_timer = Timer(**kwargs)
			self._move_timer.start()
		elif type == ACTION_RESIZE:
			kwargs['callback'] = self._resize
			self._resize_timer = Timer(**kwargs)
			self._resize_timer.start()
		elif type == ACTION_COLOR:
			kwargs['callback'] = self._color
			self._color_timer = Timer(**kwargs)
			self._color_timer.start()
			
	def _reset_anim(self, type=None):
		""" undo changes made by the animation (but leave alone disco matrix ^^) """
		if type == ACTION_MOVE:
			if self._move_timer:
				self._move_timer.stop()
			self.mew.position = 0, 0
		elif type == ACTION_RESIZE:
			if self._resize_timer:
				self._resize_timer.stop()
			SIZE = 100, 100
			self.rew.size = SIZE
			self.rew.min_size = SIZE
			self.rew.max_size = SIZE
			self.widget.findChild(name="resize_wrapper").adaptLayout()
		elif type == ACTION_COLOR:
			if self._color_timer:
				self._color_timer.stop()
			COLOR = 255, 255, 255, 100
			self.cew.base_color = COLOR
							
	def _move(self):
		""" move the mew widget """
		position = list(self.mew.position)
		if position[0] < 100:
			position[0] += 1
			self.mew.position = position
		else:
			self._reset_anim(ACTION_MOVE)	
		
	def _resize(self):
		""" resize the rew widget """
		size = list(self.rew.size)
		if size[0] > 0:
			size[0] -= 1
			size[1] -= 1
			self.rew.size = size
			self.rew.min_size = size
			self.rew.max_size = size
		else:
			self._reset_anim(ACTION_RESIZE)
			
	def _color(self):
		""" tint the cew widgets """
		color = self.cew.base_color
		red = color.r
		if red > 1:
			red -= 1
			self.cew.base_color = (red, 255, 255, 100)

			# disco!
			for widget in self.little_matrix:
				color = tuple(random.randint(1,255) for i in range(1,5))
				widget.background_color = color
		else:
			self._reset_anim(ACTION_COLOR)
			
