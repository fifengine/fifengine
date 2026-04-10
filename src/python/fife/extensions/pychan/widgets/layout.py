# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""Widget module for PyChan - contains the Layout class."""

AlignTop, AlignBottom, AlignLeft, AlignRight, AlignCenter = list(range(5))


def isLayouted(widget):
    """Return whether the given widget is layouted.

    Parameters
    ----------
    widget : Widget
        Widget to query.

    Returns
    -------
    bool
        True if the widget is layouted, False otherwise.
    """
    return widget.real_widget.isLayouted()
