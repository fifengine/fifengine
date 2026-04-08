# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""
Font management for PyChan.

This module provides the Font class for loading and managing fonts
in PyChan-based GUIs.
"""

from .exceptions import InitializationError
from .fontfileparser import FontFileParser


class Font:
    """
    Represents a font in PyChan.

    Parameters
    ----------
    name : str
        The name of the font.
    get : callable
        A callable that returns font attributes.
    """

    def __init__(self, name, get):
        from .internal import get_manager

        self.font = None
        self.name = name
        self.typename = get("type")
        self.source = str(get("source"))
        self.row_spacing = int(get("row_spacing", 0))
        self.glyph_spacing = int(get("glyph_spacing", 0))

        if self.typename == "truetype":
            self.size = int(get("size"))
            self.antialias = bool(get("antialias", True))
            self.bold = bool(get("bold", False))
            self.italic = bool(get("italic", False))
            self.underline = bool(get("underline", False))
            self.recoloring = bool(get("recoloring", False))
            self.color = list(map(int, get("color", "255,255,255").split(",")))
            self.font = get_manager().createFont(self.source, self.size)

            if self.font is None:
                raise InitializationError(f"Could not load font {name}")

            self.font.setAntiAlias(self.antialias)
            self.font.setBoldStyle(self.bold)
            self.font.setItalicStyle(self.italic)
            self.font.setUnderlineStyle(self.underline)
            self.font.setDynamicColoring(self.recoloring)
            self.font.setColor(*self.color)
        else:
            raise InitializationError(f"Unsupported font type {self.typename}")

        self.font.setRowSpacing(self.row_spacing)
        self.font.setGlyphSpacing(self.glyph_spacing)

    @staticmethod
    def loadFromFile(filename):
        """
        Load font definitions from an XML file.

        Parameters
        ----------
        filename : str
            The file to be loaded.

        Returns
        -------
        list of Font
            A list of new Font objects.
        """
        fontXMLFile = FontFileParser()
        fontXMLFile.parse(filename, fontXMLFile)
        fonts = []
        for font in fontXMLFile.fonts():
            fonts.append(
                Font(font, lambda key, default=None: fontXMLFile.get(font, key, default))
            )
        return fonts

    def __str__(self):
        """
        Return a string representation of the font.

        Returns
        -------
        str
            String representation.
        """
        return f"Font(source='{self.source}')"

    def __repr__(self):
        """
        Return a detailed string representation of the font.

        Returns
        -------
        str
            Detailed representation including memory address.
        """
        return f"<Font(source='{self.source}') at {id(self):x}>"


def loadFonts(filename):
    """Load fonts from a config file. These are then available via their name."""
    from .internal import get_manager

    for font in Font.loadFromFile(filename):
        get_manager().addFont(font)
