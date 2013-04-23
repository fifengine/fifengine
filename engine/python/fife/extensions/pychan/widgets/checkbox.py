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

from fife.extensions.pychan.attrs import BoolAttr

from basictextwidget import BasicTextWidget


class CheckBox(BasicTextWidget):
	"""
	A basic checkbox.

	New Attributes
	==============

	  - marked: Boolean value, whether the checkbox is checked or not.

	Data
	====
	The marked status can be read and set via L{distributeData} and L{collectData}
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [ BoolAttr('marked') ]

	DEFAULT_MARKED = False

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
				 marked = None):
				 
		self.real_widget = fifechan.CheckBox()
		self.marked = self.DEFAULT_MARKED
		super(CheckBox,self).__init__(parent=parent, 
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
		if marked is not None: 
			self.marked = marked

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
						self.margins,
						self.text,
						self.marked)
		return checkboxClone

	def _isMarked(self): return self.real_widget.isSelected()
	def _setMarked(self,mark): self.real_widget.setSelected(mark)
	marked = property(_isMarked,_setMarked)
