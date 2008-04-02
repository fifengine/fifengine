# coding: utf-8

"""
Plugin for the editor. See fifedit.py.
Viewer provides for both viewing loaded content, and selecting a view. See viewMap.
Viewer must be pumped (see pump) for proper scrolling.
"""

import math

import fife
import pychan
import plugin

SCROLL_MODIFIER = 0.3

class Viewer(plugin.Plugin, fife.IKeyListener, fife.IMouseListener):
	def __init__(self, engine):
		self.engine = engine
		self.viewSelect = None

		eventmanager = self.engine.getEventManager()

		eventmanager.setNonConsumableKeys([
			fife.Key.LEFT,
			fife.Key.RIGHT,
			fife.Key.UP,
			fife.Key.DOWN])

		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)
		fife.IMouseListener.__init__(self)
		eventmanager.addMouseListener(self)

		self._ctrldown = False
		self._shiftdown = False
		self._altdown = False
		self._dragx = 0
		self._dragy = 0

		self.active = False
		self.new_view = False
		self.map = None
		self.camera = None

	def mousePressed(self, evt):
		if (evt.getButton() == fife.MouseEvent.LEFT) and (self._ctrldown):
			self._dragx = evt.getX()
			self._dragy = evt.getY()

	def mouseReleased(self, evt):
		pass
	def mouseEntered(self, evt):
		pass
	def mouseExited(self, evt):
		pass
	def mouseClicked(self, evt):
		pass
	def mouseMoved(self, evt):
		pass
	
	def mouseWheelMovedUp(self, evt):
		if self._ctrldown and self.camera:
			self.camera.setZoom(self.camera.getZoom() * 1.05)
			
	def mouseWheelMovedDown(self, evt):
		if self._ctrldown and self.camera:
			self.camera.setZoom(self.camera.getZoom() / 1.05)
	
	def move_camera(self, screen_x, screen_y):
		coords = self.camera.getLocationRef().getMapCoordinates()
		z = self.camera.getZoom()
		r = self.camera.getRotation()
		if screen_x:
			coords.x -= screen_x / z * math.cos(r / 180.0 * math.pi) / 100;
			coords.y -= screen_x / z * math.sin(r / 180.0 * math.pi) / 100;
		if screen_y:
			coords.x -= screen_y / z * math.sin(-r / 180.0 * math.pi) / 100;
			coords.y -= screen_y / z * math.cos(-r / 180.0 * math.pi) / 100;
		coords = self.camera.getLocationRef().setMapCoordinates(coords)
		self.camera.refresh()
	
	def mouseDragged(self, evt):
		if self._ctrldown and self.camera:
			self.move_camera(evt.getX() - self._dragx, evt.getY() - self._dragy)
			self._dragx = evt.getX()
			self._dragy = evt.getY()

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		if keyval == fife.Key.LEFT:
			self.move_camera(-50, 0)
		elif keyval == fife.Key.RIGHT:
			self.move_camera(50, 0)
		elif keyval == fife.Key.UP:
			self.move_camera(0, -50)
		elif keyval == fife.Key.DOWN:
			self.move_camera(0, 50)
		elif keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
			self._ctrldown = True
		elif keyval in (fife.Key.LEFT_SHIFT, fife.Key.RIGHT_SHIFT):
			self._shiftdown = True
		elif keyval in (fife.Key.LEFT_ALT, fife.Key.RIGHT_ALT):
			self._altdown = True
	
	def keyReleased(self, evt):
		keyval = evt.getKey().getValue()
		if keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
			self._ctrldown = False
		elif keyval in (fife.Key.LEFT_SHIFT, fife.Key.RIGHT_SHIFT):
			self._shiftdown = False
		elif keyval in (fife.Key.LEFT_ALT, fife.Key.RIGHT_ALT):
			self._altdown = False

	def _editSelection(self):
		map_list = [map.Id() for map in self.engine.getModel().getMaps()]

		def selected():
			selection = self.viewSelect.collectData('optionDrop')
			if selection < 0:
				return
			self._viewMap(map_list[selection])

		if not self.viewSelect:
			self.viewSelect = pychan.loadXML('content/gui/selection.xml')
			self.viewSelect.mapEvents({
				'okButton'    : selected,
				'cancelButton' : self.viewSelect.hide
			})

		self.viewSelect.distributeInitialData({
			'optionDrop'  : map_list
		})
		self.viewSelect.show()

	def viewMap(self, mapid):
		self.map = self.engine.getModel().getMap(mapid)
		if self.map.getLayers():
			self.layer = self.map.getLayers()[0]
			self.adjust_views()
			self.active = True
		else:
			raise AttributeError('Viewer error: map ' + self.map.getId() + ' has no layers. Nothing to view.')

	def deactivate(self):
		self.camera.setEnabled(False)
		self.active = False

	def adjust_views(self):
		W = self.engine.getRenderBackend().getScreenWidth()
		H = self.engine.getRenderBackend().getScreenHeight()

		self._set_camera((0, 0, W, H))
		self.engine.getView().resetRenderers()
		for camera in self.engine.getView().getCameras():
			camera.setEnabled(False)
		self.camera.setEnabled(True)
		self.new_view = True

	def _set_camera(self, viewport):
		if self.map:
			# grab a camera associated with this map
			try:
				self.camera = filter(lambda c: c.getLocationRef().getMap().getId() == self.map.getId(), self.engine.getView().getCameras())[0]
			except IndexError:
				raise RuntimeError('No cameras found associated with this map: ' + self.map.getId())

		self.camera.setViewPort(fife.Rect(*[int(c) for c in viewport]))

	def pump(self):
		if not self.active:
			return
