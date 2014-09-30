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

from fife.extensions.pychan.attrs import BoolAttr, IntAttr, PointAttr

from widget import Widget


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
									   BoolAttr('opaque')
									 ]
	DEFAULT_HEXPAND = 0
	DEFAULT_VEXPAND = 0

	DEFAULT_RADIUS = 10
	DEFAULT_OPAQUE = False

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
				 opaque = None,
				 center = None,
				 radius = None):

		self.real_widget = fifechan.PieGraph()
		self.opaque = self.DEFAULT_OPAQUE
		self.radius = self.DEFAULT_RADIUS

		super(PieGraph, self).__init__(parent=parent,
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

		if opaque is not None: self.opaque = opaque
		if center is not None: self.center = center
		if radius is not None: self.radius = radius


	def clone(self, prefix):
		pieGraphClone = PieGraph(None,
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
					self.opaque,
					self.center,
					self.radius)
		return pieGraphClone

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
	def _getRadius(self): self.real_widget.getRadius()
	radius = property(_getRadius, _setRadius)

	def addSegment(self, startAngle, stopAngle, color):
		self.real_widget.addSegment(startAngle, stopAngle, color)

	def clearSegments(self):
		self.real_widget.clearSegments()

