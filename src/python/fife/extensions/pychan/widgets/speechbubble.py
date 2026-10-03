# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""Widget module for PyChan - contains the SpeechBubble class."""

from fife import fifechan
from fife.extensions.pychan.attrs import ColorAttr, FloatAttr, IntAttr, UnicodeAttr
from fife.extensions.pychan.exceptions import ParserError

from .containers import Container

#: Maps the tail_profile attribute value onto the fifechan TailProfile factory.
#: These are the lowercase spellings used in GUI XML files.
TAIL_PROFILES = {
    "sharp": "sharp",
    "rounded": "rounded",
    "curved": "curved",
    "wide": "wide",
    "jagged": "jagged",
    "pronged": "pronged",
}

#: Maps the bubble_style attribute value onto the fifechan BubbleStyle enum.
#: The names mirror FIFE::BubbleType (SpeechStyle.bubbleType), which uses the
#: same ordering, so both bubble APIs share one vocabulary.
BUBBLE_STYLES = {
    "classic": "BubbleStyle_Classic",
    "round": "BubbleStyle_Round",
    "thought": "BubbleStyle_Thought",
    "shout": "BubbleStyle_Shout",
    "whisper": "BubbleStyle_Whisper",
}

#: Maps the tail_direction attribute value onto the fifechan TailDirection enum.
TAIL_DIRECTIONS = {
    "none": "TailDirection__None",
    "up": "TailDirection_Up",
    "down": "TailDirection_Down",
    "left": "TailDirection_Left",
    "right": "TailDirection_Right",
    "bottomleft": "TailDirection_BottomLeft",
    "bottomright": "TailDirection_BottomRight",
    "topleft": "TailDirection_TopLeft",
    "topright": "TailDirection_TopRight",
    "auto": "TailDirection_Auto",
}


