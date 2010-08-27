#!/usr/bin/env python
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

# This is the pychan demo client for FIFE.

import sys, os, re

fife_path = os.path.join('..','..','engine','python')
if os.path.isdir(fife_path) and fife_path not in sys.path:
	sys.path.insert(0,fife_path)

from fife import fife
print "Using the FIFE python module found here: ", os.path.dirname(fife.__file__)

from fife.extensions import fifelog
from fife.extensions import basicapplication
from fife.extensions import pychan
from fife.extensions.pychan.dialogs import trace

class PyChanExample(object):
	"""
	Example class.
	"""
	def __init__(self,xmlFile):
		self.xmlFile = xmlFile
		self.widget = None

	def start(self):
		"""
		The Example Protocoll: start
		"""
		# For simplicity the most basic examples should define
		# a okButton and/or a closeButton. Those are mapped
		# to the stop handler.
		self.widget = pychan.loadXML(self.xmlFile)
		eventMap = {
			'closeButton':self.stop,
			'okButton'   :self.stop
		}
		# Since the basic example are not required to
		# supply close and ok button, we 'ignoreMissing'
		self.widget.mapEvents(eventMap, ignoreMissing = True)
		self.widget.show()

		#from pprint import pprint
		#pprint(self.widget.getNamedChildren())

	def stop(self):
		"""
		The Example Protocoll: stop
		"""
		if self.widget:
			self.widget.hide()
		self.widget = None

class TextSetter(object):
	def __init__(self,text):
		self.text = text
	def __call__(self,widget):
		widget.text = self.text

class DemoApplication(basicapplication.ApplicationBase):
	def __init__(self):
		# Let the ApplicationBase initialise FIFE
		super(DemoApplication,self).__init__()

		# Init Pychan
		pychan.loadFonts("fonts/freefont.fontdef")
		pychan.manager.setDefaultFont("FreeSans")
		pychan.setupModalExecution(self.mainLoop,self.breakFromMainLoop)

		# Build the main GUI
		self.gui = pychan.loadXML('gui/demoapp.xml')
		self.gui.min_size = self.engine.getRenderBackend().getScreenWidth(),self.engine.getRenderBackend().getScreenHeight()

		eventMap = {
			'creditsLink'  : self.showCredits,
			'closeButton'  : self.quit,
			'demoList' : self.selectExample,
		}
		self.gui.mapEvents(eventMap)

		# A simple hover-effect for the credits label
		credits = self.gui.findChild(name="creditsLink")
		# setEnterCallback is deprecated - we use it here to test it.
		credits.setEnterCallback(TextSetter(u"CREDITS"))
		# Note that we can't simply write:
		# credits.capture(credits._setText(u"Credits"), event_name="mouseExited")
		# that's because that would call credits._setText _NOW_ and we want to call
		# it later.
		credits.capture(lambda : credits._setText(u"Credits"), event_name="mouseExited")


		# import example modules
		from dynamic import DynamicExample
		from styling import StylingExample
		from sliders import SliderExample
		from colortester import ColorExample
		from poc_gui_animation import PocAnimations

		# Our list of examples
		# We keep a dictionary of these and fill
		# the ListBox on the left with its names.
		self.examples = {
			'Absolute Positioning' : PyChanExample('gui/absolute.xml'),
			'All Widgets' : PyChanExample('gui/all_widgets.xml'),
			'Basic Styling' : StylingExample(),
			'Dynamic Widgets' : DynamicExample(),
			'Sliders' : SliderExample(),
			'ScrollArea' : PyChanExample('gui/scrollarea.xml'),
			'Colortester': ColorExample(),
			'GuiAnimations' : PocAnimations(),
		}
		self.demoList = self.gui.findChild(name='demoList')
		self.demoList.items = sorted(self.examples.keys())

		# Finally show the main GUI
		self.gui.show()
		
		self.currentExample = None
		self.creditsWidget = None

	# We use the trace decorator which can help debugging the examples.
	# mostly it's for show though :-)
	@trace
	def selectExample(self):
		"""
		Callback handler for clicking on the example list.
		"""
		if self.demoList.selected_item is None:
			return
		#print "selected",self.demoList.selected_item
		if self.currentExample:
			self.currentExample.stop()
		self.currentExample = self.examples[self.demoList.selected_item]
		self.gui.findChild(name="xmlSource").text = unicode(open(self.currentExample.xmlFile).read(), 'utf8')
		self.currentExample.start()

	def showCredits(self):
		"""
		Callback handler from the credits link/label.
		"""
		# We use PyChan's synchronous execution feature here.
		pychan.loadXML('gui/credits.xml').execute({ 'okButton' : "Yay!" })
		
class TestXMLApplication(basicapplication.ApplicationBase):
	"""
	Test Application. Run the pychan_test.py file
	with the XML file you want to load as argument.
	"""
	def __init__(self,xmlfile):
		super(TestXMLApplication,self).__init__()
		pychan.init(self.engine,debug=True)
		self.start()
	@trace
	def start(self):
		self.widget = pychan.loadXML(xmlfile)
		self.widget.show()

if __name__ == '__main__':
	import sys
	if len(sys.argv) == 2:
		app = TestXMLApplication(sys.argv[1])
	else:
		app = DemoApplication()
	app.run()
