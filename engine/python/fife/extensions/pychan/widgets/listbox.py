# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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
from widget import Widget

class GenericListmodel(fife.ListModel,list):
	"""
	A wrapper for the exported list model to behave more like a Python list.
	Don't use directly.
	"""
	def __init__(self,*args):
		super(GenericListmodel,self).__init__()
		map(self.append,args)
	def clear(self):
		while len(self):
			self.pop()
	def getNumberOfElements(self):
		return len(self)

	def getElementAt(self, i):
		i = max(0,min(i,len(self) - 1))
		return text2gui(unicode(self[i]))

class ListBox(Widget):
	"""
	A basic list box widget for displaying lists of strings. It makes most sense to wrap
	this into a L{ScrollArea}.

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
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 1

	def __init__(self,items=[],**kwargs):
		self._items = GenericListmodel(*items)
		self.real_widget = fife.ListBox(self._items)
		super(ListBox,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_initial_data = True
		self._realSetInitialData = self._setItems

		self.accepts_data = True
		self._realSetData = self._setSelected
		self._realGetData = self._getSelected

	def resizeToContent(self,recurse=True):
		# We append a minimum value, so max() does not bail out,
		# if no items are in the list
		_item_widths = map(self.real_font.getWidth,map(gui2str,self._items)) + [0]
		max_w = max(_item_widths)
		self.width = max_w
		self.height = (self.real_font.getHeight() + 2) * len(self._items)

	def _getItems(self): return self._items
	def _setItems(self,items):
		# Note we cannot use real_widget.setListModel
		# for some reason ???

		# Also self assignment can kill you
		if id(items) != id(self._items):
			self._items.clear()
			self._items.extend(items)

	items = property(_getItems,_setItems)

	def _getSelected(self): return self.real_widget.getSelected()
	def _setSelected(self,index): self.real_widget.setSelected(index)
	selected = property(_getSelected,_setSelected)
	def _getSelectedItem(self):
		if 0 <= self.selected < len(self._items):
			return self._items[self.selected]
		return None
	selected_item = property(_getSelectedItem)
