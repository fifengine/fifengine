# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""Widget module for PyChan - contains the Panel class."""

from fife import fifechan
from fife.extensions.pychan.attrs import BoolAttr, IntAttr, UnicodeAttr
from fife.extensions.pychan.exceptions import ParserError

from .containers import Container

#: Maps the visibility_state attribute value onto the fifechan VisibilityState
#: enum. These are the lowercase spellings used in GUI XML files.
VISIBILITY_STATES = {
    "visible": "VisibilityState_Visible",
    "hidden": "VisibilityState_Hidden",
    "collapsed": "VisibilityState_Collapsed",
}


class Panel(Container):
    """A panel with a title bar and an optional close button.

    This wraps the panel widget provided by FifeGUI. For a panel that can be
    docked into a DockArea, see DockPanel.

    Attributes
    ----------
    - title: Text shown in the panel's title bar.
    - closable: If true, the title bar shows a close button.
    - collapsed_width: Width in pixels used while the panel is collapsed.
    - visibility_state: One of visible/hidden/collapsed.
    """

    ATTRIBUTES = Container.ATTRIBUTES + [
        UnicodeAttr("title"),
        BoolAttr("closable"),
        IntAttr("collapsed_width"),
        UnicodeAttr("visibility_state"),
    ]

    DEFAULT_TITLE = ""
    DEFAULT_CLOSABLE = True
    DEFAULT_COLLAPSED_WIDTH = 10
    DEFAULT_VISIBILITY_STATE = "visible"

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
        background_image=None,
        opaque=None,
        layout=None,
        spacing=None,
        uniform_size=None,
        title=None,
        closable=None,
        collapsed_width=None,
        visibility_state=None,
        _real_widget=None,
    ):
        self.real_widget = _real_widget or fifechan.Panel()

        self._title = self.DEFAULT_TITLE
        self._closable = self.DEFAULT_CLOSABLE
        self._collapsed_width = self.DEFAULT_COLLAPSED_WIDTH
        self._visibility_state = self.DEFAULT_VISIBILITY_STATE

        super().__init__(
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
            background_image=background_image,
            opaque=opaque,
            layout=layout,
            spacing=spacing,
            uniform_size=uniform_size,
            _real_widget=self.real_widget,
        )

        self.title = self.DEFAULT_TITLE if title is None else title
        self.closable = self.DEFAULT_CLOSABLE if closable is None else closable
        self.collapsed_width = (
            self.DEFAULT_COLLAPSED_WIDTH if collapsed_width is None else collapsed_width
        )
        self.visibility_state = (
            self.DEFAULT_VISIBILITY_STATE
            if visibility_state is None
            else visibility_state
        )

    def clone(self, prefix):
        """Create a clone of this Panel with a name prefix.

        Returns
        -------
        Panel
            New Panel instance cloned from this one.
        """
        panel_clone = Panel(
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
            self.background_image,
            self.opaque,
            self.layout,
            self.spacing,
            self.uniform_size,
            self._title,
            self._closable,
            self._collapsed_width,
            self._visibility_state,
        )
        return panel_clone

    def _getTitle(self):
        """
        Return the title bar text.

        Returns
        -------
        str
            The current value.
        """
        return self._title

    def _setTitle(self, title):
        self._title = str(title)
        self.real_widget.setTitle(self._title)

    title = property(_getTitle, _setTitle)

    def _getClosable(self):
        """
        Return whether the close button is shown.

        Returns
        -------
        bool
            The current value.
        """
        return self._closable

    def _setClosable(self, closable):
        self._closable = bool(closable)
        self.real_widget.setClosable(self._closable)

    closable = property(_getClosable, _setClosable)

    def _getCollapsedWidth(self):
        """
        Return the width used while the panel is collapsed.

        Returns
        -------
        int
            The current value.
        """
        return self._collapsed_width

    def _setCollapsedWidth(self, width):
        self._collapsed_width = int(width)
        self.real_widget.setCollapsedWidth(self._collapsed_width)

    collapsed_width = property(_getCollapsedWidth, _setCollapsedWidth)

    def _getVisibilityState(self):
        """
        Return the visibility state name.

        Returns
        -------
        str
            The current value.
        """
        return self._visibility_state

    def _setVisibilityState(self, state):
        key = str(state).lower()
        try:
            enum_name = VISIBILITY_STATES[key]
        except KeyError:
            raise ParserError(
                f"Invalid visibility_state '{state}'. "
                f"Must be one of {sorted(VISIBILITY_STATES)}."
            ) from None
        self._visibility_state = key
        self.real_widget.setVisibilityState(getattr(fifechan, enum_name)())

    visibility_state = property(_getVisibilityState, _setVisibilityState)
