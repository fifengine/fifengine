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

class CheckBox(BasicTextWidget):
	"""
	A basic checkbox.

	New Attributes
	==============

	  - marked: Boolean value, whether the checkbox is checked or not.

	Data
	====
	The marked status can be read and set via L{distributeData} and L{collectData}
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [ BoolAttr('marked') ]

	def __init__(self, 
				 parent = None, 
				 name = BasicTextWidget.DEFAULT_NAME,
				 size = BasicTextWidget.DEFAULT_SIZE, 
				 min_size = BasicTextWidget.DEFAULT_MIN_SIZE, 
				 max_size = BasicTextWidget.DEFAULT_MAX_SIZE,
				 helptext = BasicTextWidget.DEFAULT_HELPTEXT, 
				 position = BasicTextWidget.DEFAULT_POSITION,
				 style = None, 
				 hexpand = None, 
				 vexpand = None,
				 text = u"",
				 marked = False):
				 
		self.real_widget = fife.CheckBox()
		self.marked = marked
		super(CheckBox,self).__init__(parent=parent, 
									  name=name, 
									  size=size, 
									  min_size=min_size, 
									  max_size=max_size,
									  helptext=helptext, 
									  position=position,
									  style=style, 
									  hexpand=hexpand, 
									  vexpand=vexpand,
									  text=text)

		# Prepare Data collection framework
		self.accepts_data = True
		self._realGetData = self._isMarked
		self._realSetData = self._setMarked

		# Initial data stuff inherited.

	def _isMarked(self): return self.real_widget.isSelected()
	def _setMarked(self,mark): self.real_widget.setSelected(mark)
	marked = property(_isMarked,_setMarked)
