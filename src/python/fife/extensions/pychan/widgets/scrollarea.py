# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fifechan
from fife.extensions.pychan.attrs import BoolAttr, IntAttr

from .widget import Widget


class ScrollArea(Widget):
    """
    A wrapper around another (content) widget.

    New Attributes
    ==============

      - content: The wrapped widget.
      - vertical_scrollbar: Boolean: Set this to False to hide the Vertical scrollbar
      - horizontal_scrollbar: Boolean: Set this to False to hide the Horizontal scrollbar

    """

    ATTRIBUTES = Widget.ATTRIBUTES + [
        BoolAttr("vertical_scrollbar"),
        BoolAttr("horizontal_scrollbar"),
        IntAttr("vertical_scroll_amount"),
        IntAttr("horizontal_scroll_amount"),
    ]
    DEFAULT_HEXPAND = True
    DEFAULT_VEXPAND = True

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
        content=None,
        vertical_scrollbar=None,
        horizontal_scrollbar=None,
        vertical_scroll_amount=None,
        horizontal_scroll_amount=None,
    ):

        self.real_widget = fifechan.ScrollArea()
        self._content = None
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
        )

        if content is not None:
            self.content = content
        if vertical_scrollbar is not None:
            self.vertical_scrollbar = vertical_scrollbar
        if horizontal_scrollbar is not None:
            self.horizontal_scrollbar = horizontal_scrollbar
        if vertical_scroll_amount is not None:
            self.vertical_scroll_amount = vertical_scroll_amount
        if horizontal_scroll_amount is not None:
            self.horizontal_scroll_amount = horizontal_scroll_amount

    def clone(self, prefix):
        scrollareaClone = ScrollArea(
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
            None,  # NOTE since content is a widget hierarchy it should be cloned too
            self.vertical_scrollbar,
            self.horizontal_scrollbar,
            self.vertical_scroll_amount,
            self.horizontal_scroll_amount,
        )

        scrollareaClone.content = self.content.clone(prefix)

        return scrollareaClone

    def addChild(self, widget):
        self.content = widget
        widget.parent = self

    def removeChild(self, widget):
        if self._content != widget:
            raise RuntimeError(
                f"{str(self)} does not have {str(widget)} as direct child widget."
            )
        self.content = None
        widget.parent = None

    def _setContent(self, content):
        if content is None:
            self.real_widget.setContent(content)
        else:
            self.real_widget.setContent(content.real_widget)
        self._content = content

    def _getContent(self):
        return self._content

    content = property(_getContent, _setContent)

    def deepApply(self, visitorFunc, leaves_first=True, shown_only=False):
        if leaves_first:
            if self._content:
                self._content.deepApply(
                    visitorFunc, leaves_first=leaves_first, shown_only=shown_only
                )
        visitorFunc(self)
        if not leaves_first:
            if self._content:
                self._content.deepApply(
                    visitorFunc, leaves_first=leaves_first, shown_only=shown_only
                )

    def _visibilityToScrollPolicy(self, visibility):
        if visibility:
            return fifechan.ScrollArea.ScrollPolicy_ShowAuto
        return fifechan.ScrollArea.ScrollPolicy_ShowNever

    def _scrollPolicyToVisibility(self, policy):
        if policy == fifechan.ScrollArea.ScrollPolicy_ShowNever:
            return False
        return True

    def _setHorizontalScrollbar(self, visibility):
        self.real_widget.setHorizontalScrollPolicy(
            self._visibilityToScrollPolicy(visibility)
        )

    def _setVerticalScrollbar(self, visibility):
        self.real_widget.setVerticalScrollPolicy(
            self._visibilityToScrollPolicy(visibility)
        )

    def _getHorizontalScrollbar(self):
        return self._scrollPolicyToVisibility(
            self.real_widget.getHorizontalScrollPolicy()
        )

    def _getVerticalScrollbar(self):
        return self._scrollPolicyToVisibility(self.real_widget.getVerticalScrollPolicy())

    def getVerticalMaxScroll(self):
        return self.real_widget.getVerticalMaxScroll()

    def getHorizontalMaxScroll(self):
        return self.real_widget.getHorizontalMaxScroll()

    def _getHorizontalScrollAmount(self):
        return self.real_widget.getHorizontalScrollAmount()

    def _setHorizontalScrollAmount(self, scroll_amount):
        return self.real_widget.setHorizontalScrollAmount(scroll_amount)

    def _getVerticalScrollAmount(self):
        return self.real_widget.getVerticalScrollAmount()

    def _setVerticalScrollAmount(self, scroll_amount):
        return self.real_widget.setVerticalScrollAmount(scroll_amount)

    vertical_scrollbar = property(_getVerticalScrollbar, _setVerticalScrollbar)
    horizontal_scrollbar = property(_getHorizontalScrollbar, _setHorizontalScrollbar)

    horizontal_scroll_amount = property(
        _getHorizontalScrollAmount, _setHorizontalScrollAmount
    )
    vertical_scroll_amount = property(_getVerticalScrollAmount, _setVerticalScrollAmount)
