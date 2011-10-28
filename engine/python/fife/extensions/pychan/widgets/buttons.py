# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2011 by the FIFE team
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

from common import *
from basictextwidget import BasicTextWidget
from fife.extensions.pychan.properties import ImageProperty

class Button(BasicTextWidget):
	"""
	A basic push button.
	"""
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
				 margins = None,
				 text = None):
				 
		self.real_widget = fife.Button("")
		super(Button,self).__init__(parent=parent, 
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
									comment=comment,
									margins=margins,
									text=text)

class ImageButton(BasicTextWidget):
	"""
	A basic push button with three different images for the up, down and hover state.

	B{Work in progress.}

	New Attributes
	==============

	  - up_image: String: The source location of the Image for the B{unpressed} state.
	  - down_image: String: The source location of the Image for the B{pressed} state.
	  - hover_image: String: The source location of the Image for the B{unpressed hovered} state.
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [ Attr('up_image'),
												Attr('down_image'),
												PointAttr('offset'),
												Attr('hover_image')
											  ]

	DEFAULT_UPIMAGE = ""
	DEFAULT_DOWNIMAGE = ""
	DEFAULT_HOVERIMAGE = ""
	DEFAULT_OFFSET = 0,0
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
				 margins = None,
				 text = None,
				 up_image=None,
				 down_image=None,
				 hover_image=None,
				 offset=None,
				 real_widget=None):

		if real_widget is None:
			self.real_widget = fife.TwoButton()
		else:
			self.real_widget = real_widget
		
		# set the defaulst
		offset = self.DEFAULT_OFFSET
		
		super(ImageButton,self).__init__(parent=parent, 
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
										 comment=comment,
										 margins=margins,
										 text=text)
		
		if up_image is not None: 
			self.up_image = up_image
		else:
			self.up_image = self.DEFAULT_UPIMAGE
			
		if down_image is not None: 
			self.down_image = down_image
		else:
			self.down_image = self.DEFAULT_DOWNIMAGE
			
		if hover_image is not None: 
			self.hover_image = hover_image
		else:
			self.hover_image = self.DEFAULT_HOVERIMAGE
		
		# Override anything set when stylize was called
		if offset is not None: self.offset = offset

	up_image = ImageProperty("UpImage")
	down_image = ImageProperty("DownImage")
	hover_image = ImageProperty("HoverImage")

	def _setOffset(self, offset):
		self.real_widget.setDownOffset(offset[0], offset[1])
	def _getOffset(self):
		return (self.real_widget.getDownXOffset(), self.real_widget.getDownYOffset())
	offset = property(_getOffset,_setOffset)

	def resizeToContent(self, recurse=True):
		th, tw = 0, 0
		if self.text:
			th = self.real_font.getHeight()#+self.real_font.getSpacing()
			tw = self.real_font.getWidth(text2gui(self.text))#+self.real_font.getSpacing()
		self.height = max(
			self._prop_upimage["image"].getHeight(),
			self._prop_downimage["image"].getHeight(),
			self._prop_hoverimage["image"].getHeight(),
			th) + self.margins[1]*2
		self.width = max(
			self._prop_upimage["image"].getWidth(),
			self._prop_downimage["image"].getWidth(),
			self._prop_hoverimage["image"].getWidth(),
			tw) + self.margins[0]*2

class ToggleButton(ImageButton):
	"""
	A basic push button that can be toggled.

	Unfortunately a bit of code duplication from ImageButton.

	New Attributes
	==============

	  - group: String: The group the button belongs to. Only one button in each group will be toggled at one time.
	  - toggled: Boolean: Whether the button is toggled or not.
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [	Attr('up_image'),
												Attr('down_image'),
												Attr('hover_image'),
												PointAttr('offset'),
												Attr('group')
											  ]
	DEFAULT_GROUP = ""

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
				 margins = None,
				 text = None,
				 up_image = None,
				 down_image = None,
				 hover_image = None,
				 offset = None, 
				 group = None):

		group = self.DEFAULT_GROUP

		super(ToggleButton,self).__init__(parent=parent, 
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
										  comment=comment,
										  margins=margins,
										  text=text,
										  up_image=up_image,
										  down_image=down_image,
										  hover_image=hover_image,
										  offset=offset,
										  real_widget=fife.ToggleButton())
										  
		if group is not None: self.group = group

	def _setGroup(self,group):
		if group is not None and group != "":
			self.real_widget.setGroup( group )

	def _getGroup(self):
		return self.real_widget.getGroup()
	group = property(_getGroup,_setGroup)

	def _setToggled(self, toggled):
		self.real_widget.setToggled( toggled )

	def _isToggled(self):
		return self.real_widget.isToggled()
	toggled = property(_isToggled, _setToggled)

	def resizeToContent(self, recurse=True):
		# NOTE: Figure out how the spacing comes into play
		tw, th = 0, 0
		if self.text:
			th = self.real_font.getHeight() + self.real_widget.getSpacing()
			tw = self.real_font.getWidth(text2gui(self.text)) + self.real_widget.getSpacing()
		self.height = max(
			self._prop_upimage["image"].getHeight(),
			self._prop_downimage["image"].getHeight(),
			self._prop_hoverimage["image"].getHeight(),
			th) + self.margins[1]*2
		self.width = max(
			self._prop_upimage["image"].getWidth(),
			self._prop_downimage["image"].getWidth(),
			self._prop_hoverimage["image"].getWidth(),
			tw) + self.margins[0]*2
