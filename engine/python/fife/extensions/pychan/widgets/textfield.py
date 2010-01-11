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

class TextField(Widget):
	"""
	An editable B{single line} text edit widget.

	New Attributes
	==============

	  - text: The text in the TextBox.

	Data
	====
	The text can be read and set via L{distributeData} and L{collectData}.
	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [UnicodeAttr('text')]
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 0

	def __init__(self,text=u"",**kwargs):
		self.real_widget = fife.TextField()
		self.text = text
		super(TextField,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_data = True
		self.accepts_inital_data = True
		self._realSetInitialData = self._setText
		self._realSetData = self._setText
		self._realGetData = self._getText

	def resizeToContent(self,recurse=True):
		max_w = self.real_font.getWidth(text2gui(self.text))
		self.width = max_w
		self.height = (self.real_font.getHeight() + 2)
	def _getText(self): return gui2text(self.real_widget.getText())
	def _setText(self,text): self.real_widget.setText(text2gui(text))
	text = property(_getText,_setText)

	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)
