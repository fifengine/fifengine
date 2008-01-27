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
from mapeditor import MapEditor

# Help display
class Help(object):
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

		# embed Fifedit tools
		self.fifedit = Fifedit(self.engine)

		# Create this client's modules
		self.mapedit = MapEditor(self.engine)

		# Register plugins with Fifedit.
		self.fifedit.registerPlugin(Help())
		self.fifedit.registerPlugin(MapLoader(self.engine))
		self.fifedit.registerPlugin(self.mapedit)

	def createListener(self):
		# override default event listener
		return EditorListener(self)

	def _pump(self):
		self.mapedit.pump()

if __name__ == '__main__':
	app = Editor()
	app.run()
