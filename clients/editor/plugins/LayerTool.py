# coding: utf-8
# ###################################################
# Copyright (C) 2008 The Zero-Projekt team
# http://zero-projekt.net
# info@zero-projekt.net
# This file is part of Zero "Was vom Morgen blieb"
#
# The Zero-Projekt codebase is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the
# Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
# ###################################################

""" A layertool plugin for FIFedit """

import fife
import scripts.plugin as plugin
import scripts.editor
from scripts.events import *
from scripts.gui.action import Action
import pychan
import pychan.widgets as widgets
from pychan.tools import callbackWithArguments as cbwa

# default should be pychan default, highlight can be choosen (format: r,g,b)
_DEFAULT_BACKGROUND_COLOR = pychan.internal.DEFAULT_STYLE['default']['base_color']
_HIGHLIGHT_BACKGROUND_COLOR = pychan.internal.DEFAULT_STYLE['default']['selection_color']

# the dynamicly created widgets have the name scheme prefix + layerid
_LABEL_NAME_PREFIX = "select_"

class LayerTool(plugin.Plugin):
	""" The B{LayerTool} allows to select and show / hide layers of a loaded
		map as well as creating new layers or edit layer properties
	"""
	def __init__(self):	
		self._editor = None
		self._enabled = False
		self._mapview = None
		
		self._showAction = None
		
		self.subwrappers = []
		
		self._layer_wizard = None
			
	#--- Plugin function ---#
	def enable(self):
		""" """
		if self._enabled is True:
			return
			
		# Fifedit plugin data
		self._editor = scripts.editor.getEditor()
		self._showAction = Action(u"LayerTool", checkable=True)
		scripts.gui.action.activated.connect(self.toggle, sender=self._showAction)
		self._editor._toolsMenu.addAction(self._showAction)

		self.__create_gui()
		
		self.toggle()
		
		events.postMapShown.connect(self.update)

	def disable(self):
		""" """
		if self._enabled is False:
			return
		self.container.setDocked(False)
		self.container.hide()
		
		events.postMapShown.disconnect(self.update)
		
		self._editor._toolsMenu.removeAction(self._showAction)

	def isEnabled(self):
		""" """
		return self._enabled;

	def getName(self):
		""" """
		return u"Layertool"
	
	#--- End plugin functions ---#
	
	def __create_gui(self):
		""" create the basic gui container """
		self.container =  pychan.loadXML('gui/layertool.xml')
		self.wrapper = self.container.findChild(name="layers_wrapper")
		self.removeLayerButton = self.container.findChild(name="remove_layer_button")
		self.createLayerButton = self.container.findChild(name="add_layer_button")
		self.editLayerButton = self.container.findChild(name="edit_layer_button")
		
		self.removeLayerButton.capture(self.removeSelectedLayer)
		self.removeLayerButton.capture(cbwa(self._editor.getStatusBar().showTooltip, self.removeLayerButton.helptext), 'mouseEntered')
		self.removeLayerButton.capture(self._editor.getStatusBar().hideTooltip, 'mouseExited')
		
		self.createLayerButton.capture(self.showLayerWizard)
		self.createLayerButton.capture(cbwa(self._editor.getStatusBar().showTooltip, self.createLayerButton.helptext), 'mouseEntered')
		self.createLayerButton.capture(self._editor.getStatusBar().hideTooltip, 'mouseExited')
		
		self.editLayerButton.capture(self.showEditDialog)
		self.editLayerButton.capture(cbwa(self._editor.getStatusBar().showTooltip, self.editLayerButton.helptext), 'mouseEntered')
		self.editLayerButton.capture(self._editor.getStatusBar().hideTooltip, 'mouseExited')
		
		self.update(None)

	def _adjust_position(self):
		"""	adjusts the position of the container - we don't want to
		let the window appear at the center of the screen.
		(new default position: left, beneath the tools window)
		"""
		self.container.position = (50, 200)
		
	def removeSelectedLayer(self):
		"""
		
		"""
		if not self._mapview: return
		
		if self._mapview.getMap().getNumLayers() <= 1:
			print "Can't remove the last layer"
			return
		
		layer = self.getActiveLayer()
		if not layer: return
		
		self.select_no_layer()
		
		map = self._mapview.getMap()		
		
		# FIFE will crash if we try to delete the layer which is in use by a camera
		# so we will set the camera to another layer instead
		for cam in self._editor.getEngine().getView().getCameras():
			if cam.getLocationRef().getMap().getId() != map.getId():
				continue
				
			if cam.getLocation().getLayer().getId() != layer.getId():
				continue
				
			for l in map.getLayers():
				if l.getId() == layer.getId(): 
					continue
				
				cam.getLocationRef().setLayer(l)
				break
		
		map.deleteLayer(layer)
		self.update(self._mapview)
		
	def showLayerWizard(self):
		if not self._mapview: return
		
		if self._layer_wizard: self._layer_wizard._widget.hide()
		self._layer_wizard = LayerEditor(self._editor.getEngine(), self._mapview.getMap(), callback=self._layerCreated)
		
	def _layerCreated(self, layer):
		self.update(self._mapview)
		self.select_active_layer(None, self.wrapper.findChild(name=_LABEL_NAME_PREFIX + layer.getId()))
		
	def showEditDialog(self):
		if not self._mapview: return
		layer = self.getActiveLayer()
		if not layer: return
		
		if self._layer_wizard: self._layer_wizard._widget.hide()
		self._layer_wizard = LayerEditor(self._editor.getEngine(), self._mapview.getMap(), layer=layer, callback=cbwa(self.update, self._mapview))
		
	def clear(self):
		""" remove all subwrappers """
		if self.subwrappers is []: return
		
		for subwrapper in self.subwrappers:
			self.wrapper.removeChild(subwrapper)
			
		self.subwrappers = []
		
	def update(self, mapview):
		""" Dump new layer informations into the wrapper 
		
		We group one ToggleButton and one Label into a HBox, the main wrapper
		itself is a VBox and we also capture both the Button and the Label to listen
		for mouse actions
		"""
		layers = []
		self._mapview = mapview
		if self._mapview is not None:
			layers = self._mapview.getMap().getLayers()
		
		self.clear()
		
		if len(layers) <= 0:
			if not self._mapview:
				layerid = "No map is open"
			else:
				layerid = "No layers"
			subwrapper = pychan.widgets.HBox()

			layerLabel = pychan.widgets.Label()
			layerLabel.text = unicode(layerid)
			layerLabel.name = _LABEL_NAME_PREFIX + layerid
			subwrapper.addChild(layerLabel)
			
			self.wrapper.addChild(subwrapper)
			self.subwrappers.append(subwrapper)
		
		active_layer = self.getActiveLayer()
		if active_layer:
			active_layer = active_layer.getId()
		for layer in reversed(layers):
			layerid = layer.getId()
			subwrapper = pychan.widgets.HBox()

			toggleVisibleButton = pychan.widgets.ToggleButton(hexpand=0, up_image="gui/icons/is_visible.png", down_image="gui/icons/is_visible.png", hover_image="gui/icons/is_visible.png")
			toggleVisibleButton.name = "toggle_" + layerid
			if layer.areInstancesVisible():
				toggleVisibleButton.toggled = True
			toggleVisibleButton.capture(self.toggle_layer_visibility)
			
			layerLabel = pychan.widgets.Label()
			layerLabel.text = unicode(layerid)
			layerLabel.name = _LABEL_NAME_PREFIX + layerid
			layerLabel.capture(self.select_active_layer,"mousePressed")
			
			if active_layer == layerid:
				layerLabel.background_color	= _HIGHLIGHT_BACKGROUND_COLOR
				layerLabel.foreground_color	= _HIGHLIGHT_BACKGROUND_COLOR
				layerLabel.base_color		= _HIGHLIGHT_BACKGROUND_COLOR
			
			subwrapper.addChild(toggleVisibleButton)
			subwrapper.addChild(layerLabel)
			
			self.wrapper.addChild(subwrapper)
			self.subwrappers.append(subwrapper)
		
		self.container.adaptLayout(False)		
			
	def toggle_layer_visibility(self, event, widget):
		""" Callback for ToggleButtons 
		
		Toggle the chosen layer visible / invisible
		
		NOTE:
			- if a layer is set to invisible, it also shouldn't be the active layer anymore
		
		@type	event:	object
		@param	event:	pychan mouse event
		@type	widget:	object
		@param	widget:	the pychan widget where the event occurs, transports the layer id in it's name
		"""

		layerid = widget.name[len(_LABEL_NAME_PREFIX):]
		
		layer = self._mapview.getMap().getLayer(layerid)
		active_layer = self.getActiveLayer()
		if active_layer:
			active_layer = active_layer.getId()
		
		if layer.areInstancesVisible():
			layer.setInstancesVisible(False)
		else:
			layer.setInstancesVisible(True)
			
		if active_layer == layerid:
			self.select_no_layer()
			
			
	def select_no_layer(self):
		""" Resets the current active layer (widget + editor) """
		previous_active_layer = self.getActiveLayer()
		if previous_active_layer is not None:
			previous_layer_id = previous_active_layer.getId()
			previous_active_widget = self.container.findChild(name=_LABEL_NAME_PREFIX + previous_layer_id)
			previous_active_widget.background_color = _DEFAULT_BACKGROUND_COLOR
			previous_active_widget.foreground_color = _DEFAULT_BACKGROUND_COLOR
			previous_active_widget.base_color = _DEFAULT_BACKGROUND_COLOR
			previous_active_widget.text = unicode(previous_layer_id)
			
		self._mapview.getController().selectLayer(None)
		
	def getActiveLayer(self):
		""" Returns the active layer """
		if self._mapview:
			return self._mapview.getController()._layer
		
	def select_active_layer(self, event, widget):
		""" callback for Labels 
		
		We hand the layerid over to the mapeditor module to select a 
		new active layer
		
		Additionally, we mark the active layer widget (changing base color) and reseting the previous one

		@type	event:	object
		@param	event:	pychan mouse event
		@type	widget:	object
		@param	widget:	the pychan widget where the event occurs, transports the layer id in it's name
		"""

		self.select_no_layer()
		
		layerid = widget.name[len(_LABEL_NAME_PREFIX):]	
		
		widget.background_color = _HIGHLIGHT_BACKGROUND_COLOR
		widget.foreground_color = _HIGHLIGHT_BACKGROUND_COLOR
		widget.base_color = _HIGHLIGHT_BACKGROUND_COLOR
		self.container.adaptLayout(False)
		
		self._mapview.getController().selectLayer(layerid)

	def toggle(self):
		"""	toggles the layertool visible / invisible and sets
			dock status 
		"""
		if self.container.isVisible() or self.container.isDocked():
			self.container.setDocked(False)
			self.container.hide()

			self._showAction.setChecked(False)
		else:
			self.container.show()
			self._showAction.setChecked(True)
			self._adjust_position()


