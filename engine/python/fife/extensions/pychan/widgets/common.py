# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
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

from fife import fife
from fife import fifechan
from fife.extensions.pychan import tools
from fife.extensions.pychan import events
from fife.extensions.pychan.exceptions import *
from fife.extensions.pychan.attrs import Attr,UnicodeAttr, PointAttr,ColorAttr,BoolAttr,IntAttr,FloatAttr,ListAttr
from fife.extensions.pychan.properties import ColorProperty

# These used to be defined in here, duplicating the definitions in .layout
# Retain for backwards compatibility of any code importing them from .common
from layout import AlignTop, AlignBottom, AlignLeft, AlignRight, AlignCenter
from layout import isLayouted


def get_manager():
	from fife.extensions import pychan
	return pychan.manager

def text2gui(text):
	"""
	This function is applied to all text set on widgets.
	It replaces tabs by four spaces.
	It assumes the text to be a unicode object.
	"""
	if not isinstance(text,unicode):
		print "Widget text needs to be set from an unicode object. Got: '%s'" % repr(text)
		text = unicode(text,"utf8")
	return text.encode("utf8",*get_manager().unicodePolicy).replace("\t"," "*4).replace("[br]","\n")

def gui2text(text):
	"""
	This function is applied to all text get from widgets.
	Translates the encoded string into a unicode object.
	"""
	return unicode(text,"utf8",*get_manager().unicodePolicy)
	
def gui2str(text):
	"""
	This function returns an 8-bit representation of the
	unicode string. This is useful for passing strings
	to SWIG functions.
	"""
	try: 
		return text.__str__()
	except:
		# String contains non-ascii characters
		return text.encode("utf-8")
