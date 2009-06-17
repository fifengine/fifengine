import os, math, fife, pychan, filebrowser, traceback, sys
import loaders, savers
import action
import scripts.editor
import pychan.widgets as widgets
from pychan.tools import callbackWithArguments as cbwa
from scripts.gui.error import ErrorDialog
from action import Action, ActionGroup
from input import InputDialog
from selection import SelectionDialog, ClickSelectionDialog

class FileManager(object):
	def __init__(self):
		self.editor = scripts.editor.getEditor()
		self.engine = self.editor.getEngine()
		self._map = None
		self._layer = None
		self._mapdlg = None
		self._layerdlg = None
		self._cameradlg = None
		
		self._filebrowser = None
		self._importbrowser = None
		self._savebrowser = None

		newAction = Action(u"New map", "gui/icons/new_map.png")
		loadAction = Action(u"Open", "gui/icons/load_map.png")
		saveAction = Action(u"Save", "gui/icons/save_map.png")
		saveAsAction = Action(u"Save as", "gui/icons/save_mapas.png")
		saveAllAction = Action(u"Save all", "gui/icons/save_allmaps.png")
		importFileAction = Action(u"Import file", "gui/icons/import_file.png")
		importDirAction = Action(u"Import directory", "gui/icons/import_dir.png")
		
		newAction.helptext = u"Create new map"
		loadAction.helptext = u"Open existing map"
		saveAction.helptext = u"Save map"
		saveAsAction.helptext = u"Save map as"
		saveAllAction.helptext = u"Save all opened maps"
		importFileAction.helptext = u"Imports an object file"
		importDirAction.helptext = u"Recursively imports all objects from a directory"
		
		action.activated.connect(self.showMapWizard, sender=newAction)
		action.activated.connect(self.showLoadDialog, sender=loadAction)
		action.activated.connect(self.save, sender=saveAction)
		action.activated.connect(self.saveAs, sender=saveAsAction)
		action.activated.connect(self.editor.saveAll, sender=saveAllAction)
		
		self._importFileCallback = cbwa(self.showImportDialog, self.importFile, False)
		self._importDirCallback = cbwa(self.showImportDialog, self.importDir, True)
		action.activated.connect(self._importFileCallback, sender=importFileAction)
		action.activated.connect(self._importDirCallback, sender=importDirAction)
		
		eventlistener = self.editor.getEventListener()
		eventlistener.getKeySequenceSignal(fife.Key.N, ["ctrl"]).connect(self.showMapWizard)
		eventlistener.getKeySequenceSignal(fife.Key.O, ["ctrl"]).connect(self.showLoadDialog)
		eventlistener.getKeySequenceSignal(fife.Key.S, ["ctrl"]).connect(self.save)
		eventlistener.getKeySequenceSignal(fife.Key.S, ["ctrl", "shift"]).connect(self.editor.saveAll)
		
		fileGroup = ActionGroup()
		fileGroup.addAction(newAction)
		fileGroup.addAction(loadAction)
		fileGroup.addAction(saveAction)
		fileGroup.addAction(saveAsAction)
		fileGroup.addAction(saveAllAction)
		fileGroup.addSeparator()
		fileGroup.addAction(importFileAction)
		fileGroup.addAction(importDirAction)
		
		self.editor.getToolBar().insertAction(fileGroup, 0)
		self.editor.getToolBar().insertSeparator(None, 1)
		self.editor._fileMenu.insertAction(fileGroup, 0)
		self.editor._fileMenu.insertSeparator(None, 1)

	def showLoadDialog(self):
		if self._filebrowser is None:
			self._filebrowser = filebrowser.FileBrowser(self.engine, self.loadFile, extensions = loaders.fileExtensions)
		self._filebrowser.showBrowser()
		
	def showSaveDialog(self):
		if self._savebrowser is None:
			self._savebrowser = filebrowser.FileBrowser(self.engine, self.saveFile, savefile=True, extensions = loaders.fileExtensions)
		self._savebrowser.showBrowser()
		
	def showImportDialog(self, callback, selectdir):
		if self._importbrowser is None:
			self._importbrowser = filebrowser.FileBrowser(self.engine, callback, extensions = loaders.fileExtensions)
		self._importbrowser.fileSelected = callback
		self._importbrowser.selectdir = selectdir
		self._importbrowser.showBrowser()

	def saveFile(self, path, filename):
		mapview = self.editor.getActiveMapView()
		if mapview is None:
			print "No map is open"
			return
			
		fname = '/'.join([path, filename])
		mapview.saveAs(fname)
		
	def saveAs(self):
		mapview = self.editor.getActiveMapView()
		if mapview is None:
			print "No map is open"
			return
		self.showSaveDialog(self)
		
	def loadFile(self, path, filename):
		self.editor.openFile('/'.join([path, filename]))
		
	def showMapWizard(self):
		if self._cameradlg:
			self._cameradlg._widget.show()
		elif self._layerdlg:
			self._layerdlg._widget.show()
		elif self._mapdlg:
			self._mapdlg._widget.show()
		else:
			self._newMap()

	def _newMap(self):
		self._mapdlg = InputDialog(u'Enter a map identifier:', self._newLayer, self._clean)
		
	def _newLayer(self, mapId):
		self._map = self.engine.getModel().createMap(str(mapId))
		self._layerdlg = InputDialog(u'Enter a layer identifier for a default layer:', self._newCamera, self._clean)
		
	def _newCamera(self, layerId):
		grid = fife.SquareGrid()
		self._layer = self._map.createLayer(str(layerId), grid)
		grid.thisown = 0
		
		self._cameradlg = CameraEditor(self.engine, self._addMap, self._clean, self._map, self._layer)
		
	def _addMap(self):
		self.editor.newMapView(self._map)
		self._clean()
		
	def _clean(self):
		self._mapdlg = None
		self._layerdlg = None
		self._cameradlg = None
		self._map = None
		self._layer = None

	def save(self):
		curname = None
		mapview = self.editor.getActiveMapView()
		if mapview is None:
			print "No map is open"
			return
		
		try:
			curname = mapview.getMap().getResourceLocation().getFilename()
		except RuntimeError:
			self.showSaveDialog()
			return
		
		mapview.save()
		
	def importFile(self, path, filename):
		file = os.path.normpath(os.path.join(path, filename))
		# FIXME: This is necassary for the files to be loaded properly.
		#		 The loader should be fixed to support native (windows)
		#		 path separators.
		file = file.replace('\\', '/') 
		
		try:
			if os.path.isfile(file):
				loaders.loadImportFile(file, self.engine)
			else:
				raise file+ " is not a file!"
		except:
			traceback.print_exc(sys.exc_info()[1])
			errormsg = u"Importing file failed:\n"
			errormsg += u"File: "+unicode(file)+u"\n"
			errormsg += u"Error: "+unicode(sys.exc_info()[1])
			ErrorDialog(errormsg)
			return None
			
	def importDir(self, path, filename=""):
		if os.path.isdir(os.path.join(path, filename)):
			path = os.path.join(path, filename)
			path = os.path.normpath(path)
		
		# FIXME: This is necassary for the files to be loaded properly.
		#		 The loader should be fixed to support native (windows)
		#		 path separators.
		path = path.replace('\\', '/') 
		
		try:
			if os.path.isdir(path):
				loaders.loadImportDirRec(path, self.engine)
			else:
				raise file+ " is not a directory!"
		except:
			traceback.print_exc(sys.exc_info()[1])
			errormsg = u"Importing directory failed:\n"
			errormsg += u"File: "+unicode(file)+u"\n"
			errormsg += u"Error: "+unicode(sys.exc_info()[1])
			ErrorDialog(errormsg)
			return None

