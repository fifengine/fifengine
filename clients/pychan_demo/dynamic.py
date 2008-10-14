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

from pychan_test import PyChanExample

class Test(object):
	def __init__(self,button,text1,text2):
		super(Test,self).__init__()
		self.button = button
		self.text1 = text1
		self.text2 = text2
		self.button.text = text1

	def mouseEntered(self, event):
		print event
		self.button.text = self.text2

	def mouseExited(self, event):
		print event
		self.button.text = self.text1


class DynamicExample(PyChanExample):
	def __init__(self):
		super(DynamicExample,self).__init__('gui/dynamic.xml')
		
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		self.l = Test(self.widget.findChild(name="okButton"),"Ok?","Ok!")
		self.widget.mapEvents({
			'okButton'   :self.stop,
			'addButton'  :self.addLabel,
			'okButton/mouseEntered' : self.l.mouseEntered,
			'okButton/mouseExited'  : self.l.mouseExited
		})
		self.labelBox = self.widget.findChild(name="labelBox")
		self.widget.show()

	def addLabel(self):
		# Code to check text wrapping code in FIFE
		label = pychan.widgets.Label(max_size=(200,1000),wrap_text=True)
		label.text = self.widget.collectData('labelText')
		label.capture(self.removeLabel)
		self.labelBox.addChild( label )
		self.widget.adaptLayout()
		
	def removeLabel(self,widget=None):
		widget._parent.removeChild(widget)
		self.widget.adaptLayout()
