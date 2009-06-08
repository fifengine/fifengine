# -*- coding: utf-8 -*-
from widget import Widget
from common import *

class BasicTextWidget(Widget):
	"""
	The base class for widgets which display a string - L{Label},L{ClickLabel},L{Button}, etc.
	Do not use directly.

	New Attributes
	==============

	  - text: The text (depends on actual widget)

	Data
	====

	The text can be set via the L{distributeInitialData} method.
	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [UnicodeAttr('text')]
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 0

	def __init__(self, text = u"",**kwargs):
		self.margins = (5,5)
		self.text = text
		super(BasicTextWidget,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_initial_data = True
		self._realSetInitialData = self._setText

	def _getText(self): return gui2text(self.real_widget.getCaption())
	def _setText(self,text): self.real_widget.setCaption(text2gui(text))
	text = property(_getText,_setText)

	def resizeToContent(self, recurse = True):
		self.height = self.real_font.getHeight() + self.margins[1]*2
		self.width = self.real_font.getWidth(text2gui(self.text)) + self.margins[0]*2
