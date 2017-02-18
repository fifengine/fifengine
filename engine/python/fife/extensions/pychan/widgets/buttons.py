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

from fife.extensions.pychan.attrs import Attr, BoolAttr, PointAttr, IntAttr
from fife.extensions.pychan.properties import ImageProperty

from common import text2gui
from basictextwidget import BasicTextWidget


class Button(BasicTextWidget):
	"""
	A basic push button.
	
	New Attributes
	==============
	
	  - active: Bool: If false the button is inactiv then it looks gray.
	  - alignment: Int: Sets the alignment of the caption. The alignment is relative to the center of the button.
	  - offset: Point: The offset of the caption when the button is pressed.
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [ BoolAttr('active'),
												IntAttr('alignment'),
												PointAttr('offset')
												]

	DEFAULT_ACTIVE = True
	# 0=Left, 1=Center, 2=Right
	DEFAULT_ALIGNMENT = 1
	DEFAULT_OFFSET = 1,1
	
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
				 text = None,
				 active = None,
				 alignment = None,
				 offset = None,
				 real_widget = None):

		if real_widget is None:
			self.real_widget = fifechan.Button("")
		else:
			self.real_widget = real_widget

		# set the defaults
		#offset = self.DEFAULT_OFFSET
		
		super(Button,self).__init__(parent=parent,
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
									comment=comment,
									text=text)

		# set provided attributes or defaults
		if active is not None: self.active = active
		else: self.active = self.DEFAULT_ACTIVE

		if alignment is not None: self.alignment = alignment
		else: self.alignment = self.DEFAULT_ALIGNMENT
		
		if offset is not None: self.offset = offset
		else: self.offset = self.DEFAULT_OFFSET
									
	def clone(self, prefix):
		btnClone = Button(None, 
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
					self.text,
					self.active,
					self.alignment,
					self.offset)
		return btnClone

	def _setActive(self, active): self.real_widget.setActive(active)
	def _isActive(self): return self.real_widget.isActive()
	active = property(_isActive, _setActive)

	def _setAlignment(self, alignment): self.real_widget.setAlignment(alignment)
	def _getAlignment(self): return self.real_widget.getAlignment()
	alignment = property(_getAlignment, _setAlignment)
	
	def _setOffset(self, offset): self.real_widget.setDownOffset(offset[0], offset[1])
	def _getOffset(self): return (self.real_widget.getDownXOffset(), self.real_widget.getDownYOffset())
	offset = property(_getOffset,_setOffset)

	def _setXOffset(self, x): self.real_widget.setDownXOffset(x)
	def _getXOffset(self): return self.real_widget.getDownXOffset()
	x_offset = property(_getXOffset, _setXOffset)

	def _setYOffset(self, y): self.real_widget.setDownYOffset(y)
	def _getYOffset(self): return self.real_widget.getDownYOffset()
	y_offset = property(_getYOffset, _setYOffset)
	

class ImageButton(Button):
	"""
	A basic push button with six different images for the up, down and hover state.

	New Attributes
	==============

	  - up_image: String: The source location of the Image for the B{unpressed} state.
	  - down_image: String: The source location of the Image for the B{pressed} state.
	  - hover_image: String: The source location of the Image for the B{unpressed hovered} state.
	  - in_up_image: String: The source location of the Image for the B{unpressed} inactive state.
	  - in_down_image: String: The source location of the Image for the B{pressed} inactive state.
	  - in_hover_image: String: The source location of the Image for the B{unpressed hovered} inactive state.
	"""

	ATTRIBUTES = Button.ATTRIBUTES + [ Attr('up_image'),
									   Attr('down_image'),
									   Attr('hover_image'),
									   Attr('in_up_image'),
									   Attr('in_down_image'),
									   Attr('in_hover_image')
									]

	#DEFAULT_OFFSET = 0,0
	
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
				 text = None,
				 active = None,
				 alignment = None,
				 offset = None,
				 up_image = None,
				 down_image = None,
				 hover_image = None,
				 in_up_image = None,
				 in_down_image = None,
				 in_hover_image = None,
				 real_widget = None):

		if real_widget is None:
			real_widget = fifechan.ImageButton()
		self.real_widget = real_widget
		
		# set the defaulst
		#offset = self.DEFAULT_OFFSET
		
		super(ImageButton,self).__init__(parent=parent, 
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
										 comment=comment,
										 text=text,
										 active=active,
										 alignment=alignment,
										 offset=offset,
										 real_widget=self.real_widget)
		

		# for the case that image can not be found, e.g. invalid path
		# the ImageButton is removed from the manager
		try:
			self.up_image = up_image
			self.down_image = down_image
			self.hover_image = hover_image
			self.in_up_image = in_up_image
			self.in_down_image = in_down_image
			self.in_hover_image = in_hover_image
		except Exception:
			get_manager().removeWidget(self)
			raise
		
		# Override anything set when stylize was called
		#if offset is not None: self.offset = offset

		
	def clone(self, prefix):
		
		imgButtonClone = ImageButton(None, 
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
						self.text,
						self.active,
						self.alignment,
						self.offset,
						self.up_image,
						self.down_image,
						self.hover_image,
						self.in_up_image,
						self.in_down_image,
						self.in_hover_image)
		return imgButtonClone
		
	
	up_image = ImageProperty("UpImage")
	down_image = ImageProperty("DownImage")
	hover_image = ImageProperty("HoverImage")
	in_up_image = ImageProperty("InactiveUpImage")
	in_down_image = ImageProperty("InactiveDownImage")
	in_hover_image = ImageProperty("InactiveHoverImage")


class ToggleButton(ImageButton):
	"""
	A basic push button that can be toggled.

	New Attributes
	==============

	  - group: String: The group the button belongs to. Only one button in each group will be toggled at one time.
	  - toggled: Boolean: Whether the button is toggled or not.

	Data
	====
	The toggled status can be read and set via L{distributeData} and L{collectData}
	"""

	ATTRIBUTES = ImageButton.ATTRIBUTES + [ Attr('group'),
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
				 text = None,
				 active = None,
				 alignment = None,
				 offset = None,
				 up_image = None,
				 down_image = None,
				 hover_image = None,
				 in_up_image = None,
				 in_down_image = None,
				 in_hover_image = None,
				 group = None,
				 toggled = None):

		self.real_widget = fifechan.ToggleButton()
		
		super(ToggleButton,self).__init__(parent=parent,
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
										  comment=comment,
										  text=text,
										  active=active,
										  alignment=alignment,
										  offset=offset,
										  up_image=up_image,
										  down_image=down_image,
										  hover_image=hover_image,
										  in_up_image=in_up_image,
										  in_down_image=in_down_image,
										  in_hover_image=in_hover_image,
										  real_widget=self.real_widget)
										  
		# set provided attributes or defaults
		if group is not None: self.group = group
		else: self.group = self.DEFAULT_GROUP
		
		if toggled is not None: self.toggled = toggled
		else: self.toggled = self.DEFAULT_TOGGLED
		
		# Prepare Data collection framework
		self.accepts_data = True
		self._realGetData = self._isToggled
		self._realSetData = self._setToggled

	def clone(self, prefix):
		toggleButtonClone = ToggleButton(None,
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
							self.text,
							self.active,
							self.alignment,
							self.offset,
							self.up_image,
							self.down_image,
							self.hover_image,
							self.in_up_image,
							self.in_down_image,
							self.in_hover_image,
							self.group,
							self.toggled)
		return toggleButtonClone
		
	def _setGroup(self, group):
		if group is None: group = ""
		self.real_widget.setGroup(group)
	def _getGroup(self): return self.real_widget.getGroup()
	group = property(_getGroup,_setGroup)

	def _setToggled(self, toggled): self.real_widget.setSelected(toggled)
	def _isToggled(self): return self.real_widget.isSelected()
	toggled = property(_isToggled, _setToggled)
