# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2013 by the FIFE team
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

class ImageProgressBarExample(PyChanExample):
	def __init__(self):
		super(ImageProgressBarExample,self).__init__('gui/imageprogressbar.xml')
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'increaseButton': self.increase,
			'decreaseButton': self.decrease,
			'closeButton': self.stop
		})

		self.widget.show()

	def increase(self):
		print "increase"
		bar = self.widget.findChild(name="bar")
		bar.value = bar.value + 5
		print "value ", bar.value

	def decrease(self):
		print "decrease"
		bar = self.widget.findChild(name="bar")
		bar.value = bar.value - 5
		print "value ", bar.value
