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

from fife import fife
from fife import fifechan

from fife.extensions.pychan.attrs import IntAttr

from containers import Container
from widget import Widget

class AdjustingContainer(Container):
	"""
	This is the adjusting container class. It provides space in which child widgets can
	be position via the position attribute.

	New Attributes
	==============

	  - colums - Integer: The number of columns to divide the widgets into.
	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [ IntAttr('colums'),
									 ]
	DEFAULT_NUMBER_COLUMS = 3
	
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
				 padding = None,
				 background_image = None,
				 opaque = None,
				 margins = None,
				 _real_widget = None,
				 colums = None):

		self.real_widget = _real_widget or fifechan.AdjustingContainer()
		self.colums = self.DEFAULT_NUMBER_COLUMS

		super(AdjustingContainer,self).__init__(parent=parent, 
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

		if colums is not None: self.colums = colums
		
	def clone(self, prefix):
		containerClone = AdjustingContainer(None, 
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
						self.padding,
						self.background_image,
						self.opaque,
						self.margins,
						self.colums)
			
		containerClone.addChildren(self._cloneChildren(prefix))	
		return containerClone
		
	def _setNumberColums(self, number):
		self.real_widget.setNumberOfColumns(number)
	def _getNumberColums(self):
		self.real_widget.getNumberOfColumns()
	colums = property(_getNumberColums, _setNumberColums)
	
	def setColumAlignment(self, column, alignment):
		self.real_widget.setColumnAlignment(column, alignment)
	def getColumAlignment(self, column):
		self.real_widget.getColumAlignment(column)
		
		