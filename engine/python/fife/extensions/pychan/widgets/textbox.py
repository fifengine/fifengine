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

from fife import fifechan

from fife.extensions.pychan.attrs import Attr, UnicodeAttr

from common import text2gui, gui2text
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

	ATTRIBUTES = Widget.ATTRIBUTES + [ UnicodeAttr('text'),
									   Attr('filename')
									 ]
	DEFAULT_HEXPAND = True
	DEFAULT_VEXPAND = True
	DEFAULT_MARGINS = 0, 0
	DEFAULT_PADDING = 0
	DEFAULT_TEXT = u""
	DEFAULT_FILENAME = ""

	def __init__(self,
				 parent = None,
				 name = None,
				 size = None,
				 min_size = None,
				 max_size = None,
				 fixed_size = None,
				 margins = None,
				 padding = None,
				 helptext = None,
				 position = None,
				 style = None,
				 hexpand = None,
				 vexpand = None,
				 font = None,
				 base_color = None,
				 background_color = None,
				 foreground_color = None,
				 selection_color = None,
				 border_color = None,
				 outline_color = None,
				 border_size = None,
				 outline_size = None,
				 position_technique = None,
				 is_focusable = None,
				 comment = None,
				 text = None,
				 filename = None):

		self.real_widget = fifechan.TextBox()
		self.text = text or self.DEFAULT_TEXT
		self.filename = filename or self.DEFAULT_FILENAME
		super(TextBox,self).__init__(parent=parent,
									 name=name,
									 size=size,
									 min_size=min_size,
									 max_size=max_size,
									 fixed_size=fixed_size,
									 margins=margins,
									 padding=padding,
									 helptext=helptext,
									 position=position,
									 style=style,
									 hexpand=hexpand,
									 vexpand=vexpand,
									 font=font,
									 base_color=base_color,
									 background_color=background_color,
									 foreground_color=foreground_color,
									 selection_color=selection_color,
									 border_color=border_color,
									 outline_color=outline_color,
									 border_size=border_size,
									 outline_size=outline_size,
									 position_technique=position_technique,
									 is_focusable=is_focusable,
									 comment=comment)

		# Prepare Data collection framework
		self.accepts_data = True
		self.accepts_initial_data = True # Make sense in a way ...
		self._realSetInitialData = self._setText
		self._realSetData = self._setText
		self._realGetData = self._getText

	def clone(self, prefix):
		textboxClone = TextBox(None,
					self._createNameWithPrefix(prefix),
					self.size,
					self.min_size,
					self.max_size,
					self.fixed_size,
					self.margins,
					self.padding,
					self.helptext,
					self.position,
					self.style,
					self.hexpand,
					self.vexpand,
					self.font,
					self.base_color,
					self.background_color,
					self.foreground_color,
					self.selection_color,
					self.border_color,
					self.outline_color,
					self.border_size,
					self.outline_size,
					self.position_technique,
					self.is_focusable,
					self.comment,
					self.text,
					self.filename)
		return textboxClone


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

	def _getText(self): return gui2text(self.real_widget.getText())
	def _setText(self,text): self.real_widget.setText(text2gui(text))
	text = property(_getText,_setText)

	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)
