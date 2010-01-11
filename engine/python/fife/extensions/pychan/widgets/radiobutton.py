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

class RadioButton(BasicTextWidget):
	"""
	A basic radiobutton (an exclusive checkbox).

	New Attributes
	==============

	  - marked: Boolean: Whether the checkbox is checked or not.
	  - group: String: All RadioButtons with the same group name
	  can only be checked exclusively.

	Data
	====
	The marked status can be read and set via L{distributeData} and L{collectData}
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [BoolAttr('marked'),Attr('group')]

	def __init__(self,group="_no_group_",**kwargs):
		self.real_widget = fife.RadioButton()
		super(RadioButton,self).__init__(**kwargs)

		self.group = group

		# Prepare Data collection framework
		self.accepts_data = True
		self._realGetData = self._isMarked
		self._realSetData = self._setMarked

		# Initial data stuff inherited.

	def _isMarked(self): return self.real_widget.isSelected()
	def _setMarked(self,mark): self.real_widget.setSelected(mark)
	marked = property(_isMarked,_setMarked)

	def _setGroup(self,group): self.real_widget.setGroup(group)
	def _getGroup(self): return self.real_widget.getGroup()
	group = property(_getGroup,_setGroup)

	def resizeToContent(self,recurse=True):
		self.width = self.real_font.getWidth(text2gui(self.text)) + 35# Size of the Checked box?
		self.height = self.real_font.getHeight()
