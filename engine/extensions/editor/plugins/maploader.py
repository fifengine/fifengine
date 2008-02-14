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
		
		self.filebrowser = filebrowser.FileBrowser(engine,self._selectFile)

		self.menu_items = {
			'Load Map' : self.filebrowser.showBrowser,
		}

		self.newMap = None

	def _selectFile(self,path,filename):
		# assumes the root/content/{map|datasets|...} directory structure. (And is rather fragile)
		content = path.split('/')
		content.pop()
		content.pop()
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
		self._newlocation = None

	def saveMap(self, map):
		print self
		if self._newlocation:
			saveMapFile('/'.join([self.path, self._newlocation]), self.engine, map)
		elif self._location:
			saveMapFile('/'.join([self.path, self._location]), self.engine, map)

	def _selectFile(self,path,filename,newlocation):
		self._location = filename
		self._newlocation= newlocation
		self.path = path
		self.saveRequested = True
