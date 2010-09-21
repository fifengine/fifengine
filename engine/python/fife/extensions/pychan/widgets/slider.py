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

class Slider(Widget):
	""" A slider widget

	Use a callback to read out the slider value every time the marker
	is moved.

	New Attributes
	==============

	  - orientation: 1 = horizontal, 0=vertical
	  - scale_start: float: default 0.0
	  - scale_end: float: default 1.0
	  - step_length: float: default scale_end/10
	  - marker_length: int: default 10
	FIXME:
		- update docstrings
	"""

	HORIZONTAL = fife.Slider.HORIZONTAL
	VERTICAL = fife.Slider.VERTICAL

	ATTRIBUTES = Widget.ATTRIBUTES + [IntAttr('orientation'), FloatAttr('scale_start'), FloatAttr('scale_end'), FloatAttr('step_length'), IntAttr('marker_length')]
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 0

	def __init__(self, scaleStart=0.0, scaleEnd=1.0, orientation=HORIZONTAL, min_size=(10,10),**kwargs):
		self.real_widget = fife.Slider(scaleStart, scaleEnd)
		self.orientation = orientation
		self.setOrientation(self.orientation)
		super(Slider, self).__init__(min_size=min_size,**kwargs)

		self.accepts_data = True
		self._realSetData = self.setValue
		self._realGetData = self.getValue

	def _setScale(self, start, end):
		"""setScale(self, double scaleStart, double scaleEnd)"""
		if type(start) != float:
			raise RuntimeError("Slider expects float for start scale")
		if type(end) != float:
			raise RuntimeError("Slider expects float for end scale")
		self.real_widget.setScale(start, end)

	def getScaleStart(self):
		"""getScaleStart(self) -> double"""
		return self.real_widget.getScaleStart()

	def setScaleStart(self, start):
		"""setScaleStart(self, double scaleStart)"""
		if type(start) != float:
			raise RuntimeError("Slider expects float for start scale")
		self.real_widget.setScaleStart(start)
	scale_start = property(getScaleStart, setScaleStart)

	def getScaleEnd(self):
		"""getScaleEnd(self) -> double"""
		return self.real_widget.getScaleEnd()

	def setScaleEnd(self, end):
		"""setScaleEnd(self, double scaleEnd)"""
		if type(end) != float:
			raise RuntimeError("Slider expects float for end scale")
		self.real_widget.setScaleEnd(end)
	scale_end = property(getScaleEnd, setScaleEnd)

	def getValue(self):
		"""getValue(self) -> double"""
		return self.real_widget.getValue()

	def setValue(self, value):
		"""setValue(self, double value)"""
		if type(value) != float:
			raise RuntimeError("Slider only accepts float values")
		self.real_widget.setValue(value)

	def setMarkerLength(self, length):
		"""setMarkerLength(self, int length)"""
		if type(length) != int:
			raise RuntimeError("Slider only accepts int for Marker lenght")
		self.real_widget.setMarkerLength(length)

	def getMarkerLength(self):
		"""getMarkerLength(self) -> int"""
		return self.real_widget.getMarkerLength()
	marker_length = property(getMarkerLength, setMarkerLength)

	def setOrientation(self, orientation):
		"""setOrientation(self, Orientation orientation)"""
		self.real_widget.setOrientation(orientation)

	def getOrientation(self):
		"""getOrientation(self) -> int"""
		return self.real_widget.getOrientation()
	orientation = property(getOrientation, setOrientation)

	def setStepLength(self, length):
		"""setStepLength(self, double length)"""
		if type(length) != float:
			raise RuntimeError("Slider only accepts floats for step length")
		self.real_widget.setStepLength(length)

	def getStepLength(self):
		"""getStepLength(self) -> double"""
		return self.real_widget.getStepLength()
	step_length = property(getStepLength, setStepLength)
