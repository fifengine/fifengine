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

from fife.extensions import pychan
from pychan_demo import PyChanExample

class ModalFocusExample(PyChanExample):
		def __init__(self):
			super(ModalFocusExample,self).__init__('gui/modalfocus.xml')
		
		def start(self):
			self.widget = pychan.loadXML(self.xmlFile)
			
			self.window = self.widget
			self.hbox1 = self.widget.findChild(name="hbox1")
			self.hbox2 = self.widget.findChild(name="hbox2")
			self.widgets = [self.widget, self.hbox1, self.hbox2]
			
			eventMap = {
				'closeButton':self.stop,
				'text1': self.requestModalWindow,
				'text2': self.releaseModalWindow,
				'text3': self.requestMouseModalWindow,
				'text4': self.releaseMouseModalWindow,
				'text5': self.requestModalHBox1,
				'text6': self.releaseModalHBox1,
				'text7': self.requestMouseModalHBox1,
				'text8': self.releaseMouseModalHBox1,
				'text9': self.requestModalHBox2,
				'text10': self.releaseModalHBox2,
				'text11': self.requestMouseModalHBox2,
				'text12': self.releaseMouseModalHBox2,
			}
			self.widget.mapEvents(eventMap)
			
			self.widget.show()
			self.updateColor()

		def updateColor(self):
			for widget in self.widgets:
				if widget.isModalFocused():
					if widget.isModalMouseInputFocused():
						widget.base_color = (0, 128, 128)
					else:
						widget.base_color = (0, 128, 0)
				else:
					if widget.isModalMouseInputFocused():
						widget.base_color = (128, 0, 128)
					else:
						widget.base_color = (128, 0, 0)

		def requestModalWindow(self):
			self.window.requestModalFocus()
			self.updateColor()
		
		def releaseModalWindow(self):
			self.window.releaseModalFocus()
			self.updateColor()
			
		def requestMouseModalWindow(self):
			self.window.requestModalMouseInputFocus()
			self.updateColor()
			
		def releaseMouseModalWindow(self):
			self.window.releaseModalMouseInputFocus()
			self.updateColor()

		def requestModalHBox1(self):
			self.hbox1.requestModalFocus()
			self.updateColor()
		
		def releaseModalHBox1(self):
			self.hbox1.releaseModalFocus()
			self.updateColor()
			
		def requestMouseModalHBox1(self):
			self.hbox1.requestModalMouseInputFocus()
			self.updateColor()
			
		def releaseMouseModalHBox1(self):
			self.hbox1.releaseModalMouseInputFocus()
			self.updateColor()

		def requestModalHBox2(self):
			self.hbox2.requestModalFocus()
			self.updateColor()
		
		def releaseModalHBox2(self):
			self.hbox2.releaseModalFocus()
			self.updateColor()
			
		def requestMouseModalHBox2(self):
			self.hbox2.requestModalMouseInputFocus()
			self.updateColor()
			
		def releaseMouseModalHBox2(self):
			self.hbox2.releaseModalMouseInputFocus()
			self.updateColor()
