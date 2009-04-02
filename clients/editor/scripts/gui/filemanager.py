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

		newAction = Action(u"New map", "gui/icons/new_map.png")
		loadAction = Action(u"Load", "gui/icons/load_map.png")
		saveAction = Action(u"Save", "gui/icons/save_map.png")
		saveAsAction = Action(u"Save as", "gui/icons/save_map.png")
		saveAllAction = Action(u"Save all", "gui/icons/select_layer.png")
		
		newAction.helptext = u"Create new map"
		loadAction.helptext = u"Open map"
		saveAction.helptext = u"Save map"
		saveAsAction.helptext = u"Save map as"
		saveAllAction.helptext = u"Save all maps"
		
		action.activated.connect(self.showMapWizard, sender=newAction)
		action.activated.connect(self.showLoadDialog, sender=loadAction)
		action.activated.connect(self.save, sender=saveAction)
		action.activated.connect(self.showSaveDialog, sender=saveAsAction)
		action.activated.connect(self.editor.saveAll, sender=saveAllAction)
		
		fileGroup = ActionGroup()
		fileGroup.addAction(newAction)
		fileGroup.addAction(loadAction)
		fileGroup.addAction(saveAction)
		fileGroup.addAction(saveAsAction)
		fileGroup.addAction(saveAllAction)
		
		self.editor.getToolBar().insertAction(fileGroup, 0)
		self.editor.getToolBar().insertSeparator(None, 1)

	def showLoadDialog(self):
		filebrowser.FileBrowser(self.engine, self.loadFile, extensions = loaders.fileExtensions).showBrowser()
		
	def showSaveDialog(self):
		filebrowser.FileBrowser(self.engine, self.saveFile, savefile=True, extensions = loaders.fileExtensions).showBrowser()

	def saveFile(self, path, filename):
		mapview = self.editor.getActiveMapView()
		if mapview is None:
			print "No map is open"
			return
			
		fname = '/'.join([path, filename])
		mapview.saveAs(fname)
		
	def loadFile(self, path, filename):
		self.editor.openFile('/'.join([path, filename]))
		
	def showMapWizard(self):
		map = None
		def newMap(mapId):
			def newLayer(layerId):
				def newCamera():
					self.editor.newMapView(map)

				grid = fife.SquareGrid()
				layer = map.createLayer(str(layerId), grid)
				grid.thisown = 0

				CameraEditor(self.engine, newCamera, map, layer)

			map = self.engine.getModel().createMap(str(mapId))
			InputDialog(u'Enter a layer identifier for a default layer:', newLayer)
		
		InputDialog(u'Enter a map identifier:', newMap)

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
