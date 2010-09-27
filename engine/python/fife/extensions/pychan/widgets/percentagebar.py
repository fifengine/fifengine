# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
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
from widget import Widget

class PercentageBar(Widget):
	""" A percentage bar widget

	New Attributes
	==============

	  - orientation: 1 = horizontal, 0=vertical
	  - value: int: default 0

	"""

	HORIZONTAL = fife.PercentageBar.HORIZONTAL
	VERTICAL = fife.PercentageBar.VERTICAL

	ATTRIBUTES = Widget.ATTRIBUTES + [IntAttr('orientation'), IntAttr('value')]
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 0

	def __init__(self, value=0, orientation=HORIZONTAL, min_size=(10,10),**kwargs):
		self.real_widget = fife.PercentageBar()
		self.orientation = orientation
		self.setOrientation(self.orientation)
		self.setValue(value)
		super(PercentageBar, self).__init__(min_size=min_size,**kwargs)

		self.accepts_data = True
		self._realSetData = self.setValue
		self._realGetData = self.getValue

	def getValue(self):
		"""getValue(self) -> int"""
		return self.real_widget.getValue()

	def setValue(self, value):
		"""setValue(self, int value)"""
		if type(value) != int:
			raise RuntimeError("PercentageBar only accepts integer values")
		self.real_widget.setValue(value)
	value = property(getValue, setValue)

	def setOrientation(self, orientation):
		"""setOrientation(self, Orientation orientation)"""
		self.real_widget.setOrientation(orientation)

	def getOrientation(self):
		"""getOrientation(self) -> int"""
		return self.real_widget.getOrientation()
	orientation = property(getOrientation, setOrientation)
