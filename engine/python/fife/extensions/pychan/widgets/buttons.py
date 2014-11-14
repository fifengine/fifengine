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

from fife.extensions.pychan.attrs import Attr, BoolAttr, PointAttr
from fife.extensions.pychan.properties import ImageProperty

from common import text2gui
from basictextwidget import BasicTextWidget


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
				 fixed_size = None,
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
				 margins = None,
				 text = None):
				 
		self.real_widget = fifechan.Button("")
		super(Button,self).__init__(parent=parent,
									name=name,
									size=size,
									min_size=min_size,
									max_size=max_size,
									fixed_size=fixed_size,
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
									comment=comment,
									margins=margins,
									text=text)
									
	def clone(self, prefix):
		btnClone = Button(None, 
					self._createNameWithPrefix(prefix),
					self.size,
					self.min_size,
					self.max_size,
					self.fixed_size,
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
					self.margins,
					self.text)
		return btnClone
			

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
				 fixed_size = None,
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
				 margins = None,
				 text = None,
				 up_image=None,
				 down_image=None,
				 hover_image=None,
				 offset=None,
				 real_widget=None):

		if real_widget is None:
			self.real_widget = fifechan.TwoButton()
		else:
			self.real_widget = real_widget
		
		# set the defaulst
		offset = self.DEFAULT_OFFSET
		
		super(ImageButton,self).__init__(parent=parent, 
										 name=name,
										 size=size,
										 min_size=min_size,
										 max_size=max_size,
										 fixed_size=fixed_size,
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

		
	def clone(self, prefix):
		
		imgButtonClone = ImageButton(None, 
						self._createNameWithPrefix(prefix),
						self.size,
						self.min_size,
						self.max_size,
						self.fixed_size,
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
						self.margins,
						self.text,
						self.up_image,
						self.down_image,
						self.hover_image,
						self.offset)
		return imgButtonClone
		
	
	up_image = ImageProperty("UpImage")
	down_image = ImageProperty("DownImage")
	hover_image = ImageProperty("HoverImage")

	def _setOffset(self, offset):
		self.real_widget.setDownOffset(offset[0], offset[1])
	def _getOffset(self):
		return (self.real_widget.getDownXOffset(), self.real_widget.getDownYOffset())
	offset = property(_getOffset,_setOffset)

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
												Attr('group'),
												BoolAttr('toggled')
											  ]
	DEFAULT_GROUP = ""
	DEFAULT_TOGGLED = False

	def __init__(self, 
				 parent = None,
				 name = None,
				 size = None,
				 min_size = None,
				 max_size = None,
				 fixed_size = None,
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
				 margins = None,
				 text = None,
				 up_image = None,
				 down_image = None,
				 hover_image = None,
				 offset = None, 
				 group = None,
				 toggled = None):

		group = self.DEFAULT_GROUP
		toggled = self.DEFAULT_TOGGLED

		super(ToggleButton,self).__init__(parent=parent,
										  name=name,
										  size=size,
										  min_size=min_size,
										  max_size=max_size,
										  fixed_size=fixed_size,
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
										  comment=comment,
										  margins=margins,
										  text=text,
										  up_image=up_image,
										  down_image=down_image,
										  hover_image=hover_image,
										  offset=offset,
										  real_widget=fifechan.ToggleButton())
										  
		if group is not None: self.group = group
		if toggled: self.toggled = toggled

	def clone(self, prefix):
		toggleButtonClone = ToggleButton(None,
							self._createNameWithPrefix(prefix),
							self.size,
							self.min_size,
							self.max_size,
							self.fixed_size,
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
							self.margins,
							self.text,
							self.up_image,
							self.down_image,
							self.hover_image,
							self.offset,
							self.group)
		return toggleButtonClone
		
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
