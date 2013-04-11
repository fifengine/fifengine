#!/usr/bin/env python
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

from ConfigParser import ConfigParser
from sys import argv


def _get(conf, section, part, default=None):
	if not conf.has_option(section, part):
		return default
	return conf.get(section, part)

def convertFontdefToXML(files, printToConsole=False):
	"""
	Converts a list of files containing old pychan font definitions to the new xml format

	@param files: a list of file names to be converted
	@param printToConsole: a boolean controlling if the function prints status messages to the console
	@return: None
	"""
	
	if printToConsole: print "Received", len(files), "files to convert"
	for filename in files:
		if printToConsole: print "Converting", filename
		xmldef = "<fonts>\n"
		fontdef = ConfigParser()
		fontdef.read(filename)

		def get(section, part, default=""):
			return _get(fontdef, section, part, default)

		sections = [section for section in fontdef.sections() if section.startswith("Font/")]

		for section in sections:
			if printToConsole: print "Parsing", section[5:]
			xmldef += '\t<font name=\"' + section[5:] + '"'
			xmldef += ' type="' + get(section, "type") + '"'
			xmldef += ' source="' + get(section, "source") + '"'
			xmldef += ' row_spacing="' + get(section, "row_spacing", "0") + '"'
			xmldef += ' glyph_spacing="' + get(section, "glyph_spacing", "0") + '"'
			if get(section, "type") == "truetype":
				xmldef += ' size="' + get(section, "size") + '"'
				xmldef += ' antialias="' + get(section, "antialias", "1") + '"'
				xmldef += ' color="' + get(section, "color", "255,255,255") + '"'
			xmldef += "/>\n"

		xmldef += "</fonts>\n"

		with open(filename[:-7] + "xml", "w") as f:
			if printToConsole: print "Saving", filename[:-7] + "xml"
			f.write(xmldef)

if __name__=="__main__":
	print "FIFE .fontdef to XML file converter"
	print "==================================="
	print
	if len(argv[1:]) > 0:
		convertFontdefToXML(argv[1:], printToConsole=True)
	else:
		print "Converts old .fontdef files to the new xml file format"
		print "To convert, run the following:"
		print "\tpython", argv[0], "[filenames]"
		print "with the filenames separated by spaces"
		print "It assumes that all input files end with '.fontdef'"
		print "and creates new files with an '.xml' extension. So"
		print "the input file 'foo.fontdef' would create a new output"
		print "'foo.xml' in the same folder. The old fontdef files are"
		print "left untouched."
