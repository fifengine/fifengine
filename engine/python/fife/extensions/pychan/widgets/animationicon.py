# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2013 by the FIFE team
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
from fife import fifechan

from fife.extensions.pychan.internal import get_manager
from fife.extensions.pychan.attrs import Attr, BoolAttr
from fife.extensions.serializers.xmlanimation import loadXMLAnimation

from widget import Widget


class AnimationIcon(Widget):
	"""
	An animation icon.

	New Attributes
	==============

	  - animation: String : The source location of the Animation xml.
	  - scale: Boolean: True if the image should be scaled to widget size, false otherwise.
	  - tile: Boolean: True if the image should be tiled to widget size, false otherwise.
	  - opaque: Boolean: True if the background of the icon should be drawn, false otherwise.
	"""
	ATTRIBUTES = Widget.ATTRIBUTES + [ Attr('animation'),
									   BoolAttr('scale'),
									   BoolAttr('tile'),
									   BoolAttr('opaque'),
									   BoolAttr('repeating'),
									   BoolAttr('play')
									 ]
	DEFAULT_MARGINS = 0, 0
	DEFAULT_PADDING = 0

	DEFAULT_SCALE = False
	DEFAULT_TILE = False
	DEFAULT_OPAQUE = False
	DEFAULT_REPEATING = True
	DEFAULT_PLAY = True
	
	def __init__(self, 
				 parent = None,
				 name = None,
				 size = None,
				 min_size = None,
				 max_size = None,
				 fixed_size = None,
				 margins = None,
				 padding = None,
				 helptext = None,
				 position = None,
				 style = None,
				 hexpand = None,
				 vexpand = None,
				 font = None,
				 base_color = None,
				 background_color = None,
				 foreground_color = None,
				 selection_color = None,
				 border_color = None,
				 outline_color = None,
				 border_size = None,
				 outline_size = None,
				 position_technique = None,
				 is_focusable = None,
				 comment = None,
				 animation = None,
				 scale = None,
				 tile = None,
				 opaque = None,
				 repeating = None,
				 play = None):
				 
		self.real_widget = fifechan.AnimationIcon()
		self._anim = None
		
		super(AnimationIcon,self).__init__(parent=parent,
								  name=name,
								  size=size,
								  min_size=min_size,
								  max_size=max_size,
								  fixed_size=fixed_size,
								  margins=margins,
								  padding=padding,
								  helptext=helptext,
								  position=position,
								  style=style,
								  hexpand=hexpand,
								  vexpand=vexpand,
								  font=font,
								  base_color=base_color,
								  background_color=background_color,
								  foreground_color=foreground_color,
								  selection_color=selection_color,
								  border_color=border_color,
								  outline_color=outline_color,
								  border_size=border_size,
								  outline_size=outline_size,
								  position_technique=position_technique,
								  is_focusable=is_focusable,
								  comment=comment)

		# set provided attributes or defaults
		if scale is not None: self.scale = scale
		else: self.scale = self.DEFAULT_SCALE

		if tile is not None: self.tile = tile
		else: self.tile = self.DEFAULT_TILE

		if opaque is not None: self.opaque = opaque
		else: self.opaque = self.DEFAULT_OPAQUE

		# for the case that animation can not be found, e.g. invalid path
		# the AnimationIcon is removed from the manager
		try:
			self.animation = animation
		except Exception:
			get_manager().removeWidget(self)
			raise

		if repeating is not None: self.repeating = repeating
		else: self.repeating = self.DEFAULT_REPEATING

		if play is not None: self.play = play
		else: self.play = self.DEFAULT_PLAY
		
		#if the size parameter is specified set it (again) to override
		#the icons size. That works only without layouting.
		if size is not None: self.size = size

	def clone(self, prefix):
		iconClone = AnimationIcon(None,
				self._createNameWithPrefix(prefix),
				self.size,
				self.min_size, 
				self.max_size,
				self.fixed_size,
				self.margins,
				self.padding,
				self.helptext, 
				self.position, 
				self.style, 
				self.hexpand,
				self.vexpand,
				self.font,
				self.base_color,
				self.background_color,
				self.foreground_color,
				self.selection_color,
				self.border_color,
				self.outline_color,
				self.border_size,
				self.outline_size,
				self.position_technique,
				self.is_focusable,
				self.comment,
				self.animation,
				self.scale,
				self.tile,
				self.opaque,
				self.repeating,
				self.play)
						
		return iconClone

	def _setAnimation(self, anim):
		if anim is not None:
			if isinstance(anim, fife.Animation):
				self._anim = anim
			else:
				if anim is not "":
					# use xml loader
					self._anim = loadXMLAnimation(get_manager().hook.engine, anim)
			self.real_widget.setAnimation(self._anim)
	def _getAnimation(self):
		return self._anim
	animation = property(_getAnimation, _setAnimation)

	def _setScaling(self, val):	self.real_widget.setScaling(val)
	def _getScaling(self): return self.real_widget.isScaling()
	scale = property(_getScaling, _setScaling)

	def _setTiling(self, val):	self.real_widget.setTiling(val)
	def _getTiling(self): return self.real_widget.isTiling()
	tile = property(_getTiling, _setTiling)

	def _setOpaque(self, val):	self.real_widget.setOpaque(val)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque, _setOpaque)

	def _setRepeating(self, repeat): self.real_widget.setRepeating(repeat)
	def _getRepeating(self): return self.real_widget.isRepeating()
	repeating = property(_getRepeating, _setRepeating)
	
	def startPlaying(self):
		self.real_widget.play()

	def isPlaying(self):
		return self.real_widget.isPlaying()

	def pausePlaying(self):
		self.real_widget.pause()

	def stopPlaying(self):
		self.real_widget.stop()

	def _setPlaying(self, play):
		if play is True: self.startPlaying()
		else: self.stopPlaying()
	def _getPlaying(self): return self.isPlaying()
	play = property(_getPlaying, _setPlaying)

