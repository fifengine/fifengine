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
from basictextwidget import BasicTextWidget

class Label(BasicTextWidget):
	"""
	A basic label - displaying a string.

	Also allows text wrapping and onMouse hover callbacks.

	New Attributes
	==============

	 - wrap_text: Boolean: Enable/Disable automatic text wrapping. Disabled by default.
	 Currently to actually see text wrapping you have to explicitly set a max_size with
	 the desired width of the text, as the layout engine is not capable of deriving
	 the maximum width from a parent container.

	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [BoolAttr('wrap_text')]

	def __init__(self,wrap_text=False,**kwargs):
		self.real_widget = fife.Label("")
		self.wrap_text = wrap_text
		super(Label,self).__init__(**kwargs)

	def resizeToContent(self, recurse=True):
		self.real_widget.setWidth( self.max_size[0] )
		self.real_widget.adjustSize()
		self.height = self.real_widget.getHeight() + self.margins[1]*2
		self.width  = self.real_widget.getWidth()  + self.margins[0]*2
		#print self.width,self.max_size[0]

	def _setTextWrapping(self,wrapping): self.real_widget.setTextWrapping(wrapping)
	def _getTextWrapping(self): self.real_widget.isTextWrapping()
	wrap_text = property(_getTextWrapping,_setTextWrapping)

class ClickLabel(Label):
	"""
	Deprecated - use L{Label} instead.
	"""
	__init__ = tools.this_is_deprecated(Label.__init__,message = "ClickLabel - Use Label instead")
