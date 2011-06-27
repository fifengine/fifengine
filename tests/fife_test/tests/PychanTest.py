#!/usr/bin/env python

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

import scripts.test as test

from fife.extensions import pychan


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
		

class SliderExample(PyChanExample):
	def __init__(self):
		super(SliderExample,self).__init__('data/gui/slider.xml')
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

class PychanTest(test.Test):
	""" The base calss for all tests.  All tests must override these functions! """

	def create(self, engine, application):
		self._application = application
		self._engine = engine
		self._running = False
		
		# Init Pychan
		pychan.loadFonts("data/fonts/freefont.fontdef")
		pychan.manager.setDefaultFont("FreeSans")
		pychan.setupModalExecution(self._application.mainLoop, self._application.breakFromMainLoop)

		# Build the main GUI
		self.gui = pychan.loadXML('data/gui/demoapp.xml')
		self.gui.min_size = self._engine.getRenderBackend().getScreenWidth(),self._engine.getRenderBackend().getScreenHeight()

		eventMap = {
			'creditsLink'  : self.showCredits,
			'closeButton'  : self._application.testmanager.stopTest,
			'demoList'     : self.selectExample,
		}
		self.gui.mapEvents(eventMap)
		
		self._examples = {
#			'Absolute Positioning' : PyChanExample('gui/absolute.xml'),
			'All Widgets' : PyChanExample('data/gui/all_widgets.xml'),
#			'Basic Styling' : StylingExample(),
#			'Dynamic Widgets' : DynamicExample(),
			'Sliders' : SliderExample(),
#			'ScrollArea' : PyChanExample('gui/scrollarea.xml'),
#			'Colortester': ColorExample(),
#			'GuiAnimations' : PocAnimations(),
		}
		self.demoList = self.gui.findChild(name='demoList')
		self.demoList.items = sorted(self._examples.keys())
		
		self.currentExample = None

	def selectExample(self):
		"""
		Callback handler for clicking on the example list.
		"""
		if self.demoList.selected_item is None:
			return
		#print "selected",self.demoList.selected_item
		if self.currentExample:
			self.currentExample.stop()
		self.currentExample = self._examples[self.demoList.selected_item]
		self.gui.findChild(name="xmlSource").text = unicode(open(self.currentExample.xmlFile).read(), 'utf8')
		self.currentExample.start()

	def showCredits(self):
		"""
		Callback handler from the credits link/label.
		"""
		# We use PyChan's synchronous execution feature here.
		pychan.loadXML('data/gui/credits.xml').execute({ 'okButton' : "Yay!" })
		
	def destroy(self):
		if self.currentExample:
			self.currentExample.stop()
			
		del self.gui
		
	def run(self):
		self._running = True
		
		self.gui.show()

	def stop(self):
		self._running = False
		
		self.gui.hide()
		
	def isRunning(self):
		return self._running

	def getName(self):
		return "PychanTest"
		
	def getAuthor(self):
		return "prock"

	def getDescription(self):
		return "Use this to test all pychan widgets."

	def getLicense(self):
		return ""

	def getVersion(self):
		return "0.1"

