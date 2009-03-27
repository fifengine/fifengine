# -*- coding: utf-8 -*-

from pychan_test import PyChanExample
import pychan

class SliderExample(PyChanExample):
	def __init__(self):
		super(SliderExample,self).__init__('gui/slider.xml')
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		self.yslider = self.widget.findChild(name="yslider")
		self.xslider = self.widget.findChild(name="xslider")
		self.widget.mapEvents({
			'xslider': self.update,
			'yslider': self.update,
			'closeButton':self.stop,
		})
		self.update()
		self.widget.show()
	def update(self):
		icon = self.widget.findChild(name="icon")
		icon.position = map(int, self.widget.collectData('xslider','yslider'))
		self.widget.distributeInitialData({
			'xvalue' : unicode(icon.x),
			'yvalue' : unicode(icon.y),
		})
	def stop(self):
		self.widget.hide()
