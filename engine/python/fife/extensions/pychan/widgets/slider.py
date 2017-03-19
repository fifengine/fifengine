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

from fife.extensions.pychan.attrs import IntAttr, FloatAttr

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

	HORIZONTAL = fifechan.Slider.Horizontal
	VERTICAL = fifechan.Slider.Vertical

	ATTRIBUTES = Widget.ATTRIBUTES + [ IntAttr('orientation'),
									   FloatAttr('scale_start'),
									   FloatAttr('scale_end'),
									   FloatAttr('step_length'),
									   IntAttr('marker_length')
									 ]
	DEFAULT_HEXPAND = True
	DEFAULT_VEXPAND = False
	
	DEFAULT_SIZE = 10,10
	DEFAULT_MIN_SIZE = 10,10

	DEFAULT_SCALE_START = 0.0
	DEFAULT_SCALE_END = 1.0
	DEFAULT_STEP_LENGTH = 0.1
	DEFAULT_MARKER_LENGTH = 10
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
				 scale_start = None,
				 scale_end = None,
				 step_length = None,
				 marker_length = None,
				 orientation = None):

		self.real_widget = fifechan.Slider(scale_start or self.DEFAULT_SCALE_START, scale_end or self.DEFAULT_SCALE_END)
		self.orientation = self.DEFAULT_ORIENTATION
		self.step_length = self.DEFAULT_STEP_LENGTH
		self.marker_length = self.DEFAULT_MARKER_LENGTH

		super(Slider, self).__init__(parent=parent,
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
		if scale_start is not None: self.scale_start = scale_start
		if scale_end is not None: self.scale_end = scale_end
		if step_length is not None: self.step_length = step_length
		if marker_length is not None: self.marker_length = marker_length

		self.accepts_data = True
		self._realSetData = self._setValue
		self._realGetData = self._getValue

	def clone(self, prefix):
		sliderClone = Slider(None,
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
					self.scale_start,
					self.scale_end,
					self.step_length,
					self.marker_length,
					self.orientation)
		return sliderClone

	def _setScale(self, start, end):
		"""setScale(self, double scaleStart, double scaleEnd)"""
		if type(start) != float:
			raise RuntimeError("Slider expects float for start scale")
		if type(end) != float:
			raise RuntimeError("Slider expects float for end scale")
		self.real_widget.setScale(start, end)

	def _getScaleStart(self):
		"""getScaleStart(self) -> double"""
		return self.real_widget.getScaleStart()

	def _setScaleStart(self, start):
		"""setScaleStart(self, double scaleStart)"""
		if type(start) != float:
			raise RuntimeError("Slider expects float for start scale")
		self.real_widget.setScaleStart(start)
	scale_start = property(_getScaleStart, _setScaleStart)

	def _getScaleEnd(self):
		"""getScaleEnd(self) -> double"""
		return self.real_widget.getScaleEnd()

	def _setScaleEnd(self, end):
		"""setScaleEnd(self, double scaleEnd)"""
		if type(end) != float:
			raise RuntimeError("Slider expects float for end scale")
		self.real_widget.setScaleEnd(end)
	scale_end = property(_getScaleEnd, _setScaleEnd)

	def _getValue(self):
		"""getValue(self) -> double"""
		return self.real_widget.getValue()

	def _setValue(self, value):
		"""setValue(self, double value)"""
		if type(value) != float:
			raise RuntimeError("Slider only accepts float values")
		self.real_widget.setValue(value)
	value = property(_getValue, _setValue)

	def _setMarkerLength(self, length):
		"""setMarkerLength(self, int length)"""
		if type(length) != int:
			raise RuntimeError("Slider only accepts int for Marker length")
		self.real_widget.setMarkerLength(length)

	def _getMarkerLength(self):
		"""getMarkerLength(self) -> int"""
		return self.real_widget.getMarkerLength()
	marker_length = property(_getMarkerLength, _setMarkerLength)

	def _setOrientation(self, orientation):
		"""setOrientation(self, Orientation orientation)"""
		self.real_widget.setOrientation(orientation)

	def _getOrientation(self):
		"""getOrientation(self) -> int"""
		return self.real_widget.getOrientation()
	orientation = property(_getOrientation, _setOrientation)

	def _setStepLength(self, length):
		"""setStepLength(self, double length)"""
		if type(length) != float:
			raise RuntimeError("Slider only accepts floats for step length")
		self.real_widget.setStepLength(length)

	def _getStepLength(self):
		"""getStepLength(self) -> double"""
		return self.real_widget.getStepLength()
	step_length = property(_getStepLength, _setStepLength)

