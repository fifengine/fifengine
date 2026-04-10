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
    bytes or str
        Encoded bytes (when encoding applies) or modified string with tabs and [br] replaced.

    """
    try:
        result = (
            text.encode("utf8", *get_manager().unicodePolicy)
            .replace("\t", " " * 4)
            .replace("[br]", "\n")
        )
        return result
    except TypeError:
        return text.replace("\t", " " * 4).replace("[br]", "\n")


def gui2text(text):
    """
    Convert text from GUI widgets.

    Translates the encoded string into a unicode object.

    Returns
    -------
    str or object
        Decoded unicode string or the original text on failure.

    """
    try:
        result = str(text, "utf8", *get_manager().unicodePolicy)
        return result
    except TypeError:
        return text


def gui2str(text):
    """
    Convert unicode string to 8-bit representation.

    This is useful for passing strings to SWIG functions.

    Returns
    -------
    str or bytes
        String representation or encoded bytes on error.

    """
    try:
        result = text.__str__()
        return result
    except Exception:
        return text.encode("utf-8")
