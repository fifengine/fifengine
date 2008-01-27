# MapEditor is a plugin for Fifedit. It allows for selection and visual editing of maps.
# MapEditor must be pumped (see pump).

import math

import fife
import pychan
import pychan.widgets as widgets
from viewer import Viewer

class MapEditor(fife.IMouseListener, fife.IKeyListener):
	def __init__(self, engine):
		self.engine = engine
		self.eventmanager = self.engine.getEventManager()

		fife.IMouseListener.__init__(self)
		self.eventmanager.addMouseListener(self)
		fife.IKeyListener.__init__(self)
		self.eventmanager.addKeyListener(self)

		# Fifedit plugin data
		self.menu_items = { 'Edit' : self._editSelection }

		# MapEditor needs a viewer so the user can see things.
		self.viewer = Viewer(self.engine)

		self.mapSelect = None
		self.camera = None
		self.elevation = None
		self.layer = None
		self.selection = None

	# gui for selecting a map
	def _editSelection(self):
		map_list = [map.Id() for map in self.engine.getModel().getMaps()]

		def selected():
			selection = self.mapSelect.collectData(['optionDrop'])['optionDrop']
			if selection < 0:
				return
			self.viewer.viewMap(map_list[selection])
			self.camera = self.viewer.camera
			self.elevation = self.camera.getLocation().getElevation()
			self.layer = self.camera.getLocation().getLayer()

		if not self.mapSelect:
			self.mapSelect = pychan.loadXML('content/gui/selection.xml')
			self.mapSelect.mapEvents({
				'okButton'    : selected,
				'cancelButton' : self.mapSelect.hide
			})

		self.mapSelect.distributeInitialData({
			'optionDrop'  : map_list
		})
		self.mapSelect.show()

	def pump(self):
		self.viewer.pump()

	def mousePressed(self, evt):
		if self.camera:
			# TODO: make Sleek fix this ugly mess
			tmp = fife.ScreenPoint(evt.getX(), evt.getY())
			dy = -(tmp.y - self.camera.toScreenCoordinates(self.camera.getLocation().getElevationCoordinates()).y)
			tmp.z = (int)(math.tan(self.camera.getTilt()* (math.pi / 180.0)) * dy)
			self.selection = self.camera.toElevationCoordinates(tmp)
			self.selection.z = 0
			self.selection = self.layer.getCellGrid().toLayerCoordinates(self.selection)

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
		pass

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		if keystr == 'm':
			pass # TODO: make an instance
		elif keystr == 'x':
			if self.selection:
				for inst in self.layer.getInstances('loc', str(self.selection.x) + ',' + str(self.selection.y)):
					self.layer.deleteInstance(inst)

	def keyReleased(self, evt):
		pass

