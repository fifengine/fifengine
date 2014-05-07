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

class TabbedAreaExample(PyChanExample):
	def __init__(self):
		super(TabbedAreaExample,self).__init__('gui/tabbedarea.xml')
	def start(self):
		self.labelCount = 4
		self.widget = pychan.loadXML(self.xmlFile)
		self.tabarea = self.widget.findChild(name='tabarea')
		self.widget.mapEvents({
			'closeButton':self.stop,
		})
		#self.tabarea.layout = 'vertical'

		label = self.tabarea.findChild(name='label1')
		label.tab.mapEvents({
			'close1': self.closeTab,
		})
		label = self.tabarea.findChild(name='label2')
		label.tab.mapEvents({
			'close2': self.closeTab,
		})
		label = self.tabarea.findChild(name='label3')
		label.tab.mapEvents({
			'close3': self.closeTab,
		})
		label = self.tabarea.findChild(name='label4')
		label.tab.mapEvents({
			'create': self.createTab,
		})
		self.widget.show()
		
	def closeTab(self):
		widget = self.tabarea.select_tab
		self.tabarea.removeChild(widget)
		self.tabarea.resizeToContent()

	def createTab(self):
		tmp = self.tabarea.select_tab
		self.tabarea.removeChild(tmp)
		
		widget = pychan.widgets.Label()
		number = str(self.labelCount)
		widget.text = unicode('The '+number+' new Tab.')
		widget.tab = pychan.widgets.Tab()
		content = pychan.widgets.Label()
		content.text = 'Tab '+number
		widget.tab.addChild(content)
		content = pychan.widgets.Button(text='X', name="close"+number, border_size=0)
		widget.tab.addChild(content)
		widget.tab.mapEvents({
			'close'+number: self.closeTab,
		})
		self.tabarea.addChild(widget)
		self.tabarea.addChild(tmp)
		self.labelCount += 1
		self.tabarea.select_tab = widget
		self.tabarea.adaptLayout()
		