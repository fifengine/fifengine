#!/usr/bin/env python
# -*- coding: utf-8 -*-
# coding: utf-8
# This is the pychan demo client for FIFE.

import sys, os, re

def _jp(path):
	return os.path.sep.join(path.split('/'))

_paths = ('../../engine/swigwrappers/python', '../../engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(_jp(p))

import fife
import fifelog
import basicapplication
import pychan

class PyChanExample(object):
	def __init__(self,xmlFile):
		self.xmlFile = xmlFile
		self.widget = None
	
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		eventMap = {
			'closeButton':self.stop,
			'okButton'   :self.stop
		}
		self.widget.mapEvents(eventMap, ignoreMissing = True)
		self.widget.show()

	def stop(self):
		if self.widget:
			self.widget.hide()
		self.widget = None

def testTimer():
	import timer
	timer.init( pychan.manager.hook.engine.getTimeManager() )
	def spam():
		print "SPAM SPAM"
		return 1
	repeater = timer.repeatCall(500,spam)
	def stop_spam():
		repeater.stop()
		print "BACON EGGS AND SPAM"
	timer.delayCall(5000,stop_spam)


class DemoApplication(basicapplication.ApplicationBase):
	def __init__(self):
		super(DemoApplication,self).__init__()
		
		pychan.init(self.engine,debug=True)
		pychan.loadFonts("fonts/freefont.fontdef")
		pychan.manager.setDefaultFont("FreeSans")
		pychan.setupModalExecution(self.mainLoop,self.breakFromMainLoop)
		
		self.gui = pychan.loadXML('gui/demoapp.xml')
		self.gui.findChild(name="xmlSource").font = "FreeMono"
		
		eventMap = {
			'creditsLink'  : self.showCredits,
			'closeButton'  : self.quit,
			'demoList' : self.selectExample,
			'slider': self.test_slider
		}
		self.gui.mapEvents(eventMap)
		credits = self.gui.findChild(name="creditsLink")
		credits.setEnterCallback(lambda w : credits._setText("CREDITS"))
		credits.capture(lambda : credits._setText("Credits"), event_name="mouseExited")

		from dynamic import DynamicExample
		from styling import StylingExample
		
		self.examples = {
			'Absolute Positioning' : PyChanExample('gui/absolute.xml'),
			'All Widgets' : PyChanExample('gui/all_widgets.xml'),
			'Basic Styling' : StylingExample(),
			'Dynamic Widgets' : DynamicExample(),
			'ScrollArea' : PyChanExample('gui/scrollarea.xml'),
		}
		self.demoList = self.gui.findChild(name='demoList')
		self.demoList.items += self.examples.keys()
		self.gui.show()
		
		self.slider = self.gui.findChild(name='slider')
		self.slider_value = self.gui.findChild(name='slider_value')
		
		self.currentExample = None
		self.creditsWidget = None
		testTimer()

	def selectExample(self):
		if self.demoList.selected_item is None: return
		print "selected",self.demoList.selected_item
		if self.currentExample: self.currentExample.stop()
		self.currentExample = self.examples[self.demoList.selected_item]
		self.gui.findChild(name="xmlSource").text = open(self.currentExample.xmlFile).read()
		self.currentExample.start()

	def showCredits(self):
		print pychan.loadXML('gui/credits.xml').execute({ 'okButton' : "Yay!" })
	def test_slider(self):
		self.slider_value._setText( str(self.slider.getValue()) )

class TestXMLApplication(basicapplication.ApplicationBase):
	"""
	Test Application. Run the pychan_test.py file
	with the XML file you want to load as argument.
	"""
	def __init__(self,xmlfile):
		super(TestXMLApplication,self).__init__()
		pychan.init(self.engine,debug=True)
		self.widget = pychan.loadXML(xmlfile)
		self.widget.show()

if __name__ == '__main__':
	import sys
	if len(sys.argv) == 2:
		app = TestXMLApplication(sys.argv[1])
	else:
		app = DemoApplication()
	app.run()
