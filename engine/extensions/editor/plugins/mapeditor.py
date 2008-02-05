# MapEditor is a plugin for Fifedit. It allows for selection and visual editing of maps.
# MapEditor must be pumped (see pump).

import math

import fife
import pychan
import pychan.widgets as widgets
from viewer import Viewer
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
		self.elevation = None
		self.layer = None
		self.selection = None

		self.dataset = None
		self.object = None

		self.elevEdit = None

		self.insertmode = False

	# gui for selecting a map
	def _selectMap(self):
		Selection([map.Id() for map in self.engine.getModel().getMaps()], self.editMap)

	def editMap(self, mapid):
		self.viewer.viewMap(mapid)
		self.map = self.engine.getModel().getMaps('id', mapid)[0]
		self.camera = self.viewer.camera
		self.elevation = self.camera.getLocation().getElevation()
		self.layer = self.camera.getLocation().getLayer()

		if not self.mapEdit:
			self.mapEdit = pychan.loadXML('content/gui/mapeditor.xml')
			self.mapEdit.mapEvents({
				'elevButton'  : self._selectElevation,
				'datButton'   : self._selectDataset,
				'closeButton' : self.quit
			})

		metafields = self.mapEdit.findChild(name='Metadata Properties')
		for metafield in self.map.listFields():
			hbox = widgets.HBox()
			metafields.add(hbox)

			label = widgets.Label(text=metafield)
			hbox.add(label)
			field = widgets.TextField(text=self.map.get(metafield))
			hbox.add(field)

		self.mapEdit.show()

	def _selectElevation(self):
		Selection([elevation.Id() for elevation in self.map.getElevations()], self._editElevation)

	def _editElevation(self, elevid):
		self.elevation = self.map.getElevations('id', elevid)[0]

		if not self.elevEdit:
			self.elevEdit = pychan.loadXML('content/gui/eleveditor.xml')
			self.elevEdit.mapEvents({
				'layerButton' : self._selectLayer,
				'closeButton' : self.elevEdit.hide
			})

		metafields = self.elevEdit.findChild(name='Metadata Properties')
		for metafield in self.elevation.listFields():
			hbox = widgets.HBox()
			metafields.add(hbox)

			label = widgets.Label(text=metafield)
			hbox.add(label)
			field = widgets.TextField(text=self.elevation.get(metafield))
			hbox.add(field)

		self.elevEdit.show()

	def _selectLayer(self):
		Selection([layer.Id() for layer in self.elevation.getLayers()], self._editLayer)

	def _editLayer(self, layerid):
		self.layer = self.elevation.getLayers('id', layerid)[0]
		print layerid

	def _selectDataset(self):
		Selection([dat.Id() for dat in self.map.getDatasets()], self._viewDataset)

	def _viewDataset(self, datid):
		self.dataset = self.engine.getModel().getMetaModel().getDatasets('id', datid)[0]
		ClickSelection([obj.Id() for obj in self.dataset.getObjects()], self.editWith)

	def editWith(self, object_id):
		if self.dataset:
			self.object = self.dataset.getObjects('id', object_id)[0]

	def pump(self):
		self.viewer.pump()

	def quit(self):
#   Sleek: why doesn't this work?
#		self.viewer.deactivate() 
		self.camera = None
		self.elevation = None
		self.layer = None
		self.selection = None

		self.mapEdit.hide()

	def _setSelection(self, screenx, screeny):
		if self.camera:
			# TODO: make Sleek fix this ugly mess
			tmp = fife.ScreenPoint(screenx, screeny)
			dy = -(tmp.y - self.camera.toScreenCoordinates(self.camera.getLocation().getElevationCoordinates()).y)
			tmp.z = (int)(math.tan(self.camera.getTilt()* (math.pi / 180.0)) * dy)
			self.selection = self.camera.toElevationCoordinates(tmp)
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

	def mouseReleased(self, evt):
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

