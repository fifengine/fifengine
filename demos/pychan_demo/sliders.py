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

from pychan_demo import PyChanExample
from fife.extensions import pychan

class SliderExample(PyChanExample):
	def __init__(self):
		super(SliderExample,self).__init__('gui/slider.xml')
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'xslider': self.update,
			'yslider': self.update,
			'pbarslider' : self.update,
			'closeButton':self.stop,
		})
		self.update()
		self.widget.show()
	def update(self):
		"""
		Update Icon position from the sliders.
		"""
		icon = self.widget.findChild(name="icon")
		# sliders have floats, guichan is picky and wants ints
		# so we convert here.
		icon.position = map(int, self.widget.collectData('xslider','yslider'))
		# we distribute to the labels with the x,y value.
		# That's user visible 'text' - so pychan wants unicode.
		self.widget.distributeInitialData({
			'xvalue' : unicode(icon.x),
			'yvalue' : unicode(icon.y),
		})
		
		#quick demo to show the percentage bar in action
		pbarslider = self.widget.findChild(name="pbarslider")
		pbar = self.widget.findChild(name="pbar")
		
		pbar.value = int(pbarslider.getValue())
