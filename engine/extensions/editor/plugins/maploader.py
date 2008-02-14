#!/usr/bin/env python
# coding: utf-8
# Plugin for the editor. See fifedit.py. MapLoader presents a directory browser for finding and loading xml maps.

import sys, os, re

def _jp(path):
	return os.path.sep.join(path.split('/'))

_paths = ('../../engine/swigwrappers/python', '../../engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(_jp(p))

import fife
import fifelog
import plugin
import pychan
import pychan.widgets as widgets
from loaders import loadMapFile
from savers import saveMapFile

class FileBrowser(object):
	def __init__(self, engine,fileSelected, savefile = False):
		self.engine = engine
		self.fileSelected = fileSelected

		self._widget = None
		self.savefile = savefile

		self.path = './..'
		self.dir_list = ('..',) + self.engine.getVFS().listDirectories(self.path)
		self.file_list = self.engine.getVFS().listFiles(self.path)

	def showBrowser(self):
		if self._widget:
			self._widget.show()
			return
		self._widget = pychan.loadXML('content/gui/filebrowser.xml')
		self._widget.mapEvents({
			'dirList'       : self._setDirectory,
			'selectButton'  : self._selectFile,
			'closeButton'   : self._widget.hide
		})
		self._setDirectory()
		if self.savefile:
			self._file_entry = widgets.TextField(name='saveField', text='')	
			self._widget.findChild(name="fileColumn").addChild(self._file_entry)
		self._widget.show()

	def _setDirectory(self):
		selection = self._widget.collectData('dirList')
		if not (selection < 0):
			new_dir = self.dir_list[selection]
			lst = self.path.split('/')
			if new_dir == '..' and lst[-1] != '..' and lst[-1] != '.':
				lst.pop()
			else:
				lst.append(new_dir)
			self.path = '/'.join(lst)

		self.dir_list = ('..',) + filter(lambda d: not d.startswith('.'), self.engine.getVFS().listDirectories(self.path))
		self.file_list = filter(lambda f: f.split('.')[-1] == 'xml', self.engine.getVFS().listFiles(self.path))
		self._widget.distributeInitialData({
			'dirList'  : self.dir_list,
			'fileList' : self.file_list
		})

	def _selectFile(self):
		self._widget.hide()
		selection = self._widget.collectData('fileList')
		if selection >= 0:
			selection = self.file_list[selection]
			if self.savefile:
				newlocation = self._widget.collectData('saveField')
				self.fileSelected(self.path,selection,newlocation)
				return
			self.fileSelected(self.path,selection)


class MapLoader(object):
	def __init__(self, engine):
		super(MapLoader,self).__init__(engine)
		self.engine = engine
		
		self.filebrowser = FileBrowser(engine,self._selectFile)

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

class MapSaver(object):
	def __init__(self, engine):
		super(MapSaver,self).__init__(engine)
		self.engine = engine
		
		self.filebrowser = FileBrowser(engine,self._selectFile,savefile=True)

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
