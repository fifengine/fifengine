# MapWizard is a plugin for Fifedit. It is a step-by-step map creation utility.

import math

import fife
import plugin
import pychan
import pychan.widgets as widgets
from viewer import Viewer
from editor.input import Input
from editor.selection import Selection,ClickSelection

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
				grid = fife.SquareGrid()
				layer = self.map.createLayer(id, grid)
				grid.thisown = 0
				self.engine.getView().addCamera(id,layer,fife.Rect(0,0,100,100),fife.ExactModelCoordinate(0,0,0))
				self.newMap = True

			self.map = self.engine.getModel().createMap(id)
			Input('Enter a layer identifier for a default layer:', newLayer)
		
		Input('Enter a map identifier:', newMap)
