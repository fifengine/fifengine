# coding: utf-8

import pychan
import pychan.widgets as widgets

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

		if self.savefile:
			if self._widget.collectData('saveField'):
				self.fileSelected(self.path,self._widget.collectData('saveField'))
				return

		if selection >= 0:
			self.fileSelected(self.path,self.file_list[selection])
			return
		
		print 'FileBrowser: error, no selection.'
