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

from widget import Widget
from common import *

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
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 0
	
	def __init__(self, 
				 parent = None, 
				 name = Widget.DEFAULT_NAME,
				 size = Widget.DEFAULT_SIZE, 
				 min_size = Widget.DEFAULT_MIN_SIZE, 
				 max_size = Widget.DEFAULT_MAX_SIZE,
				 helptext = Widget.DEFAULT_HELPTEXT, 
				 position = Widget.DEFAULT_POSITION,
				 style = None, 
				 hexpand = None, 
				 vexpand = None,
				 text = u""):
				 
		self.margins = (5,5)
		self.text = text
		super(BasicTextWidget,self).__init__(parent=parent, 
											 name=name, 
											 size=size, 
											 min_size=min_size, 
											 max_size=max_size,
											 helptext=helptext, 
											 position=position,
											 style=style, 
											 hexpand=hexpand, 
											 vexpand=vexpand)

		# Prepare Data collection framework
		self.accepts_initial_data = True
		self._realSetInitialData = self._setText

	def _getText(self): return gui2text(self.real_widget.getCaption())
	def _setText(self,text): self.real_widget.setCaption(text2gui(text))
	text = property(_getText,_setText)

	def resizeToContent(self, recurse = True):
		self.height = self.real_font.getHeight() + self.margins[1]*2
		self.width = self.real_font.getWidth(text2gui(self.text)) + self.margins[0]*2
