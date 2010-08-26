# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

""" a filebrowser implementation for pychan """

import pychan
import pychan.widgets as widgets
import sys

def u2s(string):
	return string.encode(sys.getfilesystemencoding())

class FileBrowser(object):
	""" The B{FileBrowser} displays directory and file listings from the vfs.
	
		B{The fileSelected} parameter is a callback invoked when a file selection has been made; its
		signature must be fileSelected(path,filename).
		
		B{If selectdir} is set, fileSelected's filename parameter should be optional.
		
		B{The savefile} option provides a box for supplying a new filename that doesn't exist yet.
		
		B{The selectdir} option allows directories to be selected as well as files.
	"""
	def __init__(self, engine, fileSelected, savefile=False, selectdir=False, extensions=('xml',), guixmlpath="gui/filebrowser.xml"):
		"""
		@type	engine:	object
		@param	engine:	initiated instance of FIFE
		@type	fileSelected:	function
		@param	fileSelected:	callback invoked on file selection
		@type	savefile:	bool
		@param	savefile:	flag to provide a gui for an usergiven filename
		@type	selectdir:	bool
		@param	selectdir:	flag to fire fileSelected without filename
		@type	extensions:	tuple
		@param	extensions:	list of extensions the filebrowser should show (defaults to xml)
		@type	guixmlpath:	string
		@param	guixmlpath:	path to the xml guifile defaults to (gui/filebrowser.xml)
		"""
		self.engine = engine
		self.fileSelected = fileSelected

		self._widget = None
		self.savefile = savefile
		self.selectdir = selectdir
		
		self.guixmlpath = guixmlpath

		self.extensions = extensions
		self.path = './..'
		self.dir_list = []
		self.file_list = []

	def showBrowser(self):
		"""	create and / or show the gui """
		if self._widget:
			self.setDirectory(self.path)
			self._widget.show()
			return
		
		self._widget = pychan.loadXML(self.guixmlpath)
		self._widget.mapEvents({
			'dirList'       : self._selectDir,
			'selectButton'  : self._selectFile,
			'closeButton'   : self._widget.hide
		})
		if self.savefile:
			self._file_entry = widgets.TextField(name='saveField', text=u'')	
			self._widget.findChild(name="fileColumn").addChild(self._file_entry)
			
		self.setDirectory(self.path)
		self._widget.show()
		
	def setDirectory(self, path):
		""" sets the current directory according to path """
		path_copy = self.path
		self.path = path
		if not self._widget: return
		
		def decodeList(list):
			fs_encoding = sys.getfilesystemencoding()
			if fs_encoding is None: fs_encoding = "ascii"
		
			newList = []
			for i in list:
				try: newList.append(unicode(i, fs_encoding))
				except:
					newList.append(unicode(i, fs_encoding, 'replace'))
					print "WARNING: Could not decode item:", i
			return newList
	
		dir_list_copy = list(self.dir_list)
		file_list_copy = list(self.file_list)

		self.dir_list = []
		self.file_list = []
		
		try:
			dir_list = ('..',) + filter(lambda d: not d.startswith('.'), self.engine.getVFS().listDirectories(self.path))
			file_list = filter(lambda f: f.split('.')[-1] in self.extensions, self.engine.getVFS().listFiles(self.path))
			self.dir_list = decodeList(dir_list)
			self.file_list = decodeList(file_list)
		except:
			self.path = path_copy
			self.dir_list = list(dir_list_copy)
			self.file_list = list(file_list_copy)
			print "WARNING: Tried to browse to directory that is not accessible!"

		self._widget.distributeInitialData({
			'dirList'  : self.dir_list,
			'fileList' : self.file_list
		})
		
		self._widget.adaptLayout()

	def _selectDir(self):
		""" callback for directory ListBox """
		selection = self._widget.collectData('dirList')
		if selection >= 0 and selection < len(self.dir_list):
			new_dir = u2s(self.dir_list[selection])
			lst = self.path.split('/')
			if new_dir == '..' and lst[-1] != '..' and lst[-1] != '.':
				lst.pop()
			else:
				lst.append(new_dir)
			 
			path = '/'.join(lst)
			self.setDirectory(path)

	def _selectFile(self):
		""" callback for selectButton, hides filebrowser """
		self._widget.hide()
		selection = self._widget.collectData('fileList')

		if self.savefile:
			if self._widget.collectData('saveField'):
				self.fileSelected(self.path, u2s(self._widget.collectData('saveField')))
				return

		if selection >= 0 and selection < len(self.file_list):
			self.fileSelected(self.path, u2s(self.file_list[selection]))
			return
		
		if self.selectdir:
			self.fileSelected(self.path)
			return

		print 'FileBrowser: error, no selection.'
