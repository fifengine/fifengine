#!/usr/bin/env python
# coding: utf-8
# Plugin for the editor. See fifedit.py. Importer presents a directory browser for finding and importing data files.

import plugin
import filebrowser
from loaders import loadImportFile

class Importer(plugin.Plugin):
	def __init__(self, engine):
		super(Importer,self).__init__()
		self.engine = engine
		
		self.filebrowser = filebrowser.FileBrowser(engine,self._selectFile,selectdir=True)

		self.menu_items = {
			'Import Objects' : self.filebrowser.showBrowser,
		}

		self.newImport = None

	def _selectFile(self,path,filename=None):
		if filename:
			self.newImport = loadImportFile('/'.join([path, filename]), self.engine)
		else:
			for file in filter(lambda f: f.split('.')[-1] == 'xml', self.engine.getVFS().listFiles(path)):
				try:
					loadImportFile('/'.join([path, file]), self.engine)
				except:
					# suppress these errors; we probably just tried to open a file of the wrong type
					# if we're going do this, we probably need to make some modification to the @guarded serializer decorator
					# right now, we get very imprecise exception throws out of it, and a lot of irrelevant error reporting
					continue

			for dir in filter(lambda d: not d.startswith('.'), self.engine.getVFS().listDirectories(path)):
				self._selectFile('/'.join([path, dir]))

