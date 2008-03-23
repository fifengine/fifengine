# MapEditor is a plugin for Fifedit. It allows for selection and visual editing of maps.
# MapEditor must be pumped (see pump).

import math

import fife
import plugin
import pychan
import pychan.widgets as widgets
from viewer import Viewer
from editor.selection import Selection, ClickSelection
from editor.plugins.objectselector import ObjectSelector

class MapEditor(plugin.Plugin,fife.IMouseListener, fife.IKeyListener):
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

		self.object = None

		self.insertmode = False
		self.objectSelector = None

	# gui for selecting a map
	def _selectMap(self):
		Selection([map.getId() for map in self.engine.getModel().getMaps()], self.editMap)

	def editMap(self, mapid):
		self.viewer.viewMap(mapid)
		self.map = self.engine.getModel().getMap(mapid)
		self.camera = self.viewer.camera
		self.layer = self.camera.getLocationRef().getLayer()

		if not self.mapEdit:
			self.mapEdit = pychan.loadXML('content/gui/mapeditor.xml')
			self.mapEdit.mapEvents({
				'layerButton'  : self._selectLayer,
				'objButton'   : self._selectObject,
				'closeButton' : self.quit
			})

		fields = self.mapEdit.findChild(name='Properties')
		
		# Clear previously added children
		fields.removeChildren(*fields.children)
		
		hbox = widgets.HBox()
		fields.addChild(hbox)

		label = widgets.Label(text='ID',min_size=(80,0))
		hbox.addChild(label)
		field = widgets.TextField(text=self.map.getId(),min_size=(100,0))
		hbox.addChild(field)
		
		# Make sure Layout get's recalculated.
		self.mapEdit.adaptLayout()
		
		self.mapEdit.show()

	def _selectLayer(self):
		Selection([layer.getId() for layer in self.map.getLayers()], self._editLayer)

	def _editLayer(self, layerid):
		self.layer = self.map.getLayers()[0]

	def _selectObject(self):
		if not self.objectSelector:
			self.objectSelector = ObjectSelector(self.engine, self.map, self.editWith)
		self.objectSelector.show()

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
			self.selection = self.camera.toMapCoordinates(fife.ScreenPoint(screenx, screeny), False)
			self.selection.z = 0
			self.selection = self.layer.getCellGrid().toLayerCoordinates(self.selection)
			loc = fife.Location(self.layer)
			loc.setLayerCoordinates(self.selection)
			fife.CellSelectionRenderer.getInstance(self.viewer.camera).selectLocation(loc)

	def _placeInstance(self):
		if self.insertmode and self.selection and self.object:
			# don't place repeat instances
			for prior in self.layer.getInstances('loc', ''.join([str(self.selection.x),',',str(self.selection.y)])):
				if prior.getObject().this == self.object.this:
					return
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
		self._setSelection(evt.getX(), evt.getY())
		self._placeInstance()

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
				for inst in self.layer.getInstances('loc', ''.join([str(self.selection.x),',',str(self.selection.y)])):
					self.layer.deleteInstance(inst)
				
		elif keystr == 't':
			gridrenderer = self.viewer.camera.getRenderer('GridRenderer')
			gridrenderer.setEnabled(not gridrenderer.isEnabled())

		elif keystr == 'b':
			blockrenderer = self.viewer.camera.getRenderer('BlockingInfoRenderer')
			blockrenderer.setEnabled(not blockrenderer.isEnabled())

	def keyReleased(self, evt):
		pass

