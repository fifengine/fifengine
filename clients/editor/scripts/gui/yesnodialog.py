# -*- coding: utf-8 -*

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

import pychan
import pychan.widgets as widgets

class YesNoDialog:
	"""
	Presents a dialog with some text and OK and cancel buttons
	"""
	def __init__(self, prompt, onEntry, onCancel=None):
		self._callback = onEntry
		self._cancelCallback = onCancel

		self._widget = pychan.loadXML('gui/yesnodialog.xml')

		self._widget.mapEvents({
			'okButton'     : self._complete,
			'cancelButton' : self._cancel
		})

		self._widget.distributeInitialData({
			'prompt' : prompt
		})
		self._widget.show()

	def _complete(self):
		self._callback()
		self._widget.hide()
		
	def _cancel(self):
		if self._cancelCallback:
			self._cancelCallback()
		self._widget.hide()


