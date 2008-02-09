# MapEditor is a plugin for Fifedit. It allows for selection and visual editing of maps.
# MapEditor must be pumped (see pump).

import math

import fife
import pychan
import pychan.widgets as widgets
from viewer import Viewer
from datasetselector import DatasetSelector
from editor.selection import Selection, ClickSelection

class MapEditor(fife.IMouseListener, fife.IKeyListener):
	def __init__(self, engine):
		self.engine = engine
		self.eventmanager = self.engine.getEventManager()

		fife.IMouseListener.__init__(self)
		self.eventmanager.addMouseListener(self)
		fife.IKeyListener.__init__(self)
		self.eventmanager.addKeyListener(self)

		# Fifedit plugin data
		self.menu_items = { 'Edit' : self._selectMap }

		# MapEditor needs a viewer so the user can see things.
		self.viewer = Viewer(self.engine)

		self.mapSelect = None
		self.datSelect = None

		self.mapEdit = None
		self.camera = None
		self.map = None
		self.layer = None
		self.selection = None

		self.dataset = None
		self.object = None

		self.insertmode = False
		self.datasetSelector = None

	# gui for selecting a map
	def _selectMap(self):
		Selection([map.Id() for map in self.engine.getModel().getMaps()], self.editMap)

	def editMap(self, mapid):
		self.viewer.viewMap(mapid)
		self.map = self.engine.getModel().getMaps('id', mapid)[0]
		self.camera = self.viewer.camera
		self.layer = self.camera.getLocation().getLayer()

		if not self.mapEdit:
			self.mapEdit = pychan.loadXML('content/gui/mapeditor.xml')
			self.mapEdit.mapEvents({
				'layerButton'  : self._selectLayer,
				'datButton'   : self._selectDataset,
				'closeButton' : self.quit
			})

		metafields = self.mapEdit.findChild(name='Metadata Properties')
		for metafield in self.map.listFields():
			hbox = widgets.HBox()
			metafields.add(hbox)

			label = widgets.Label(text=metafield)#,min_size="80,0")
			hbox.add(label)
			field = widgets.TextField(text=self.map.get(metafield))#,min_size="100,0")
			hbox.add(field)

		self.mapEdit.show()

	def _selectLayer(self):
		Selection([layer.Id() for layer in self.map.getLayers()], self._editLayer)

	def _editLayer(self, layerid):
		self.layer = self.map.getLayers('id', layerid)[0]
		print layerid

	def _selectDataset(self):
		if not self.datasetSelector:
			self.datasetSelector = DatasetSelector(self.engine, self.map, self.editWith)
		self.datasetSelector.show()

	def editWith(self, object):
		self.object = object

	def pump(self):
		self.viewer.pump()

	def quit(self):
#   Sleek: why doesn't this work?
#		self.viewer.deactivate() 
		self.camera = None
		self.layer = None
		self.selection = None

		self.mapEdit.hide()

	def _setSelection(self, screenx, screeny):
		if self.camera:
			# TODO: make Sleek fix this ugly mess
			tmp = fife.ScreenPoint(screenx, screeny)
			dy = -(tmp.y - self.camera.toScreenCoordinates(self.camera.getLocation().getMapCoordinates()).y)
			tmp.z = (int)(math.tan(self.camera.getTilt()* (math.pi / 180.0)) * dy)
			self.selection = self.camera.toMapCoordinates(tmp)
			self.selection.z = 0
			self.selection = self.layer.getCellGrid().toLayerCoordinates(self.selection)
			loc = fife.Location()
			loc.setLayer(self.layer)
			loc.setLayerCoordinates(self.selection)
			fife.CellSelectionRenderer.getInstance(self.engine.getView()).selectLocation(loc)

	def _placeInstance(self):
		if self.insertmode and self.selection and self.object:
			inst = self.layer.createInstance(self.object, self.selection)
			fife.InstanceVisual.create(inst)

	def mousePressed(self, evt):
		self._setSelection(evt.getX(), evt.getY())
		self._placeInstance()
		#self.clickmode = True

	def mouseReleased(self, evt):
		#self.clickmode = False
		pass
	def mouseEntered(self, evt):
		pass
	def mouseExited(self, evt):
		pass
	def mouseClicked(self, evt):
		pass
	def mouseWheelMovedUp(self, evt):
		pass
	def mouseWheelMovedDown(self, evt):
		pass
	def mouseMoved(self, evt):
		pass

	def mouseDragged(self, evt):
		#if self.clickmode:
		self._setSelection(evt.getX(), evt.getY())
		self._placeInstance()

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		if keystr == 'm':
			self.insertmode = not self.insertmode

		elif keystr == 'x':
			if self.selection:
				for inst in self.layer.getInstances('loc', str(self.selection.x) + ',' + str(self.selection.y)):
					self.layer.deleteInstance(inst)
				
		elif keystr == 't':
			gridrenderer = self.engine.getView().getRenderer('GridRenderer')
			gridrenderer.setEnabled(not gridrenderer.isEnabled())

		elif keystr == 'b':
			blockrenderer = self.engine.getView().getRenderer('BlockingInfoRenderer')
			blockrenderer.setEnabled(not blockrenderer.isEnabled())

	def keyReleased(self, evt):
		pass

