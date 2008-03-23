# MapWizard is a plugin for Fifedit. It is a step-by-step map creation utility.

import math

import fife
import plugin
import pychan
import pychan.widgets as widgets
from viewer import Viewer
from editor.input import Input
from editor.selection import Selection,ClickSelection

class CameraEditor():
	"""
	CameraEditor provides a gui dialog for camera creation. The callback is called when camera creation is complete. A
	partial specification of the camera parameters may optionally be given.
	"""
	def __init__(self, engine, callback=None, map=None, layer=None):
		self.engine = engine
		self.callback = callback
		self._widget = pychan.loadXML('content/gui/cameraedit.xml')

		if map:
			self._widget.distributeData({
				'mapBox'	: map.getId(),
			})

		if layer:
			self._widget.distributeData({
				'layerBox'	: layer.getId(),
			})

		self._widget.mapEvents({
			'okButton'     : self._finished,
			'cancelButton' : self._widget.hide
		})

		self._widget.show()

	def _finished(self):
		id = self._widget.collectData('idBox')
		map = self.engine.getModel().getMap(str(self._widget.collectData('mapBox')))
		layer = map.getLayer(str(self._widget.collectData('layerBox')))
		viewport = fife.Rect(*[int(c) for c in self._widget.collectData('viewBox').split(',')])

		refh = int(self._widget.collectData('refhBox'))
		refw = int(self._widget.collectData('refwBox'))

		rot = int(self._widget.collectData('rotBox'))
		tilt = int(self._widget.collectData('tiltBox'))

		cam = self.engine.getView().addCamera(id, layer, viewport, fife.ExactModelCoordinate(0,0,0))
		cam.setCellImageDimensions(refw, refh)
		cam.setRotation(rot)
		cam.setTilt(tilt)
	
		self._widget.hide()

		if self.callback:
			self.callback()

class MapWizard(plugin.Plugin):
	def __init__(self, engine):
		self.engine = engine

		# Fifedit plugin data
		self.menu_items = { 'Map Wizard' : self._buildMap }

		self.newMap = False
		self.map = None

	def _buildMap(self):
		def newMap(id):
			def newLayer(id):
				def newCamera():
					self.newMap = True

				grid = fife.SquareGrid()
				layer = self.map.createLayer(id, grid)
				grid.thisown = 0

				CameraEditor(self.engine, newCamera, self.map, layer)

			self.map = self.engine.getModel().createMap(id)
			Input('Enter a layer identifier for a default layer:', newLayer)
		
		Input('Enter a map identifier:', newMap)
