# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
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

from fife import fifechan

from fife.extensions.pychan.attrs import Attr, BoolAttr
from fife.extensions.pychan.properties import ImageProperty

from common import get_manager
from widget import Widget


class Icon(Widget):
	"""
	An image icon.

	New Attributes
	==============

	  - image: String or GuiImage: The source location of the Image or a direct GuiImage
	  - scale: Boolean: True if the image should be scaled to widget size, false otherwise.
	  - tile: Boolean: True if the image should be tiled to widget size, false otherwise.
	  - opaque: Boolean: True if the background of the icon should be drawn, false otherwise.
	"""
	ATTRIBUTES = Widget.ATTRIBUTES + [ Attr('image'),
									   BoolAttr('scale'),
									   BoolAttr('tile'),
									   BoolAttr('opaque')
									 ]

	DEFAULT_SCALE = False
	DEFAULT_TILE = False
	DEFAULT_OPAQUE = False

	DEFAULT_MARGINS = 0,0
	DEFAULT_PADDING = 0
	
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
				 image = None,
				 scale = None,
				 tile = None,
				 opaque = None):
				 
		self.real_widget = fifechan.Icon(None)
		
		super(Icon,self).__init__(parent=parent,
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

		# for the case that image can not be found, e.g. invalid path
		# the Icon is removed from the manager
		try:
			self.image = image
		except Exception:
			get_manager().removeWidget(self)
			raise
		
		#if the size parameter is specified set it (again) to override
		#the icons size. That works only without layouting.
		if size is not None: self.size = size

	def clone(self, prefix):
		iconClone = Icon(None,
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
				self.image,
				self.scale)
						
		return iconClone
		
	image = ImageProperty("Image")

	def _setScaling(self, val):	self.real_widget.setScaling(val)
	def _getScaling(self): return self.real_widget.isScaling()
	scale = property(_getScaling, _setScaling)

	def _setTiling(self, val):	self.real_widget.setTiling(val)
	def _getTiling(self): return self.real_widget.isTiling()
	tile = property(_getTiling, _setTiling)

	def _setOpaque(self, val):	self.real_widget.setOpaque(val)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque, _setOpaque)

	def _checkSize(self):
		if not self.scale:
			if self.image is not None:
				self.min_size = self.image.getWidth(), self.image.getHeight()
			else:
				self.min_size = self.real_widget.getWidth(), self.real_widget.getHeight()
			self.max_size = self.min_size
			self.size = self.min_size
		else:
			if self.parent:
				self.min_size = self.parent.min_size
				self.max_size = self.parent.max_size
			else:
				self.min_size = Widget.DEFAULT_MIN_SIZE
				self.max_size = Widget.DEFAULT_MAX_SIZE
