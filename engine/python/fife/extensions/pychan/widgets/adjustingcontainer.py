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

from fife.extensions.pychan.attrs import IntAttr, IntListAttr

from containers import Container

class AdjustingContainer(Container):
	"""
	This is the adjusting container class. It provides space in which child widgets can
	be position via the position attribute.

	New Attributes
	==============

	  - colums - Integer: The number of columns to divide the widgets into.
	  - alignments: The alignment per column.
	"""

	ATTRIBUTES = Container.ATTRIBUTES + [ IntAttr('columns'),
										  IntListAttr('alignments'),
										]
	DEFAULT_NUMBER_COLUMNS = 3
	
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
				 background_image = None,
				 opaque = None,
				 layout = None,
				 spacing = None,
				 uniform_size = None,
				 _real_widget = None,
				 columns = None,
				 alignments = None):

		if _real_widget is None: _real_widget = fifechan.AdjustingContainer()
		
		super(AdjustingContainer,self).__init__(parent=parent,
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
												background_image=background_image,
												opaque=opaque,
												layout=layout,
												spacing=spacing,
												uniform_size=uniform_size,
												_real_widget=_real_widget)

		if columns is not None:
			self.columns = columns
		else:
			self.columns = self.DEFAULT_NUMBER_COLUMNS

		if alignments is not None:
			self.alignments = alignments
				
	def clone(self, prefix):
		containerClone = AdjustingContainer(None, 
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
						self.background_image,
						self.opaque,
						self.layout,
						self.spacing,
						self.uniform_size,
						None,
						self.columns,
						self.alignments)
			
		containerClone.addChildren(self._cloneChildren(prefix))	
		return containerClone
		
	def _setNumberColumns(self, number):
		self.real_widget.setNumberOfColumns(number)
	def _getNumberColumns(self):
		self.real_widget.getNumberOfColumns()
	columns = property(_getNumberColumns, _setNumberColumns)
	
	def _setColumnAlignment(self, column, alignment):
		self.real_widget.setColumnAlignment(column, alignment)
	def _getColumnAlignment(self, column):
		self.real_widget.getColumnAlignment(column)
	column_alignment = property(_getColumnAlignment, _setColumnAlignment)

	def _setColumnAlignments(self, alignments):
		i = 0
		if alignments is not None:
			for a in alignments:
				self.real_widget.setColumnAlignment(i, a)
				i += 1
		else:
			cols = self.columns
			while i < cols:
				self.real_widget.setColumnAlignment(i, 0)
				i += 1
	def _getColumnAlignments(self):
		alignments = []
		cols = self.columns
		i = 0
		while i < cols:
			alignments.append(self.column_alignment(i))
			i += 1
		return alignments
	alignments = property(_getColumnAlignments, _setColumnAlignments)

