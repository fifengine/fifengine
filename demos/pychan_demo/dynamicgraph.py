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
from pychan_demo import PyChanExample
from fife.fifechan import Point
from fife.extensions import pychan

class DynamicGraphExample(PyChanExample):
	def __init__(self):
		super(DynamicGraphExample,self).__init__('gui/dynamicgraph.xml')
	def start(self):
		self.moved_point = None
		self.widget = pychan.loadXML(self.xmlFile)
		self.graph_container = self.widget.findChild(name="graphContainer")
		self.curve_graph = self.graph_container.findChild(name="curveGraph")
		self.line_graph = self.graph_container.findChild(name="lineGraph")
		self.point_graph = self.graph_container.findChild(name="pointGraph")

		self.graph_container.capture(self.onMousePressed, "mousePressed")
		self.graph_container.capture(self.onMouseReleased, "mouseReleased")
		self.graph_container.capture(self.onMouseDragged, "mouseDragged")
		
		self.widget.mapEvents({
			'closeButton': self.stop,
		})

		self.widget.show()

	def onMousePressed(self, event):
		# left button
		if event.getButton() == 1:
			result = self.findPoint(event)
			if result[0] is not None:
				self.moved_point = result[1]
		# right button
		elif event.getButton() == 2:
			result = self.findPoint(event)
			if result[0] is None:
				self.addPoint(event)
			else:
				self.removePoint(result[1])

	def onMouseReleased(self, event):
		self.moved_point = None

	def onMouseDragged(self, event):
		if self.moved_point is None:
			return

		points = self.point_graph.coordinates
		x = event.getX()
		y = event.getY()
		# keep in bounds
		mw = self.graph_container.width - 2 * self.point_graph.thickness
		mh = self.graph_container.height - 2 * self.point_graph.thickness
		if x > mw:
			x = mw
		elif x < 0:
			x = 0
		if y > mh:
			y = mh
		elif y < 0:
			y = 0
			
		points[self.moved_point].x = x
		points[self.moved_point].y = y
		self.setPoints(points)
		
	def findPoint(self, event):
		point = None
		pos = -1
		thickness = self.point_graph.thickness
		points = self.point_graph.coordinates
		rec = fife.Rect(event.getX()-thickness, event.getY()-thickness, thickness*2, thickness*2)
		for i, p in enumerate(points):
			if rec.contains(fife.Point(p.x, p.y)):
				point = p
				pos = i
				break
		return (point, pos)
		
	def addPoint(self, event):
		points = self.point_graph.coordinates
		point = Point(event.getX(), event.getY())
		points.append(point)
		self.setPoints(points)

	def removePoint(self, index):
		points = self.point_graph.coordinates
		del points[index]
		self.setPoints(points)

	def setPoints(self, points):
		self.point_graph.coordinates = points
		self.line_graph.coordinates = points
		self.curve_graph.coordinates = points
