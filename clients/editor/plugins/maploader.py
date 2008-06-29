#!/usr/bin/env python
# coding: utf-8
# Plugin for the editor. See fifedit.py. MapLoader presents a directory browser for finding and loading xml maps.

import plugin
import filebrowser
import loaders, savers
from loaders import loadMapFile
from savers import saveMapFile

class MapLoader(plugin.Plugin):
	def __init__(self, engine):
		super(MapLoader,self).__init__()
		self.engine = engine
		
		self.filebrowser = filebrowser.FileBrowser(engine,self.loadFile, extensions = loaders.fileExtensions)

		self.menu_items = {
			'Load' : self.filebrowser.showBrowser,
		}
		self.newMap = None

	def loadFile(self, path, filename):
		content = path.split('/')
		self.newMap = loadMapFile('/'.join([path, filename]), self.engine, '/'.join(content) + '/')

class MapSaver(plugin.Plugin):
	def __init__(self, engine):
		super(MapSaver,self).__init__()
		self.engine = engine
		
		self.filebrowser = filebrowser.FileBrowser(engine,self._selectFile,savefile=True, extensions = savers.fileExtensions)

		self.menu_items = {
			'Save' : self.save,
			'Save As' : self.filebrowser.showBrowser,
		}	

		self.saveRequested = False
		self._location = None
		self.path = '.'

	def save(self):
		self.saveRequested = True

	def saveMap(self, map, importList):
		curname = None
		try:
			curname = map.getResourceLocation().getFilename()
		except RuntimeError:
			pass # no name set for map yet
		if self._location:
			fname = '/'.join([self.path, self._location])
			saveMapFile(fname, self.engine, map, importList)
			print "map saved as " + fname
			self._location = None
		elif curname:
			saveMapFile(curname, self.engine, map, importList)
			print "map saved with old name " + curname
		else:
			print 'MapSaver: error, no file location specified.'

	def _selectFile(self,path,filename):
		self._location = filename
		self.path = path
		self.saveRequested = True
