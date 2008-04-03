#!/usr/bin/env python
# coding: utf-8
# Plugin for the editor. See fifedit.py. Importer presents a directory browser for finding and importing data files.

import plugin
import filebrowser
from loaders import loadImportFile
from loaders import loadImportDirRec

class Importer(plugin.Plugin):
	def __init__(self, engine):
		super(Importer,self).__init__()
		self.engine = engine
		
		self.filebrowser = filebrowser.FileBrowser(engine,self._select,selectdir=True)

		self.menu_items = {
			'Import Objects' : self.filebrowser.showBrowser,
		}

		self.newImport = None
		self.importList = []

	def addDirs(self, dirs):
		self.importList.extend(dirs)

	def _select(self,path,filename=None):
		if filename:
			self.newImport = loadImportFile('/'.join([path, filename]), self.engine)
		else:
			self.importList.append(path)
			loadImportDirRec(path, self.engine)
