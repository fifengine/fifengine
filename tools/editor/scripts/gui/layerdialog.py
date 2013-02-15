# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2013 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from fife import fife
from fife.extensions import pychan
from fife.extensions.pychan import dialogs
from fife.extensions.pychan.tools import callbackWithArguments as cbwa

import scripts.editor

class LayerDialog(object):
	""" 
	The B{LayerDialog} provides a gui dialog for creating and editing layers.
	
	@todo:
			- gridtypes can only be square for now
			- pathing strategy 
	"""

	# default should be pychan default, highlight can be choosen (format: r,g,b)
	DEFAULT_BACKGROUND_COLOR = pychan.internal.DEFAULT_STYLE['default']['base_color']
	HIGHLIGHT_BACKGROUND_COLOR = pychan.internal.DEFAULT_STYLE['default']['selection_color']
	SELECTION_COLOR = (0,70,10)
	COLORED_DEFAULT_BACKGROUND_COLOR = (SELECTION_COLOR[0] + DEFAULT_BACKGROUND_COLOR.r,
										SELECTION_COLOR[1] + DEFAULT_BACKGROUND_COLOR.g,
										SELECTION_COLOR[2] + DEFAULT_BACKGROUND_COLOR.b)
	COLORED_HIGHLIGHT_BACKGROUND_COLOR = (SELECTION_COLOR[0] + HIGHLIGHT_BACKGROUND_COLOR.r,
										  SELECTION_COLOR[1] + HIGHLIGHT_BACKGROUND_COLOR.g,
										  SELECTION_COLOR[2] + HIGHLIGHT_BACKGROUND_COLOR.b)
	
	LABEL_NAME_PREFIX = "select_"
	
	def __init__(self, engine, map, callback=None, onCancel=None, layer=None):
		self.engine = engine
		self.model = engine.getModel()
		self.map = map
		self.layer = layer
		self.callback = callback
		self.onCancel = onCancel
		self.selected_interact_layer = 0
		self._widget = pychan.loadXML('gui/layerdialog.xml')
		self._editor = scripts.editor.getEditor()

		# wrapper for layers
		self.wrapper = self._widget.findChild(name="layers_wrapper")
		
		# TODO: Add access method for adopted grid types?
		self._widget.findChild(name="gridBox").items = ['square', 'hexagonal']
		
		# TODO: Ditto for pather?
		self._widget.findChild(name="pathingBox").items = ['cell_edges_only', 'cell_edges_and_diagonals']

		self._widget.findChild(name="sortingBox").items = ['camera', 'location', 'camera_and_location']

		if layer:
			cg = layer.getCellGrid()
			cgtype = 0
			if cg.getType() == 'hexagonal':
				cgtype = 1
			
			self._widget.distributeData({
				"layerBox" : unicode(layer.getId()),
				"xScaleBox" : unicode(cg.getXScale()),
				"yScaleBox" : unicode(cg.getYScale()),
				"zScaleBox" : unicode(cg.getZScale()),
				"rotBox" : unicode(cg.getRotation()),
				"xOffsetBox" : unicode(cg.getXShift()),
				"yOffsetBox" : unicode(cg.getYShift()),
				"zOffsetBox" : unicode(cg.getZShift()),
				"transBox" : unicode(layer.getLayerTransparency()),
				'is_walkable': bool(layer.isWalkable()),
				'is_interact': bool(layer.isInteract())
			})

			self._widget.findChild(name="pathingBox").selected = int(layer.getPathingStrategy())
			self._widget.findChild(name="sortingBox").selected = int(layer.getSortingStrategy())
			self._widget.findChild(name="gridBox").selected = int(cgtype)

			# fill layer list
			layers = self.map.getLayers()
			if len(layers) <= 0:
				layerid = "No layers"
				subwrapper = pychan.widgets.HBox()

				layerLabel = pychan.widgets.Label()
				layerLabel.text = unicode(layerid)
				layerLabel.name = LayerDialog.LABEL_NAME_PREFIX + layerid
				subwrapper.addChild(layerLabel)
				self.wrapper.addChild(subwrapper)
			
			interact_layers = self.layer.getInteractLayers()
			for l in reversed(layers):
				layerid = l.getId()
				if layerid == self.layer.getId():
					continue
				
				subwrapper = pychan.widgets.HBox()
				layerLabel = pychan.widgets.Label()
				layerLabel.text = unicode(layerid)
				layerLabel.name = LayerDialog.LABEL_NAME_PREFIX + layerid
				layerLabel.capture(cbwa(self.selectInteractLayer, l, layerLabel),"mousePressed")

				for i in interact_layers:
					if i.getId() == layerid:
						layerLabel.background_color	= LayerDialog.COLORED_DEFAULT_BACKGROUND_COLOR
						layerLabel.foreground_color	= LayerDialog.COLORED_DEFAULT_BACKGROUND_COLOR
						layerLabel.base_color		= LayerDialog.COLORED_DEFAULT_BACKGROUND_COLOR
						break
				
				subwrapper.addChild(layerLabel)
				self.wrapper.addChild(subwrapper)

			# button callbacks
			wdgt = self._widget.findChild(name='interact_add')
			wdgt.capture(self.addInteractLayer, 'mousePressed')
			wdgt.capture(cbwa(self._editor.getStatusBar().showTooltip, wdgt.helptext), 'mouseEntered')
			wdgt.capture(self._editor.getStatusBar().hideTooltip, 'mouseExited')
			
			wdgt = self._widget.findChild(name='interact_remove')
			wdgt.capture(self.removeInteractLayer, 'mousePressed')
			wdgt.capture(cbwa(self._editor.getStatusBar().showTooltip, wdgt.helptext), 'mouseEntered')
			wdgt.capture(self._editor.getStatusBar().hideTooltip, 'mouseExited')

			wdgt = self._widget.findChild(name='is_interact')
			wdgt.capture(self.updateCheckBox, 'mousePressed')
		else:
			self._widget.findChild(name="pathingBox").selected = 0
			self._widget.findChild(name="sortingBox").selected = 0
			self._widget.findChild(name="gridBox").selected = 0

		self._widget.mapEvents({
			'okButton'     : self._finished,
			'cancelButton' : self._cancelled
		})

		self._widget.show()

	def updateCheckBox(self):
		if self.layer:
			wdgt = self._widget.findChild(name='is_interact')
			wdgt._setMarked(not self.layer.isInteract())
		
	def selectInteractLayer(self, layer, widget):
		self.selected_interact_layer = layer

		# reset
		layers = layer.getMap().getLayers()
		for l in layers:
			if l == self.layer:
				continue
			layer_id = l.getId()
			layer_widget = self.wrapper.findChild(name=LayerDialog.LABEL_NAME_PREFIX + layer_id)
			if layer_widget:
				if l.isInteract() and l.getWalkableId() == self.layer.getId():
					color = LayerDialog.COLORED_DEFAULT_BACKGROUND_COLOR
				else:
					color = LayerDialog.DEFAULT_BACKGROUND_COLOR

				layer_widget.background_color = color
				layer_widget.foreground_color = color
				layer_widget.base_color = color
				layer_widget.text = unicode(layer_id)

		# set new highlighted
		if layer.isInteract() and layer.getWalkableId() == self.layer.getId():
			color = LayerDialog.COLORED_HIGHLIGHT_BACKGROUND_COLOR
		else:
			color = LayerDialog.HIGHLIGHT_BACKGROUND_COLOR
			
		widget.background_color = color
		widget.foreground_color = color
		widget.base_color = color

	def addInteractLayer(self):
		if self.layer.isWalkable() and self.selected_interact_layer:
			if self.selected_interact_layer.isInteract():
				if self.layer.getId() == self.selected_interact_layer.getWalkableId():
					return
			
			cache = self.layer.getCellCache()
			if cache:
				cache.addInteractOnRuntime(self.selected_interact_layer)
				layer_id = self.selected_interact_layer.getId()
				layer_widget = self.wrapper.findChild(name=LayerDialog.LABEL_NAME_PREFIX + layer_id)
				if layer_widget:
					layer_widget.background_color = LayerDialog.COLORED_HIGHLIGHT_BACKGROUND_COLOR
					layer_widget.foreground_color = LayerDialog.COLORED_HIGHLIGHT_BACKGROUND_COLOR
					layer_widget.base_color = LayerDialog.COLORED_HIGHLIGHT_BACKGROUND_COLOR
					layer_widget.text = unicode(layer_id)

	def removeInteractLayer(self):
		if self.layer.isWalkable() and self.selected_interact_layer:
			if self.selected_interact_layer.isInteract():
				if self.layer.getId() == self.selected_interact_layer.getWalkableId():
					cache = self.layer.getCellCache()
					if cache:
						cache.removeInteractOnRuntime(self.selected_interact_layer)
						layer_id = self.selected_interact_layer.getId()
						layer_widget = self.wrapper.findChild(name=LayerDialog.LABEL_NAME_PREFIX + layer_id)
						if layer_widget:
							layer_widget.background_color = LayerDialog.HIGHLIGHT_BACKGROUND_COLOR
							layer_widget.foreground_color = LayerDialog.HIGHLIGHT_BACKGROUND_COLOR
							layer_widget.base_color = LayerDialog.HIGHLIGHT_BACKGROUND_COLOR
							layer_widget.text = unicode(layer_id)

	def _cancelled(self):
		""" """
		if self.onCancel:
			self.onCancel()
		self._widget.hide()		

	def _finished(self):
		""" """
		# Collect and validate data
		layerId = self._widget.collectData('layerBox')
		if layerId == '':
			dialogs.message(message=unicode("Please enter a layer id."), caption=unicode("Error"))
			return
			
		try:
			x_offset = float(self._widget.collectData('xOffsetBox'))
			y_offset = float(self._widget.collectData('yOffsetBox'))
			z_offset = float(self._widget.collectData('zOffsetBox'))
		except ValueError:
			dialogs.message(message=unicode("Please enter integer or decimal values for offset."), caption=unicode("Error"))
			return

		try:
			x_scale = float(self._widget.collectData('xScaleBox'))
			y_scale = float(self._widget.collectData('yScaleBox'))
			z_scale = float(self._widget.collectData('zScaleBox'))
		except ValueError:
			dialogs.message(message=unicode("Please enter integer or decimal values for scale."), caption=unicode("Error"))
			return

		try:
			rotation = float(self._widget.collectData('rotBox'))
		except ValueError:
			dialogs.message(message=unicode("Please enter integer or decimal value for rotation."), caption=unicode("Error"))
			return
			
		try:
			transparency = int(self._widget.collectData('transBox'))
		except ValueError:
			dialogs.message(message=unicode("Please enter an integer value in the range of 0-255 for transparency."), caption=unicode("Error"))
			return
			
		# Clamp the transparency value between 0 and 255
		if transparency < 0:
			transparency = 0
		if transparency > 255:
			transparency = 255
		
		grid_type = int(self._widget.collectData('gridBox'))
		pathing = int(self._widget.collectData('pathingBox'))
		sorting = int(self._widget.collectData('sortingBox'))
		walkable = bool(self._widget.collectData('is_walkable'))
		interact = bool(self._widget.collectData('is_interact'))

		if grid_type == 0:
			grid_type = "square"
		else:
			grid_type = "hexagonal"

		# Set up layer
		layer = self.layer
		
		cellgrid = self.model.getCellGrid(grid_type)

		if not cellgrid:
			print "Invalid grid"
			return

		cellgrid.setRotation(rotation)
		cellgrid.setXScale(x_scale)
		cellgrid.setYScale(y_scale)
		cellgrid.setZScale(z_scale)
		cellgrid.setXShift(x_offset)
		cellgrid.setYShift(y_offset)
		cellgrid.setZShift(z_offset)
	
		if not self.layer:
			try:
				layer = self.map.createLayer(str(layerId), cellgrid)
				
			except fife.NameClash:
				print 'The layer ' + str(layerId) + ' already exists!'
				del cellgrid
				return
		else:
			layer.setCellGrid(cellgrid)
			try:
				layer.setId(str(layerId))
			except fife.NameClash:
				print 'The layer ' + str(layerId) + ' already exists!'
				return
		
		layer.setPathingStrategy(pathing)
		layer.setSortingStrategy(sorting)
		layer.setLayerTransparency(transparency)

		# destroy CellCache
		if layer.isWalkable() and not walkable:
			layer.destroyCellCache()
		# create CellCache
		elif not layer.isWalkable() and walkable:
			layer.setWalkable(walkable)
			layer.createCellCache()
			cache = layer.getCellCache()
			cache.createCells()
			cache.forceUpdate()
		
		for cam in self.map.getCameras():
			cam.resetRenderers()
			
			if layer:
				# make sure the layer is active for the various renderers
				fife.InstanceRenderer.getInstance(cam).addActiveLayer(layer)
				fife.GridRenderer.getInstance(cam).addActiveLayer(layer)
				fife.BlockingInfoRenderer.getInstance(cam).addActiveLayer(layer)
				fife.CoordinateRenderer.getInstance(cam).addActiveLayer(layer)
				fife.CellSelectionRenderer.getInstance(cam).addActiveLayer(layer)
				fife.LightRenderer.getInstance(cam).addActiveLayer(layer)
				fife.GenericRenderer.getInstance(cam).addActiveLayer(layer)
				fife.CellRenderer.getInstance(cam).addActiveLayer(layer)
		
		# Hide dialog and call back
		self._widget.hide()
		
		if self.callback:
			pychan.tools.applyOnlySuitable(self.callback, layer=layer)
