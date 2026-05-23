# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""
Common utilities for PyChan widgets.

These were previously defined in here, duplicating the definitions in .layout.
Retained for backwards compatibility of any code importing them from .common.
"""

from fife.extensions.pychan.attrs import (  # noqa: F401
    Attr,
    BoolAttr,
    ColorAttr,
    IntAttr,
    PointAttr,
    UnicodeAttr,
)

from .layout import (  # noqa: F401
    AlignBottom,
    AlignCenter,
    AlignLeft,
    AlignRight,
    AlignTop,
)


def get_manager():
    """Get the PyChan manager instance.

    Returns
    -------
    object
        The global PyChan manager instance.

    """
    from fife.extensions import pychan

    return pychan.manager


def text2gui(text):
    """
    Convert text for use in GUI widgets.

    Replaces tabs by four spaces. Assumes the text is a unicode object.

    Returns
    -------
    str
        Modified string with tabs and [br] replaced.

    """
    return str(text).replace("\t", "    ").replace("[br]", "\n")


def gui2text(text):
    """
    Convert text from GUI widgets.

    Returns
    -------
    str
        The text as a string.

    """
    if isinstance(text, bytes):
        return text.decode("utf-8")
    return str(text)

