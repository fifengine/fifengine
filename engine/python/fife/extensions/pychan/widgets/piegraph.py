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

import weakref
from fife import fifechan
from fife.extensions.pychan.attrs import BoolAttr, IntAttr, PointAttr, MixedListAttr
from widget import Widget

class PieSegment(object):
	def __init__(self, pie, start, stop, color):
		self.pie = weakref.ref(pie)
		self.start = start
		self.stop = stop
		self._color = None
		self.color = color

	def _setStartAngle(self, angle):
		self.start = angle
		if self.pie is not None:
			self.pie().update()
	def _getStartAngle(self): return self.start
	start_angle = property(_getStartAngle, _setStartAngle)

	def _setStopAngle(self, angle):
		self.stop = angle
		if self.pie is not None:
			self.pie().update()
	def _getStopAngle(self): return self.stop
	stop_angle = property(_getStopAngle, _setStopAngle)

	def _setColor(self, color):
		if isinstance(color, tuple):
			self._color = fifechan.Color(*color)
		else:
			# Force a copy to get value semantics
			self._color = fifechan.Color(color.r,color.g,color.b,color.a)
		if self.pie is not None:
			self.pie().update()
	def _getColor(self):
		return fifechan.Color(self._color.r, self._color.g, self._color.b, self._color.a)
	color = property(_getColor, _setColor)


class PieGraph(Widget):
	""" A pie graph widget

	New Attributes
	==============

	  - coordinates: int list: x and y coordinates
	  - thickness': int: Line thickness, default 1
	  - pie_color: color: Pie color
	  - opaque: bool: default False
	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [ PointAttr('center'),
									   IntAttr('radius'),
									   BoolAttr('opaque'),
									   MixedListAttr('segments')
									 ]
	DEFAULT_HEXPAND = False
	DEFAULT_VEXPAND = False

	DEFAULT_RADIUS = 10
	DEFAULT_OPAQUE = False

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
				 opaque = None,
				 center = None,
				 radius = None,
				 segments = None):

		self.real_widget = fifechan.PieGraph()
		self.opaque = self.DEFAULT_OPAQUE
		self.radius = self.DEFAULT_RADIUS
		self._segments = []

		super(PieGraph, self).__init__(parent=parent,
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

		if opaque is not None: self.opaque = opaque
		if center is not None: self.center = center
		if radius is not None: self.radius = radius
		if segments is not None: self.segments = segments


	def clone(self, prefix):
		pieGraphClone = PieGraph(None,
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
					self.opaque,
					self.center,
					self.radius,
					self.segments)
		return pieGraphClone

	def update(self):
		self.real_widget.clearSegments()
		for s in self._segments:
			self.real_widget.addSegment(s.start_angle, s.stop_angle, s.color)
		
	def _setOpaque(self, opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque, _setOpaque)

	def _setCenter(self, center):
		# point check
		if isinstance(center, fifechan.Point):
			self.real_widget.setCenter(center)
			return
		self.real_widget.setCenter(center[0], center[1])
	def _getCenter(self): return (self.real_widget.getCenterX(), self.real_widget.getCenterY())
	center = property(_getCenter, _setCenter)

	def _setRadius(self, radius): self.real_widget.setRadius(radius)
	def _getRadius(self): return self.real_widget.getRadius()
	radius = property(_getRadius, _setRadius)

	def addSegment(self, start, stop, color):
		segment = PieSegment(self, start, stop, color)
		self._segments.append(segment)
		self.real_widget.addSegment(segment.start_angle, segment.stop_angle, segment.color)
		
	def removeSegment(self, start, stop, color):
		tmp_color = color
		if isinstance(color, tuple):
			tmp_color = fifechan.Color(*color)

		for s in self._segments:
			if s.start_angle == start and s.stop_angle == stop and s.color == tmp_color:
				self._segments.remove(s)
				self.update()
				break
	
	def clearSegments(self):
		self._segments = []
		self.real_widget.clearSegments()

	def _setSegments(self, segments):
		self._segments = []
		if segments is None:
			self.real_widget.clearSegments()
			return
		if isinstance(segments[0], PieSegment):
			self._segments = segments
		else:
			for i in range(0, len(segments), 3):
				segment = PieSegment(self, segments[i], segments[i+1], segments[i+2])
				self._segments.append(segment)
		self.update()
	def _getSegments(self): return self._segments
	segments = property(_getSegments, _setSegments)
