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

from fife.extensions.pychan.attrs import Attr, BoolAttr, IntAttr
from fife.extensions.pychan.properties import ImageProperty

from buttons import ImageButton


class CheckBox(ImageButton):
	"""
	A basic checkbox.

	New Attributes
	==============

	  - marked: Boolean value, whether the checkbox is checked or not.
	  - marker_style: Integer: The visual style of the marker.
	  - background_image: String: Optional image for the background, the size should also include the caption.

	Data
	====
	The marked status can be read and set via L{distributeData} and L{collectData}
	"""

	ATTRIBUTES = ImageButton.ATTRIBUTES + [ BoolAttr('marked'),
											IntAttr('marker_style'),
											Attr('background_image')]

	DEFAULT_MARKED = False
	# 0=Checkmark, 1=Cross, 2=Dot, 3=Rhombus, 4=Image
	DEFAULT_MARKER_STYLE = 0
	DEFAULT_OFFSET = 0,0

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
				 marked = None,
				 marker_style = None,
				 background_image = None):
				 
		self.real_widget = fifechan.CheckBox()

		super(CheckBox,self).__init__(parent=parent,
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
		if marked is not None: self.marked = marked
		else: self.marked = self.DEFAULT_MARKED
		
		if marker_style is not None: self.marker_style = marker_style
		else: self.marker_style = self.DEFAULT_MARKER_STYLE

		# for the case that image can not be found, e.g. invalid path
		# the Checkbox is removed from the manager
		try:
			self.background_image = background_image
		except Exception:
			get_manager().removeWidget(self)
			raise

		# Prepare Data collection framework
		self.accepts_data = True
		self._realGetData = self._isMarked
		self._realSetData = self._setMarked

	def clone(self, prefix):
		checkboxClone = CheckBox(None, 
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
						self.marked,
						self.marker_style,
						self.background_image)
		return checkboxClone

	def _isMarked(self): return self.real_widget.isSelected()
	def _setMarked(self,mark): self.real_widget.setSelected(mark)
	marked = property(_isMarked,_setMarked)

	def _getMarkerStyle(self): return self.real_widget.getMarkerStyle()
	def _setMarkerStyle(self, style): self.real_widget.setMarkerStyle(style)
	marker_style = property(_getMarkerStyle, _setMarkerStyle)

	background_image = ImageProperty("BackgroundImage")
