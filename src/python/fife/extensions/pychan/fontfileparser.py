# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""
Font file parser for PyChan.

This module provides a SAX content handler for parsing font configuration
XML files.
"""

from xml.sax import ContentHandler, ErrorHandler, parse, parseString


class FontFileParser(ContentHandler):
    """
    Parser for font configuration XML files.

    Inherits from xml.sax.ContentHandler to parse font definitions.
    """

    def __init__(self):
        ContentHandler.__init__(self)
        self._fonts = {}

    def startElement(self, tag, rawAttributes):
        """
        Handle the start of an XML element.

        Parameters
        ----------
        tag : str
            The element tag name.
        rawAttributes : Attributes
            The element attributes.
        """
        attributes = {
            name: rawAttributes.getValue(name) for name in rawAttributes.getNames()
        }
        if tag.lower() == "font":
            if "name" not in attributes:
                raise ValueError("Fonts must have a name")
            name = attributes["name"]
            del attributes["name"]
            self._fonts[name] = attributes

    def get(self, font, attribute, default=None):
        """
        Get an attribute value for a font.

        Parameters
        ----------
        font : str
            The name of the font.
        attribute : str
            The attribute to retrieve.
        default : object
            Default value if attribute not found.

        Returns
        -------
        object
            The attribute value or default.
        """
        return self._fonts.get(font, {}).get(attribute, default)

    def fonts(self):
        """
        Get an iterator over all font names.

        Returns
        -------
        iterator
            Iterator over font names.
        """
        return iter(self._fonts.keys())

    def parse(self, filename, errorHandler=ErrorHandler()):
        """
        Parse a font XML file.

        Parameters
        ----------
        filename : str
            Path to the XML file.
        errorHandler : ErrorHandler, optional
            SAX error handler.
        """
        parse(filename, self, errorHandler)

    def parseString(self, string, errorHandler=ErrorHandler()):
        """
        Parse font XML from a string.

        Parameters
        ----------
        string : str
            XML string to parse.
        errorHandler : ErrorHandler, optional
            SAX error handler.
        """
        parseString(string, self, errorHandler)
