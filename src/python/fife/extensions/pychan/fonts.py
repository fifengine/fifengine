# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import

from builtins import map, object

from future import standard_library

from .exceptions import InitializationError
from .fontfileparser import FontFileParser

standard_library.install_aliases()


class Font(object):
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
            self.color = map(int, get("color", "255,255,255").split(","))
            self.font = get_manager().createFont(self.source, self.size)

            if self.font is None:
                raise InitializationError("Could not load font %s" % name)

            self.font.setAntiAlias(self.antialias)
            self.font.setBoldStyle(self.bold)
            self.font.setItalicStyle(self.italic)
            self.font.setUnderlineStyle(self.underline)
            self.font.setDynamicColoring(self.recoloring)
            self.font.setColor(*self.color)
        else:
            raise InitializationError("Unsupported font type %s" % self.typename)

        self.font.setRowSpacing(self.row_spacing)
        self.font.setGlyphSpacing(self.glyph_spacing)

    @staticmethod
    def loadFromFile(filename):
        """
        Static method to load font definitions out of an xml file.

        @param filename: The file to be loaded
        @param name: (Optional) The name of the font being loaded. If the file definition contains another name, the name from the file definition is used instead.
        @return A new Font object
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
        return "Font(source='%s')" % self.source

    def __repr__(self):
        return "<Font(source='%s') at %x>" % (self.source, id(self))


def loadFonts(filename):
    """
    Load fonts from a config file. These are then available via their name.
    """
    from .internal import get_manager

    for font in Font.loadFromFile(filename):
        get_manager().addFont(font)
