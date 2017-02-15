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

from fife.extensions.pychan.properties import ImageProperty
from fife.extensions.pychan.attrs import Attr, BoolAttr, IntAttr

from widget import Widget


class IconProgressBar(Widget):
	"""
	An image icon.

	New Attributes
	==============

	  - image: String or GuiImage: The source location of the Image or a direct GuiImage
	  - max_icons: Maximal count of icons
	  - icons: Current count of active icons
	  - orientation: Horizontal (0) or Vertical (1)
	  - opaque: True if the widget is opaque, false otherwise.
	"""
	ATTRIBUTES = Widget.ATTRIBUTES + [ Attr('image'), 
									   IntAttr('max_icons'),
									   IntAttr('icons'),
									   IntAttr('orientation'),
									   BoolAttr('opaque')
									 ]

	DEFAULT_OPAQUE = True
									 
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
				 max_icons = None,
				 icons = None,
				 orientation = None,
				 opaque = None):
				 
		self.real_widget = fifechan.IconProgressBar() 
		self.opaque = opaque or self.DEFAULT_OPAQUE
		
		super(IconProgressBar,self).__init__(parent=parent,
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
		
		# for the case that image can not be found, e.g. invalid path
		# the IconProgressbar is removed from the manager
		try:
			self.image = image
		except Exception:
			get_manager().removeWidget(self)
			raise

		if max_icons is not None: self.max_icons = max_icons
		if icons is not None: self.icons = icons

	def clone(self, prefix):
		iconClone = IconProgressBar(None,
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
				self.max_icons,
				self.icons,
				self.orientation,
				self.opaque)
				 
		
		return iconClone
		
	_image = ImageProperty("Image")

	def advance(self):
		self.real_widget.advance()
		
	def reset(self):
		self.real_widget.reset()
	
	def _setImage(self,source):
		self._image = source

	def _getImage(self):
		return self._image
	image = property(_getImage,_setImage)
	
	def _setMaxIcons(self, maxIcons):
		if type(maxIcons) != int:
			raise RuntimeError("IconProgressBar max icons should be an integer")
		self.real_widget.setMaxIcons(maxIcons)
		
	def _getMaxIcons(self):
		return self.real_widget.getMaxIcons()
	max_icons = property(_getMaxIcons, _setMaxIcons)

	def _setIcons(self, icons):
		if type(icons) != int:
			raise RuntimeError("IconProgressBar icons should be an integer")
		self.real_widget.setIconCount(icons)	
	def _getIcons(self):
		return self.real_widget.getIconCount()
	icons = property(_getIcons, _setIcons)
	
	def _setOrientation(self, orientation):
		self.real_widget.setOrientation(orientation)
	
	def _getOrientation(self):
		return self.real_widget.getOrientation()
	orientation = property(_getOrientation, _setOrientation)
	
	def _setOpaque(self,opaque): 
		self.real_widget.setOpaque(opaque)
	def _getOpaque(self): 
		return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)
