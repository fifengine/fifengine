#!/usr/bin/env python
# coding: utf-8
# Plugin for the editor. See fifedit.py. Importer presents a directory browser for finding and importing data files.

import plugin
import filebrowser
from loaders import loadMapFile
from savers import saveMapFile

class Importer(plugin.Plugin):
	def __init__(self, engine):
		super(Importer,self).__init__()
		self.engine = engine
		
		self.filebrowser = filebrowser.FileBrowser(engine,self._selectFile)

		self.menu_items = {
			'Import' : self.filebrowser.showBrowser,
		}

		self.newImport = None

	def _selectFile(self,path,filename):
		self.newImport = loadImportFile('/'.join([path, filename]))
