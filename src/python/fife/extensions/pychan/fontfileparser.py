# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from xml.sax import ContentHandler, ErrorHandler, parse, parseString


class FontFileParser(ContentHandler):
    def __init__(self):
        ContentHandler.__init__(self)
        self._fonts = {}

    def startElement(self, tag, rawAttributes):
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
        return self._fonts.get(font, {}).get(attribute, default)

    def fonts(self):
        return iter(self._fonts.keys())

    def parse(self, filename, errorHandler=ErrorHandler()):
        parse(filename, self, errorHandler)

    def parseString(self, string, errorHandler=ErrorHandler()):
        parseString(string, self, errorHandler)
