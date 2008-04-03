#!/usr/bin/env python
# coding: utf-8
# This is the editor client for FIFE.

import sys, os, re

def _jp(path):
	return os.path.sep.join(path.split('/'))

_paths = ('.', '../../engine/swigwrappers/python', '../../engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(_jp(p))

import fife
import fifelog
import basicapplication
import pychan
import pychan.widgets as widgets
import sys

from listener import EditorListener

from plugins.plugin import Plugin
from plugins.maploader import MapLoader
from plugins.maploader import MapSaver
from plugins.importer import Importer
from plugins.mapeditor import MapEditor
from plugins.mapwizard import MapWizard
from fifedit import Fifedit

# Help display
class Help(Plugin):
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
	def __init__(self, params):
		super(Editor,self).__init__()

		# embed Fifedit tools
		self.fifedit = Fifedit(self.engine)

		# Create this client's modules
		self.mapedit = MapEditor(self.engine)
		self.maploader = MapLoader(self.engine)
		self.mapsaver = MapSaver(self.engine)
		self.mapwizard = MapWizard(self.engine)
		self.importer = Importer(self.engine)

		# Register plugins with Fifedit.
		self.fifedit.registerPlugin(Help())
		self.fifedit.registerPlugin(self.maploader)
		self.fifedit.registerPlugin(self.mapsaver)
		self.fifedit.registerPlugin(self.mapedit)
		self.fifedit.registerPlugin(self.mapwizard)
		self.fifedit.registerPlugin(self.importer)
		
		self.params = params

	def createListener(self):
		# override default event listener
		return EditorListener(self)

	def _pump(self):
		if self.maploader.newMap:
			self.mapedit.editMap(self.maploader.newMap.getId())	
			self.importer.addDirs(self.maploader.newMap.importDirs)
			self.maploader.newMap = None
		if self.mapwizard.newMap:
			self.mapedit.editMap(self.mapwizard.map.getId())
			self.mapwizard.newMap = False
		if self.mapsaver.saveRequested and self.mapedit._map:
			self.mapsaver.saveMap(self.mapedit._map,self.importer.importList)
			self.mapsaver.saveRequested = False
		if not self.fifedit.active: 
			self.quitRequested = True
		if self.params:
			s = os.path.sep
			parts = self.params.split(s)
			self.maploader.loadFile(s.join(parts[0:-1]), parts[-1])
			self.params = None

if __name__ == '__main__':
	print sys.argv
	params = None
	if len(sys.argv) > 1:
		params = sys.argv[1]
	app = Editor(params)
	app.run()
