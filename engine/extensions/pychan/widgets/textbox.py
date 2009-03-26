# -*- coding: utf-8 -*-

from common import *
from widget import Widget

class TextBox(Widget):
	"""
	An editable B{multiline} text edit widget.

	New Attributes
	==============

	  - text: The text in the TextBox.
	  - filename: A write-only attribute - assigning a filename will cause the widget to load it's text from it.

	Data
	====
	The text can be read and set via L{distributeData} and L{collectData}.
	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [UnicodeAttr('text'),Attr('filename')]

	def __init__(self,text=u"",filename = "", **kwargs):
		self.real_widget = fife.TextBox()
		self.text = text
		self.filename = filename
		super(TextBox,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_data = True
		self.accepts_initial_data = True # Make sense in a way ...
		self._realSetInitialData = self._setText
		self._realSetData = self._setText
		self._realGetData = self._getText

	def _getFileName(self): return self._filename
	def _loadFromFile(self,filename):
		self._filename = filename
		if not filename: return
		try:
			# FIXME needs encoding detection.
			self.text = unicode(open(filename).read(),"utf8")
		except Exception, e:
			self.text = str(e)
	filename = property(_getFileName, _loadFromFile)

	def resizeToContent(self,recurse=True):
		rows = [self.real_widget.getTextRow(i) for i in range(self.real_widget.getNumberOfRows())]
		max_w = max(map(self.real_font.getWidth,rows))
		self.width = max_w
		self.height = (self.real_font.getHeight() + 2) * self.real_widget.getNumberOfRows()

	def _getText(self): return gui2text(self.real_widget.getText())
	def _setText(self,text): self.real_widget.setText(text2gui(text))
	text = property(_getText,_setText)

	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)
