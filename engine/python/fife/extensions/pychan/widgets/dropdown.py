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

from common import text2gui
from listbox import GenericListmodel
from widget import Widget

class DropDown(Widget):
	"""
	A dropdown or combo box widget for selecting lists of strings.

	New Attributes
	==============

	  - items: A List of strings. This can be treated like an ordinary python list.
	    but only strings are allowed.
	  - selected: The index of the selected item in the list. Starting from C{0} to C{len(items)-1}.
	    A negative value indicates, that no item is selected.
	  - selected_item: The selected string itself, or C{None} - if no string is selected.

	Data
	====
	The selected attribute can be read and set via L{distributeData} and L{collectData}.
	The list items can be set via L{distributeInitialData}.
	"""
	
	DEFAULT_ITEMS = []
	
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
				 items = None,
				 selected = None):

		if items is None:
			self._items = GenericListmodel(*self.DEFAULT_ITEMS)
		else:
			if type(items) is list:
				self._items = GenericListmodel(*items)
			else:
				raise RuntimeError("items parameter must be a list!")

		if selected is not None and items is not None:
			if len(items) > 0 and selected < len(items) and selected > 0:
				self.selected = selected
			else:
				raise RuntimeError("Invalid selected item index specified for dropdown!")
		elif selected is None and items is not None:
			if len(items) > 0:
				self.selected = 0
				
		self.real_widget = fifechan.DropDown(self._items)
		super(DropDown,self).__init__(parent=parent, 
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

		# Prepare Data collection framework
		self.accepts_initial_data = True
		self._realSetInitialData = self._setItems

		self.accepts_data = True
		self._realSetData = self._setSelected
		self._realGetData = self._getSelected
		
	def clone(self, prefix):
		
		itemsList = []
		
		for i in range(self.items.getNumberOfElements()):
			itemsList.append(self.items.getElementAt(i))
		
		dropdownClone = DropDown(None,
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
						itemsList,
						self.selected)
		
		return dropdownClone

	def _getItems(self): return self._items #self.real_widget.getListModel() works too
	def _setItems(self,items):
		# Also self assignment can kill you but
		# without the GenericListmodel is freed instantly ;-)
		if id(items) != id(self._items):
			self._items.clear()
			self._items.extend(items)
			self.real_widget.setListModel(self._items)
	items = property(_getItems,_setItems)

	def _getSelected(self): return self.real_widget.getSelected()
	def _setSelected(self,index): self.real_widget.setSelected(index)
	selected = property(_getSelected,_setSelected)
	def _getSelectedItem(self):
		if 0 <= self.selected < len(self._items):
			return self._items[self.selected]
		return None
	selected_item = property(_getSelectedItem)
