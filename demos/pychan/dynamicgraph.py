# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife
from fife.extensions import pychan
from fife.fifechan import Point
from pychan_demo import PyChanExample


class DynamicGraphExample(PyChanExample):
    def __init__(self):
        super().__init__("gui/dynamicgraph.xml")

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

        self.widget.mapEvents(
            {
                "closeButton": self.stop,
            }
        )

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
        rec = fife.Rect(
            event.getX() - thickness,
            event.getY() - thickness,
            thickness * 2,
            thickness * 2,
        )
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
