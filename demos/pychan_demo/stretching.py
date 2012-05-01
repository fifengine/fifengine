# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2011 by the FIFE team
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

from pychan_demo import PyChanExample
from fife.extensions import pychan

class StretchingExample(PyChanExample):
	def __init__(self):
		super(StretchingExample,self).__init__('gui/stretching.xml')
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'wslider': self.update,
			'hslider': self.update,
			'closeButton':self.stop,
		})
		self.update()
		self.widget.show()
	def update(self):
		"""
		Update Icon position from the sliders.
		"""
		icon = self.widget.findChild(name="icon")
		# sliders have floats, fifechan is picky and wants ints
		# so we convert here.
		icon.size = map(int, self.widget.collectData('wslider','hslider'))
		# we distribute to the labels with the x,y value.
		# That's user visible 'text' - so pychan wants unicode.
		self.widget.distributeInitialData({
			'wvalue' : unicode(icon.width),
			'hvalue' : unicode(icon.height),
		})
