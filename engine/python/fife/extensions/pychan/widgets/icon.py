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

from fife import fifechan

from fife.extensions.pychan.attrs import Attr, BoolAttr
from fife.extensions.pychan.properties import ImageProperty

from .widget import Widget


class Icon(Widget):
	"""
	An image icon.

	New Attributes
	==============

	  - image: String or GuiImage: The source location of the Image or a direct GuiImage
	"""
	ATTRIBUTES = Widget.ATTRIBUTES + [ Attr('image'),
									   BoolAttr('scale')
									 ]

	def __init__(self, 
				 parent = None, 
				 name = None,
				 size = None,
				 min_size = None, 
				 max_size = None, 
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
				 border_size = None,
				 position_technique = None,
				 is_focusable = None,
				 comment = None,
				 image = None,
				 scale = None):
				 
		self.real_widget = fifechan.Icon(None)
		super(Icon,self).__init__(parent=parent, 
								  name=name, 
								  size=size, 
								  min_size=min_size, 
								  max_size=max_size,
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
								  border_size=border_size,
								  position_technique=position_technique,
								  is_focusable=is_focusable,
								  comment=comment)

		if scale is not None: self.scale = scale

		self.image = image
		
		#if the size parameter is specified set it (again) to override
		#the icons size.
		if size is not None: self.size = size

	def clone(self, prefix):
		iconClone = Icon(None,
				self._createNameWithPrefix(prefix),
				self.size,
				self.min_size, 
				self.max_size, 
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
				self.border_size,
				self.position_technique,
				self.is_focusable,
				self.comment,
				self.image,
				self.scale)
				 
		
		return iconClone
		
	_image = ImageProperty("Image")

	def _setImage(self,source):
		self._image = source
		self._checkSize()

	def _getImage(self):
		return self._image
	image = property(_getImage,_setImage)

	def _setScaling(self, val):
		self.real_widget.setScaling(val)
		self._checkSize()

	def _getScaling(self):
		return self.real_widget.isScaling()
	scale = property(_getScaling, _setScaling)

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