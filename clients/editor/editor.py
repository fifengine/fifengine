#!/usr/bin/env python
# coding: utf-8
# This is the editor client for FIFE.

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
import pychan.widgets as widgets

from maploader import MapLoader
from viewer import Viewer
from listener import EditorListener
from fifedit import Fifedit

# Help display
class Help:
	def __init__(self):
		self.helpWidget = None	
		self.menu_items = { 'Help' : self.showHelp }

	def showHelp(self):
		if self.helpWidget:
			self.helpWidget.show()
			return
		self.helpWidget = pychan.loadXML('content/gui/help.xml')
		self.helpWidget.mapEvents({ 'closeButton' : self.helpWidget.hide })
		self.helpWidget.distributeData({ 'helpText' : open("content/infotext.txt").read() })
		self.helpWidget.show()

class Editor(basicapplication.ApplicationBase):
	def __init__(self):
		super(Editor,self).__init__()

		# override default event listener
		self.listenertype = EditorListener
		
		# embed Fifedit tools
		self.fifedit = Fifedit(self.engine)
		self.fifedit.registerPlugin(Help())
		self.fifedit.registerPlugin(MapLoader(self.engine))

		self.viewer = Viewer(self.engine)
		self.fifedit.registerPlugin(self.viewer)

	def _pump(self):
		self.viewer.pump()
		if self.viewer.new_view:
#			self.fifedit.editMap(self.viewer.map, self.viewer.camera)
			pass

if __name__ == '__main__':
	app = Editor()
	app.run()
