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

from fife.extensions.pychan.attrs import IntAttr

from widget import Widget


class PercentageBar(Widget):
	""" A percentage bar widget

	New Attributes
	==============

	  - orientation: 1 = horizontal, 0=vertical
	  - value: int: default 0

	"""

	HORIZONTAL = fifechan.PercentageBar.HORIZONTAL
	VERTICAL = fifechan.PercentageBar.VERTICAL

	ATTRIBUTES = Widget.ATTRIBUTES + [ IntAttr('orientation'),
									   IntAttr('value')
									 ]
	DEFAULT_HEXPAND = True
	DEFAULT_VEXPAND = False
	DEFAULT_MIN_SIZE = 10,10
	DEFAULT_VALUE = 0
	DEFAULT_ORIENTATION = HORIZONTAL

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
				 value = None,
				 orientation = None):

		self.real_widget = fifechan.PercentageBar()
		self.orientation = self.DEFAULT_ORIENTATION
		self.value = self.DEFAULT_VALUE

		super(PercentageBar, self).__init__(parent=parent,
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

		if orientation is not None: self.orientation = orientation
		if value is not None: self.value = value

		self.accepts_data = True
		self._realSetData = self._setValue
		self._realGetData = self._getValue

	def clone(self, prefix):
		pbarClone = PercentageBar(None,
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
						self.value,
						self.orientation)
		return pbarClone

	def _getValue(self):
		"""getValue(self) -> int"""
		return self.real_widget.getValue()

	def _setValue(self, value):
		"""setValue(self, int value)"""
		if type(value) != int:
			raise RuntimeError("PercentageBar only accepts integer values")
		self.real_widget.setValue(value)
	value = property(_getValue, _setValue)

	def _setOrientation(self, orientation):
		"""setOrientation(self, Orientation orientation)"""
		self.real_widget.setOrientation(orientation)

	def _getOrientation(self):
		"""getOrientation(self) -> int"""
		return self.real_widget.getOrientation()
	orientation = property(_getOrientation, _setOrientation)

