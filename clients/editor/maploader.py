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
from loaders import loadMapFile

class MapLoader:
	def __init__(self, engine):
		self.engine = engine

		self.maploadWidget = None

		self.path = '.'
		self.dir_list = ('..',) + self.engine.getVFS().listDirectories(self.path)
		self.file_list = self.engine.getVFS().listFiles(self.path)

		self.menu_items = {
			'Load Map' : self.showMapBrowser,
		}

	def showMapBrowser(self):
		if self.maploadWidget:
			self.maploadWidget.show()
			return
		self.maploadWidget = pychan.loadXML('content/gui/filebrowser.xml')
		self.maploadWidget.mapEvents({
			'dirList'     : self._setDirectory,
			'openButton'  : self._openFile,
			'closeButton' : self.maploadWidget.hide
		})
		self._setDirectory()
		self.maploadWidget.show()

	def _setDirectory(self):
		selection = self.maploadWidget.collectData(['dirList'])['dirList']
		if not (selection < 0):
			new_dir = self.dir_list[selection]
			lst = self.path.split('/')
			if new_dir == '..' and lst[-1] != '..' and lst[-1] != '.':
				lst.pop()
			else:
				lst.append(new_dir)
			self.path = '/'.join(lst)

		self.dir_list = ('..',) + self.engine.getVFS().listDirectories(self.path)
		self.file_list = filter(lambda f: f.split('.')[-1] == 'xml', self.engine.getVFS().listFiles(self.path))
		self.maploadWidget.distributeInitialData({
			'dirList'  : self.dir_list,
			'fileList' : self.file_list
		})

	def _openFile(self):
		selection = self.maploadWidget.collectData(['fileList'])['fileList']
		if selection < 0:
			return

		# assumes the root/content/{map|datasets|...} directory structure. (And is rather fragile)
		content = self.path.split('/')
		content.pop()
		content.pop()
		loadMapFile('/'.join([self.path, self.file_list[selection]]), self.engine, '/'.join(content) + '/')
		self.maploadWidget.hide()
