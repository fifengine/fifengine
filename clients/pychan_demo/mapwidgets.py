#!/usr/bin/env python
# coding: utf-8

import sys, os, re

def _jp(path):
	return os.path.sep.join(path.split('/'))

_paths = ('../../engine/swigwrappers/python', '../../engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(_jp(p))

import fife
import fifelog
import settings

import pychan

from demo import PyChanExample

class MapProperties(PyChanExample):
	def __init__(self):
		super(MapProperties,self).__init__('content/gui/mapproperty.xml')
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'closeButton':self.stop,
			'okButton'   :self.stop
		})
		self.properties =  {
			'Name:' : 'MAP_NAME',
			'Author:' : 'Jack Someone'
		}
		self.widget.findChild(name='propertyList').items = map(str,self.properties.items())
		self.widget.show()