class LayerEditor(object):
	""" The B{LayerEditor} provides a gui dialog for creating and editing layers.
	
		FIXME:
			- gridtypes can only be square for now
			- pathing strategy 
	"""
	def __init__(self, engine, map, callback=None, onCancel=None, layer=None):
		self.engine = engine
		self.model = engine.getModel()
		self.map = map
		self.layer = layer
		self.callback = callback
		self.onCancel = onCancel
		self._widget = pychan.loadXML('gui/layereditor.xml')

		# TODO: Add access method for adopted grid types?
		self._widget.findChild(name="gridBox").items = ['square', 'hexagonal']
		
		# TODO: Ditto for pather?
		self._widget.findChild(name="pathingBox").items = ['cell_edges_only', 'cell_edges_and_diagonals', 'freeform']

		if layer:
			cg = layer.getCellGrid()
			cgtype = 0
			if cg.getType() == 'hexagonal':
				cgtype = 1
			
			self._widget.distributeData({
				"layerBox" : unicode(layer.getId()),
				"xScaleBox" : unicode(cg.getXScale()),
				"yScaleBox" : unicode(cg.getYScale()),
				"rotBox" : unicode(cg.getRotation()),
				"xOffsetBox" : unicode(cg.getXShift()),
				"yOffsetBox" : unicode(cg.getYShift())
			})
			
			self._widget.findChild(name="pathingBox").selected = int(layer.getPathingStrategy())
			self._widget.findChild(name="gridBox").selected = int(cgtype)
		else:
			self._widget.findChild(name="pathingBox").selected = 0
			self._widget.findChild(name="gridBox").selected = 0

		self._widget.mapEvents({
			'okButton'     : self._finished,
			'cancelButton' : self._cancelled
		})

		self._widget.show()
		
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
			print 'Please enter a layer id.'
			return
			
		try:
			x_offset = float(self._widget.collectData('xOffsetBox'))
			y_offset = float(self._widget.collectData('yOffsetBox'))
		except ValueError:
			print 'Please enter integer or decimal values for offset.'
			return

		try:
			x_scale = float(self._widget.collectData('xScaleBox'))
			y_scale = float(self._widget.collectData('yScaleBox'))
		except ValueError:
			print 'Please enter integer or decimal values for scale.'
			return

		try:
			rotation = float(self._widget.collectData('rotBox'))
		except ValueError:
			print 'Please enter integer or decimal value for rotation.'
			return
			
		grid_type = int(self._widget.collectData('gridBox'))
		pathing = int(self._widget.collectData('pathingBox'))

		if grid_type == 0:
			grid_type = "square"
		else:
			grid_type = "hexagonal"

		# Set up layer
		layer = self.layer
		cellgrid = None
		
		cellgrid = self.model.getCellGrid(grid_type)
		if not cellgrid:
			print "Invalid grid type"
			return

		cellgrid.setRotation(rotation)
		cellgrid.setXScale(x_scale)
		cellgrid.setYScale(y_scale)
		cellgrid.setXShift(x_offset)
		cellgrid.setYShift(y_offset)
	
		if not self.layer:
			try:
				layer = self.map.createLayer(str(layerId), cellgrid)
				
			except fife.NameClash:
				print 'The layer ' + str(layerId) + ' already exists!'
				return
		else:
			layer.setCellGrid(cellgrid)
			try:
				layer.setId(str(layerId))
			except fife.NameClash:
				print 'The layer ' + str(layerId) + ' already exists!'
				return
		
		layer.setPathingStrategy(pathing)
		
		self.engine.getView().resetRenderers()
		
		# Hide dialog and call back
		self._widget.hide()
		
		if self.callback:
			pychan.tools.applyOnlySuitable(self.callback, layer=layer)

