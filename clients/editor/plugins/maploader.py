#!/usr/bin/env python
# coding: utf-8
# Plugin for the editor. See fifedit.py. MapLoader presents a directory browser for finding and loading xml maps.

import plugin
import filebrowser
from loaders import loadMapFile
from savers import saveMapFile

class MapLoader(plugin.Plugin):
	def __init__(self, engine):
		super(MapLoader,self).__init__()
		self.engine = engine
		
		self.filebrowser = filebrowser.FileBrowser(engine,self.loadFile)

		self.menu_items = {
			'Load Map' : self.filebrowser.showBrowser,
		}

		self.newMap = None

	def loadFile(self, path, filename):
		content = path.split('/')
		self.newMap = loadMapFile('/'.join([path, filename]), self.engine, '/'.join(content) + '/')

class MapSaver(plugin.Plugin):
	def __init__(self, engine):
		super(MapSaver,self).__init__()
		self.engine = engine
		
		self.filebrowser = filebrowser.FileBrowser(engine,self._selectFile,savefile=True)

		self.menu_items = {
			'Save Map' : self.filebrowser.showBrowser,
		}	

		self.saveRequested = False
		self._location = None
		self.path = '.'

	def saveMap(self, map):
		if self._location:
			saveMapFile('/'.join([self.path, self._location]), self.engine, map)
		else:
			print 'MapSaver: error, no file location specified.'

	def _selectFile(self,path,filename):
		self._location = filename
		self.path = path
		self.saveRequested = True