class CameraEditor(object):
	"""
	CameraEditor provides a gui dialog for camera creation. The callback is called when camera creation is complete. A
	partial specification of the camera parameters may optionally be given.
	"""
	def __init__(self, engine, callback=None, onCancel=None, map=None, layer=None):
		self.engine = engine
		self.callback = callback
		self.onCancel = onCancel
		self._widget = pychan.loadXML('gui/cameraedit.xml')

		if map:
			self._widget.distributeData({
				'mapBox'	: unicode(map.getId()),
			})

		if layer:
			self._widget.distributeData({
				'layerBox'	: unicode(layer.getId()),
			})

		self._widget.mapEvents({
			'okButton'     : self._finished,
			'cancelButton' : self._cancelled
		})

		self._widget.show()
		
	def _cancelled(self):
		if self.onCancel:
			self.onCancel()
		self._widget.hide()
		

	def _finished(self):
		id = self._widget.collectData('idBox')
		if id == '':
			print 'Please enter a camera id.'
			return

		try:
			map = self.engine.getModel().getMap(str(self._widget.collectData('mapBox')))
		except fife.Exception:
			print 'Cannot find the specified map id.'
			return

		try:
			layer = map.getLayer(str(self._widget.collectData('layerBox')))
		except fife.Exception:
			print 'Cannot find the specified layer id.'	
			return

		try:
			vals = self._widget.collectData('viewBox').split(',')
			if len(vals) != 4:
				raise ValueError	

			viewport = fife.Rect(*[int(c) for c in vals])
		except ValueError:
			print 'Please enter 4 comma (,) delimited values for viewport x,y,width,height.'
			return

		try:
			refh = int(self._widget.collectData('refhBox'))
			refw = int(self._widget.collectData('refwBox'))
		except ValueError:
			print 'Please enter positive integer values for reference width and height.'
			return

		try:
			rot = int(self._widget.collectData('rotBox'))
			tilt = int(self._widget.collectData('tiltBox'))
		except ValueError:
			print 'Please enter positive integer values for rotation and tilt.'
			return

		cam = self.engine.getView().addCamera(str(id), layer, viewport, fife.ExactModelCoordinate(0,0,0))
		cam.setCellImageDimensions(refw, refh)
		cam.setRotation(rot)
		cam.setTilt(tilt)
	
		self._widget.hide()

		if self.callback:
			self.callback()
