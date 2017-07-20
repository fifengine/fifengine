# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2013 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from xml.sax import ContentHandler, parse, parseString, ErrorHandler


class FontFileParser(ContentHandler):
	def __init__(self):
		ContentHandler.__init__(self)
		self._fonts = {}

	def startElement(self, tag, rawAttributes):
		attributes = dict([(name, rawAttributes.getValue(name)) for name in rawAttributes.getNames()])
		if tag.lower() == "font":
			if not "name" in attributes:
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
