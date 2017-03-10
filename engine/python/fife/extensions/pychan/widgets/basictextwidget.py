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

from fife.extensions.pychan.attrs import UnicodeAttr

from common import gui2text, text2gui
from widget import Widget


class BasicTextWidget(Widget):
	"""
	The base class for widgets which display a string - L{Label},L{Button}, etc.
	Do not use directly.

	New Attributes
	==============

	  - text: The text (depends on actual widget)

	Data
	====

	The text can be set via the L{distributeInitialData} method.
	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [ UnicodeAttr('text') ]

	DEFAULT_HEXPAND = True
	DEFAULT_VEXPAND = False
	DEFAULT_MARGINS = 0,0
	DEFAULT_PADDING = 0
	
	DEFAULT_TEXT = u""
	
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
				 text = None):
				 
		self.text = self.DEFAULT_TEXT
		super(BasicTextWidget,self).__init__(parent=parent,
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
		self.accepts_initial_data = True
		self._realSetInitialData = self._setText
		
		# Override anything set when stylize was called
		if margins is not None: self.margins = margins
		if text is not None: self.text = text

	def _getText(self): return gui2text(self.real_widget.getCaption())
	def _setText(self,text): self.real_widget.setCaption(text2gui(text))

	text = property(_getText,_setText)
