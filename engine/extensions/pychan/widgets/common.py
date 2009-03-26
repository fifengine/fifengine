# -*- coding: utf-8 -*-

import fife
import pychan.tools as tools
import pychan.events as events
from pychan.exceptions import *
from pychan.attrs import Attr,UnicodeAttr, PointAttr,ColorAttr,BoolAttr,IntAttr,FloatAttr
from pychan.properties import ColorProperty


def get_manager():
	import pychan
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

def isLayouted(widget):
	from containers import LayoutBase
	return isinstance(widget,LayoutBase)

class DummyImage(object):
	def getWidth(self): return 0
	def getHeight(self): return 0
