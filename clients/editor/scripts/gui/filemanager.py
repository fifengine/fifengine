import math, fife, pychan, filebrowser
import loaders, savers
import action
import scripts.editor
import pychan.widgets as widgets

from action import Action, ActionGroup
from input import InputDialog
from selection import SelectionDialog, ClickSelectionDialog

class FileManager(object):
	def __init__(self):
		self.editor = scripts.editor.getEditor()
		self.engine = self.editor.getEngine()
		
		self._filebrowser = None

		newAction = Action(u"New map", "gui/icons/new_map.png")
		loadAction = Action(u"Load", "gui/icons/load_map.png")
		saveAction = Action(u"Save", "gui/icons/save_map.png")
		saveAllAction = Action(u"Save all", "gui/icons/select_layer.png")
		
		newAction.helptext = u"Create new map"
		loadAction.helptext = u"Open map"
		saveAction.helptext = u"(Not implemented) Save map"
		saveAllAction.helptext = u"(Not implemented) Save all maps"
		
		action.activated.connect(self.showMapWizard, sender=newAction)
		action.activated.connect(self.showLoadDialog, sender=loadAction)
		#action.activated.connect(self._actionActivated2, sender=testAction2)
		#action.activated.connect(self._actionActivated2, sender=testAction2)
		#action.activated.connect(self._actionActivated2, sender=testAction2)
		
		fileGroup = ActionGroup()
		fileGroup.addAction(newAction)
		fileGroup.addAction(loadAction)
		fileGroup.addAction(saveAction)
		fileGroup.addAction(saveAllAction)
		
		self.editor.getToolBar().insertAction(fileGroup, 0)
		self.editor.getToolBar().insertSeparator(None, 1)

		self.map = None
		self.newMap = None
		self.saveRequested = False
		self._location = None
		self.path = '.'

	def showLoadDialog(self):
		filebrowser.FileBrowser(self.engine, self.loadFile, extensions = loaders.fileExtensions).showBrowser()
		
	def showSaveDialog(self):
		pass

	def loadFile(self, path, filename):
		self.editor.openFile('/'.join([path, filename]))
		
	def showMapWizard(self):
		def newMap(mapId):
			def newLayer(layerId):
				def newCamera():
					self.newMap = True

				grid = fife.SquareGrid()
				layer = self.map.createLayer(str(layerId), grid)
				grid.thisown = 0

				CameraEditor(self.engine, newCamera, self.map, layer)

			self.map = self.engine.getModel().createMap(str(mapId))
			InputDialog(u'Enter a layer identifier for a default layer:', newLayer)
		
		InputDialog(u'Enter a map identifier:', newMap)
"""
	def save(self):
		self.saveRequested = True
	
	def saveMap(self, map, importList):
		pass
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
"""

class CameraEditor(object):
	"""
	CameraEditor provides a gui dialog for camera creation. The callback is called when camera creation is complete. A
	partial specification of the camera parameters may optionally be given.
	"""
	def __init__(self, engine, callback=None, map=None, layer=None):
		self.engine = engine
		self.callback = callback
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
			'cancelButton' : self._widget.hide
		})

		self._widget.show()

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
