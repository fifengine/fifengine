# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import, print_function

from builtins import str

# These used to be defined in here, duplicating the definitions in .layout
# Retain for backwards compatibility of any code importing them from .common


def get_manager():
    from fife.extensions import pychan

    return pychan.manager


def text2gui(text):
    """
    This function is applied to all text set on widgets.
    It replaces tabs by four spaces.
    It assumes the text to be a unicode object.
    """
    try:
        return (
            text.encode("utf8", *get_manager().unicodePolicy)
            .replace("\t", " " * 4)
            .replace("[br]", "\n")
        )
    except TypeError:
        return text.replace("\t", " " * 4).replace("[br]", "\n")


def gui2text(text):
    """
    This function is applied to all text get from widgets.
    Translates the encoded string into a unicode object.
    """
    try:
        return str(text, "utf8", *get_manager().unicodePolicy)
    except TypeError:
        return text


def gui2str(text):
    """
    This function returns an 8-bit representation of the
    unicode string. This is useful for passing strings
    to SWIG functions.
    """
    try:
        return text.__str__()
    except Exception:
        # String contains non-ascii characters
        return text.encode("utf-8")
