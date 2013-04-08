__author__ = 'drake'

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
		return self._fonts.iterkeys()

	def parse(self, filename, errorHandler=ErrorHandler()):
		parse(filename, self, errorHandler)

	def parseString(self, string, errorHandler=ErrorHandler()):
		parseString(string, self, errorHandler)