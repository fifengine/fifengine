# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fifechan
from fife.extensions.pychan.attrs import BoolAttr, IntAttr, IntListAttr

from .widget import Widget


class PointGraph(Widget):
    """A point graph widget

    New Attributes
    ==============

      - coordinates: int list: x and y coordinates
      - thickness': int: Line thickness, default 1
      - opaque: bool: default False
    """

    ATTRIBUTES = Widget.ATTRIBUTES + [
        IntListAttr("coordinates"),
        IntAttr("thickness"),
        BoolAttr("opaque"),
    ]
    DEFAULT_HEXPAND = False
    DEFAULT_VEXPAND = False

    DEFAULT_THICKNESS = 1
    DEFAULT_OPAQUE = False

    def __init__(
        self,
        parent=None,
        name=None,
        size=None,
        min_size=None,
        max_size=None,
        fixed_size=None,
        margins=None,
        padding=None,
        helptext=None,
        position=None,
        style=None,
        hexpand=None,
        vexpand=None,
        font=None,
        base_color=None,
        background_color=None,
        foreground_color=None,
        selection_color=None,
        border_color=None,
        outline_color=None,
        border_size=None,
        outline_size=None,
        position_technique=None,
        is_focusable=None,
        comment=None,
        opaque=None,
        coordinates=None,
        thickness=None,
    ):

        self.real_widget = fifechan.PointGraph()
        self.opaque = self.DEFAULT_OPAQUE
        self.thickness = self.DEFAULT_THICKNESS

        super(PointGraph, self).__init__(
            parent=parent,
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
        )

        if opaque is not None:
            self.opaque = opaque
        if coordinates is not None:
            self.coordinates = coordinates
        if thickness is not None:
            self.thickness = thickness

    def clone(self, prefix):
        pointGraphClone = PointGraph(
            None,
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
            self.coordinates,
            self.thickness,
        )
        return pointGraphClone

    def _setOpaque(self, opaque):
        self.real_widget.setOpaque(opaque)

    def _getOpaque(self):
        return self.real_widget.isOpaque()

    opaque = property(_getOpaque, _setOpaque)

    def _setCoordinates(self, coordinates):
        # reset
        if coordinates is None or len(coordinates) == 0:
            self.real_widget.resetPointVector()
            return
        # int list to point vector
        if isinstance(coordinates[0], int):
            coords = fifechan.FcnPointVector()
            for i in range(0, len(coordinates) - 1, 2):
                coords.append(fifechan.Point(coordinates[i], coordinates[i + 1]))
            self.real_widget.setPointVector(coords)
            return
        self.real_widget.setPointVector(coordinates)

    def _getCoordinates(self):
        return self.real_widget.getPointVector()

    coordinates = property(_getCoordinates, _setCoordinates)

    def _setThickness(self, thickness):
        self.real_widget.setThickness(thickness)

    def _getThickness(self):
        return self.real_widget.getThickness()

    thickness = property(_getThickness, _setThickness)