class SpeechBubble(Container):
    """A comic speech bubble that can hold child widgets.

    Attributes
    ----------
    - corner_radius: Rounding radius of the bubble body corners.
    - tail_width: Width of the tail at its base.
    - tail_height: Length of the tail.
    - tail_direction: One of none/up/down/left/right/bottomleft/bottomright/
        topleft/topright/auto.
    - bubble_style: One of classic/round/thought/shout/whisper.
    - tail_profile: One of sharp/rounded/curved/wide/jagged/pronged.
    - tail_color: Optional r,g,b,a colour; defaults to the base colour.
    - tail_offset: Horizontal (0..1) placement of the tail along the bubble edge.
    """

    ATTRIBUTES = Container.ATTRIBUTES + [
        IntAttr("corner_radius"),
        IntAttr("tail_width"),
        IntAttr("tail_height"),
        UnicodeAttr("tail_direction"),
        UnicodeAttr("bubble_style"),
        UnicodeAttr("tail_profile"),
        ColorAttr("tail_color"),
        FloatAttr("tail_offset"),
    ]

    DEFAULT_CORNER_RADIUS = 10
    DEFAULT_TAIL_WIDTH = 16
    DEFAULT_TAIL_HEIGHT = 12
    DEFAULT_TAIL_DIRECTION = "Auto"
    DEFAULT_BUBBLE_STYLE = "Classic"
    DEFAULT_TAIL_PROFILE = "sharp"
    DEFAULT_TAIL_COLOR = None
    DEFAULT_TAIL_OFFSET = 0.5

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
        corner_radius=None,
        tail_width=None,
        tail_height=None,
        tail_direction=None,
        bubble_style=None,
        tail_profile=None,
        tail_color=None,
        tail_offset=None,
        _real_widget=None,
    ):
        self.real_widget = _real_widget or fifechan.SpeechBubble()

        self._corner_radius = self.DEFAULT_CORNER_RADIUS
        self._tail_width = self.DEFAULT_TAIL_WIDTH
        self._tail_height = self.DEFAULT_TAIL_HEIGHT
        self._tail_direction = self.DEFAULT_TAIL_DIRECTION
        self._bubble_style = self.DEFAULT_BUBBLE_STYLE
        self._tail_profile = self.DEFAULT_TAIL_PROFILE
        self._tail_color = self.DEFAULT_TAIL_COLOR
        self._tail_offset = self.DEFAULT_TAIL_OFFSET

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

        self.bubble_style = bubble_style or self.DEFAULT_BUBBLE_STYLE
        self.tail_direction = tail_direction or self.DEFAULT_TAIL_DIRECTION
        self.tail_profile = tail_profile or self.DEFAULT_TAIL_PROFILE
        self.corner_radius = (
            self.DEFAULT_CORNER_RADIUS if corner_radius is None else corner_radius
        )
        self.tail_width = self.DEFAULT_TAIL_WIDTH if tail_width is None else tail_width
        self.tail_height = (
            self.DEFAULT_TAIL_HEIGHT if tail_height is None else tail_height
        )
        self.tail_offset = (
            self.DEFAULT_TAIL_OFFSET if tail_offset is None else tail_offset
        )
        if tail_color is not None:
            self.tail_color = tail_color

    def clone(self, prefix):
        """Create a clone of this SpeechBubble with a name prefix.

        Returns
        -------
        SpeechBubble
            New SpeechBubble instance cloned from this one.
        """
        bubble_clone = SpeechBubble(
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
            self._corner_radius,
            self._tail_width,
            self._tail_height,
            self._tail_direction,
            self._bubble_style,
            self._tail_profile,
            self._tail_color,
            self._tail_offset,
        )
        bubble_clone.addChildren(self._cloneChildren(prefix))
        return bubble_clone

    def _getCornerRadius(self):
        """
        Return the corner radius in pixels.

        Returns
        -------
        int
            The current value.
        """
        return self._corner_radius

    def _setCornerRadius(self, radius):
        self._corner_radius = int(radius)
        self.real_widget.setCornerRadius(self._corner_radius)

    corner_radius = property(_getCornerRadius, _setCornerRadius)

    def _getTailWidth(self):
        """
        Return the tail base width in pixels.

        Returns
        -------
        int
            The current value.
        """
        return self._tail_width

    def _setTailWidth(self, width):
        self._tail_width = int(width)
        self.real_widget.setTailWidth(self._tail_width)

    tail_width = property(_getTailWidth, _setTailWidth)

    def _getTailHeight(self):
        """
        Return the tail length in pixels.

        Returns
        -------
        int
            The current value.
        """
        return self._tail_height

    def _setTailHeight(self, height):
        self._tail_height = int(height)
        self.real_widget.setTailHeight(self._tail_height)

    tail_height = property(_getTailHeight, _setTailHeight)

    def _getTailDirection(self):
        """
        Return the tail direction name.

        Returns
        -------
        str
            The current value.
        """
        return self._tail_direction

    def _lookup(self, table, key, value, attribute):
        """Resolve a config value to its enum attribute name.

        Returns
        -------
        str
            The fifechan enum attribute name.

        Raises
        ------
        ParserError
            If the value is not a known option for the attribute.
        """
        name = table.get(key)
        if name is None:
            raise ParserError(
                f"Unknown {attribute} '{value}', expected one of: {', '.join(sorted(table))}"
            )
        return name

    def _setTailDirection(self, direction):
        key = str(direction).lower().replace("_", "")
        self._tail_direction = self._lookup(
            TAIL_DIRECTIONS, key, direction, "tail_direction"
        )
        self.real_widget.setTailDirection(
            getattr(fifechan.SpeechBubble, self._tail_direction),
        )

    tail_direction = property(_getTailDirection, _setTailDirection)

    def _getBubbleStyle(self):
        """
        Return the bubble style name.

        Returns
        -------
        str
            The current value.
        """
        return self._bubble_style

    def _setBubbleStyle(self, style):
        key = str(style).lower()
        self._bubble_style = self._lookup(BUBBLE_STYLES, key, style, "bubble_style")
        self.real_widget.setBubbleStyle(
            getattr(fifechan.SpeechBubble, self._bubble_style),
        )

    bubble_style = property(_getBubbleStyle, _setBubbleStyle)

    def _getTailProfile(self):
        """
        Return the tail profile preset name.

        Returns
        -------
        str
            The current value.
        """
        return self._tail_profile

    def _setTailProfile(self, profile):
        key = str(profile).lower()
        self._tail_profile = self._lookup(TAIL_PROFILES, key, profile, "tail_profile")
        self.real_widget.setTailProfile(
            getattr(fifechan.TailProfile, self._tail_profile)(),
        )

    tail_profile = property(_getTailProfile, _setTailProfile)

    def _getTailColor(self):
        """
        Return the tail colour, or None when it follows the base colour.

        Returns
        -------
        tuple | None
            The current value.
        """
        return self._tail_color

    def _setTailColor(self, color):
        self._tail_color = color
        if color is None:
            return
        self.real_widget.setTailColor(
            fifechan.Color(
                color[0], color[1], color[2], color[3] if len(color) > 3 else 255
            ),
        )

    tail_color = property(_getTailColor, _setTailColor)

    def _getTailOffset(self):
        """
        Return the tail placement along the bubble edge (0..1).

        Returns
        -------
        float
            The current value.
        """
        return self._tail_offset

    def _setTailOffset(self, offset):
        self._tail_offset = float(offset)
        self.real_widget.setTailOffset(self._tail_offset)

    tail_offset = property(_getTailOffset, _setTailOffset)
