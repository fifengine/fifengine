# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""Widget module for PyChan - contains the Radiobutton class."""

from fife import fifechan
from fife.extensions.pychan.attrs import Attr, BoolAttr

from .basictextwidget import BasicTextWidget


class RadioButton(BasicTextWidget):
    """
    A basic radiobutton (an exclusive checkbox).

    Attributes
    ----------
      - marked: Boolean: Whether the checkbox is checked or not.
      - group: String: All RadioButtons with the same group name
      can only be checked exclusively.

    Data
    ====
    The marked status can be read and set via L{distributeData} and L{collectData}
    """

    ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [BoolAttr("marked"), Attr("group")]
    DEFAULT_GROUP = "_no_group_"

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
        text=None,
        group=None,
    ):

        self.real_widget = fifechan.RadioButton()
        self.group = self.DEFAULT_GROUP

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
            text=text,
        )

        if group is not None:
            self.group = group

        # Prepare Data collection framework
        self.accepts_data = True
        self._realGetData = self._isMarked
        self._realSetData = self._setMarked

        # Initial data stuff inherited.

    def clone(self, prefix):
        """Create a clone of this RadioButton with a name prefix.

        Returns
        -------
        RadioButton
            A new RadioButton instance cloned from this one.
        """
        rbuttonClone = RadioButton(
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
            self.text,
            self.group,
        )

        return rbuttonClone

    def _isMarked(self):
        return self.real_widget.isSelected()

    def _setMarked(self, mark):
        self.real_widget.setSelected(mark)

    marked = property(_isMarked, _setMarked)

    def _setGroup(self, group):
        self.real_widget.setGroup(group)

    def _getGroup(self):
        return self.real_widget.getGroup()

    group = property(_getGroup, _setGroup)
