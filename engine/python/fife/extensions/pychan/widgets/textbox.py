# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 1

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
