# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import

from fife import fifechan

from .widget import Widget


class Spacer(Widget):
    """
    A spacer represents expandable or fixed 'whitespace' in the GUI.

    In a XML file you can get this by adding a <Spacer /> inside a VBox or
    HBox element (Windows implicitly are VBox elements).
    """

    DEFAULT_HEXPAND = True
    DEFAULT_VEXPAND = False
    DEFAULT_MARGINS = 0, 0
    DEFAULT_PADDING = 0

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
    ):

        self.real_widget = fifechan.Spacer()
        super(Spacer, self).__init__(
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
        if hexpand is not None:
            self.hexpand = hexpand
        else:
            self.hexpand = self.DEFAULT_HEXPAND
        if vexpand is not None:
            self.vexpand = vexpand
        else:
            self.vexpand = self.DEFAULT_VEXPAND

    def clone(self, prefix):
        spacerClone = Spacer(
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
        )

        return spacerClone
