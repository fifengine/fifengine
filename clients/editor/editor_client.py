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

from listener import EditorListener

import editor
#from editor.fifedit import Fifedit
from editor.plugins.maploader import MapLoader
from editor.plugins.maploader import MapSaver
from editor.plugins.viewer import Viewer
from editor.plugins.mapeditor import MapEditor

# Help display
class Help(object):
	def __init__(self):
		self._helpWidget = None	
		self.menu_items = { 'Help' : self.showHelp }

	def showHelp(self):
		if self._helpWidget:
			self._helpWidget.show()
			return
		self._helpWidget = pychan.loadXML('content/gui/help.xml')
		self._helpWidget.mapEvents({ 'closeButton' : self._helpWidget.hide })
		self._helpWidget.distributeData({ 'helpText' : open("content/infotext.txt").read() })
		self._helpWidget.show()

class Editor(basicapplication.ApplicationBase):
	def __init__(self):
		super(Editor,self).__init__()

		# embed Fifedit tools
		self.fifedit = editor.Fifedit(self.engine)

		# Create this client's modules
		self.mapedit = MapEditor(self.engine)
		self.maploader = MapLoader(self.engine)
		self.mapsaver = MapSaver(self.engine)

		# Register plugins with Fifedit.
		self.fifedit.registerPlugin(Help())
		self.fifedit.registerPlugin(self.maploader)
		self.fifedit.registerPlugin(self.mapsaver)
		self.fifedit.registerPlugin(self.mapedit)

	def createListener(self):
		# override default event listener
		return EditorListener(self)

	def _pump(self):
		if self.maploader.newMap:
			self.mapedit.editMap(self.maploader.newMap.Id())	
			self.maploader.newMap = None
		if self.mapsaver.saveRequested and self.mapedit.map:
			self.mapsaver.saveMap(self.mapedit.map)
			self.mapsaver.saveRequested = False
		if not self.fifedit.active: self.quitRequested = True
		self.mapedit.pump()

if __name__ == '__main__':
	app = Editor()
	app.run()
