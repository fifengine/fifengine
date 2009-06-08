# -*- coding: utf-8 -*-

from pychan_test import PyChanExample
import pychan

class SliderExample(PyChanExample):
	def __init__(self):
		super(SliderExample,self).__init__('gui/slider.xml')
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'xslider': self.update,
			'yslider': self.update,
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
