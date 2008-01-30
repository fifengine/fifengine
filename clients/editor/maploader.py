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
import pychan
import pychan.widgets as widgets
from loaders import loadMapFile
from savers import saveMapFile

class FileBrowser(object):
	def __init__(self, engine):
		self.engine = engine

		self._widget = None

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
		self._widget.show()

	def _setDirectory(self):
		selection = self._widget.collectData(['dirList'])['dirList']
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

	def getSelection(self):
		selection = self._widget.collectData(['fileList'])['fileList']
		if selection < 0: return
		return self.file_list[self._widget.collectData(['fileList'])['fileList']]

class MapLoader(FileBrowser):
	def __init__(self, engine):
		FileBrowser.__init__(self, engine)

		self.menu_items = {
			'Load Map' : self.showBrowser,
		}

		self.newMap = None

	def _selectFile(self):
		selection = self.getSelection()
		if not selection: return

		# assumes the root/content/{map|datasets|...} directory structure. (And is rather fragile)
		content = self.path.split('/')
		content.pop()
		content.pop()
		self.newMap = loadMapFile('/'.join([self.path, selection]), self.engine, '/'.join(content) + '/')
		self._widget.hide()

class MapSaver(FileBrowser):
	def __init__(self, engine):
		FileBrowser.__init__(self, engine)

		self.menu_items = {
			'Save Map' : self.showBrowser,
		}	

		self.saveRequested = False
		self._location = None

	def showBrowser(self):
		if not self._widget:
			FileBrowser.showBrowser(self)
			self._file_entry = widgets.TextField(name='saveField', text='')	
			self._widget.findChild(name="fileColumn").add(self._file_entry)
			self._widget.adaptLayout()
		else:
			FileBrowser.showBrowser(self)

	def saveMap(self, map):
		newlocation = self._widget.collectData(['saveField'])['saveField']
		if newlocation:
			saveMapFile('/'.join([self.path, newlocation]), self.engine, map)
		elif _location:
			saveMapFile('/'.join([self.path, self._location]), self.engine, map)

	def _selectFile(self):
		self._location = self.getSelection()
		self.saveRequested = True
		self._widget.hide()
