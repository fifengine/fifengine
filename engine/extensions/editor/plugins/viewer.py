# coding: utf-8

"""
Plugin for the editor. See fifedit.py.
Viewer provides for both viewing loaded content, and selecting a view. See viewMap.
Viewer must be pumped (see pump) for proper scrolling.
"""

import math

import fife
import pychan

SCROLL_MODIFIER = 0.3

class Viewer(fife.IKeyListener, fife.IMouseListener):
	def __init__(self, engine):
		self.engine = engine
		self.viewSelect = None

		eventmanager = self.engine.getEventManager()

		eventmanager.setNonConsumableKeys([
			fife.IKey.LEFT,
			fife.IKey.RIGHT,
			fife.IKey.UP,
			fife.IKey.DOWN])

		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)
		fife.IMouseListener.__init__(self)
		eventmanager.addMouseListener(self)

		self.scrollwheelvalue = 0
		self.ctrl_scrollwheelvalue = 0
		self.alt_scrollwheelvalue = 0
		self.shift_scrollwheelvalue = 0

		# scroll support
		self.horizscroll = 0
		self.vertscroll = 0
		self.horizscrolldir = 1
		self.vertscrolldir = 1
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
		if (evt.getButton() == fife.IMouseEvent.LEFT ):
			if self._ctrldown:
				self._dragx = evt.getX()
				self._dragy = evt.getY()
				self.horizscroll = 0
				self.vertscroll = 0

	def mouseReleased(self, evt):
		self._dragx = 0
		self._dragy = 0
		self.horizscroll = 0
		self.vertscroll = 0
	
	def mouseEntered(self, evt):
		pass
	def mouseExited(self, evt):
		pass
	def mouseClicked(self, evt):
		pass
	def mouseWheelMovedUp(self, evt):
		if self._ctrldown:
			self.ctrl_scrollwheelvalue += 1
		elif self._shiftdown:
			self.shift_scrollwheelvalue += 0.01
		elif self._altdown :
			if  self.alt_scrollwheelvalue < -1:
				self.alt_scrollwheelvalue += 1
		else:
			self.scrollwheelvalue += 0.1
			
	def mouseWheelMovedDown(self, evt):
		if self._ctrldown:
			self.ctrl_scrollwheelvalue -= 1
		elif self._shiftdown:
			self.shift_scrollwheelvalue -= 0.01
		elif self._altdown :
			if  self.alt_scrollwheelvalue > -89:
				self.alt_scrollwheelvalue -= 1
		else:
			self.scrollwheelvalue -= 0.1
	
	def mouseMoved(self, evt):
		pass
	def mouseDragged(self, evt):
		if self._ctrldown:
			self.horizscroll = self.vertscroll = 1
			self.horizscrolldir = -((self._dragx - evt.getX()) / 10.0)
			self.vertscrolldir = -((self._dragy - evt.getY()) / 10.0)

			self._dragx = evt.getX()
			self._dragy = evt.getY()

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		if keyval == fife.IKey.LEFT:
			self.horizscroll = 1;
			self.horizscrolldir = -1;
		elif keyval == fife.IKey.RIGHT:
			self.horizscroll = 1;
			self.horizscrolldir = 1;
		elif keyval == fife.IKey.UP:
			self.vertscroll = 1;
			self.vertscrolldir = -1;
		elif keyval == fife.IKey.DOWN:
			self.vertscroll = 1;
			self.vertscrolldir = 1;
		elif keyval in (fife.IKey.LEFT_CONTROL, fife.IKey.RIGHT_CONTROL):
			self._ctrldown = True
		elif keyval in (fife.IKey.LEFT_SHIFT, fife.IKey.RIGHT_SHIFT):
			self._shiftdown = True
		elif keyval in (fife.IKey.LEFT_ALT, fife.IKey.RIGHT_ALT):
			self._altdown = True
	
	def keyReleased(self, evt):
		keyval = evt.getKey().getValue()
		if keyval in (fife.IKey.LEFT_CONTROL, fife.IKey.RIGHT_CONTROL):
			self._ctrldown = False
		elif keyval in (fife.IKey.LEFT_SHIFT, fife.IKey.RIGHT_SHIFT):
			self._shiftdown = False
		elif keyval in (fife.IKey.LEFT_ALT, fife.IKey.RIGHT_ALT):
			self._altdown = False
		elif keyval == fife.IKey.LEFT:
			self.horizscroll = 0
		elif keyval == fife.IKey.RIGHT:
			self.horizscroll = 0
		elif keyval == fife.IKey.UP:
			self.vertscroll = 0
		elif keyval == fife.IKey.DOWN:
			self.vertscroll = 0

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
		self.map = self.engine.getModel().getMaps('id', mapid)[0]
		self.layer = self.map.getLayers()[0]
		self.adjust_views()
		self.active = True

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
			self.camera = filter(lambda c: c.getLocationRef().getMap().Id() == self.map.Id(), self.engine.getView().getCameras())[0]

		if not self.camera:
			raise RuntimeError, "No default camera view found for this map: " + self.map.Id()

		self.camera.setViewPort(fife.Rect(*[int(c) for c in viewport]))

	def pump(self):
		if not self.active: return
		if (self.horizscroll or self.vertscroll):
			loc = self.camera.getLocationRef()
			cam_scroll = loc.getExactLayerCoordinates()
			if (self.horizscroll):
				cam_scroll.x += SCROLL_MODIFIER*self.horizscrolldir * (2/self.camera.getZoom()) * math.cos(self.camera.getRotation()/180.0 * math.pi);
				cam_scroll.y += SCROLL_MODIFIER*self.horizscrolldir * (2/self.camera.getZoom()) * math.sin(self.camera.getRotation()/180.0 * math.pi);
			if (self.vertscroll):
				cam_scroll.x += SCROLL_MODIFIER*self.vertscrolldir *(2/self.camera.getZoom()) *  math.sin(-self.camera.getRotation()/180.0 * math.pi);
				cam_scroll.y += SCROLL_MODIFIER*self.vertscrolldir *(2/self.camera.getZoom()) *  math.cos(-self.camera.getRotation()/180.0 * math.pi);

			loc.setExactLayerCoordinates(cam_scroll)
			self.camera.setLocation(loc)
			self.horizscroll = self.vertscroll = 0
