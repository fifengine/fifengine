#!/usr/bin/env python
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

""" An advanced layer tool for FIFedit """

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
	""" The B{LayerTool} is an advanced method to view
	and change layer informations.
	
	While the original FIFedit tool only allows to select
	layers, this one will provide the following functionality:
	
		- toggle layer visibility
		- select layer
		- list layers
		
	"""
	def __init__(self):	
		self._editor = None
		self._enabled = False
		self._mapview = None
		
		self._showAction = None
		
		self.subwrappers = []
			
	#--- Plugin function ---#
	def enable(self):
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
		if self._enabled is False:
			return
		self.container.setDocked(False)
		self.container.hide()
		self.removeAllChildren()
		
		events.postMapShown.disconnect(self.update)
		
		self._editor._toolsMenu.removeAction(self._showAction)

	def isEnabled(self):
		return self._enabled;

	def getName(self):
		return u"Layertool"
	
	#--- End plugin functions ---#
	
	def __create_gui(self):
		""" create the basic gui container """
		self.container =  pychan.loadXML('gui/layertool.xml')
		self.wrapper = self.container.findChild(name="layers_wrapper")
		self.update(None)

	def _adjust_position(self):
		"""	adjusts the position of the container - we don't want to
		let the window appear at the center of the screen.
		(new default position: left, beneath the tools window)
		"""
		self.container.position = (50, 200)
		
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
			layerid = "No layers"
			subwrapper = pychan.widgets.HBox()

			layer_name_widget = pychan.widgets.Label()
			layer_name_widget.text = unicode(layerid)
			layer_name_widget.name = _LABEL_NAME_PREFIX + layerid
			subwrapper.addChild(layer_name_widget)
			
			self.wrapper.addChild(subwrapper)
			self.subwrappers.append(subwrapper)
		
		active_layer = self.getActiveLayer()
		if active_layer:
			active_layer = active_layer.getId()
		for layer in reversed(layers):
			layerid = layer.getId()
			subwrapper = pychan.widgets.HBox()

			visibility_widget = pychan.widgets.ToggleButton(hexpand=0, up_image="gui/icons/is_visible.png", down_image="gui/icons/is_visible.png", hover_image="gui/icons/is_visible.png")
			visibility_widget.name = "toggle_" + layerid
			if layer.areInstancesVisible():
				visibility_widget.toggled = True
			visibility_widget.capture(self.toggle_layer_visibility,"mousePressed")
			
			layer_name_widget = pychan.widgets.Label()
			layer_name_widget.text = unicode(layerid)
			layer_name_widget.name = _LABEL_NAME_PREFIX + layerid
			layer_name_widget.capture(self.select_active_layer,"mousePressed")
			
			if active_layer == layerid:
				layer_name_widget.background_color	= _HIGHLIGHT_BACKGROUND_COLOR
				layer_name_widget.foreground_color	= _HIGHLIGHT_BACKGROUND_COLOR
				layer_name_widget.base_color		= _HIGHLIGHT_BACKGROUND_COLOR
			
			subwrapper.addChild(visibility_widget)
			subwrapper.addChild(layer_name_widget)
			
			self.wrapper.addChild(subwrapper)
			self.subwrappers.append(subwrapper)
		
		self.container.adaptLayout()		
			
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
		""" the exception approach - as soon as the user hides a layer, the mapedit module should stop to use this
		one, too.
		
		A bunch of exceptions is the result (each click on the map will result in a exception as no layer is set etc...)	
		"""
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
		
		layerid = widget.name[7:]	
		
		widget.background_color = _HIGHLIGHT_BACKGROUND_COLOR
		widget.foreground_color = _HIGHLIGHT_BACKGROUND_COLOR
		widget.base_color = _HIGHLIGHT_BACKGROUND_COLOR
		self.container.adaptLayout()
		
		self._mapview.getController().selectLayer(layerid)

	def toggle(self):
		if self.container.isVisible() or self.container.isDocked():
			self.container.setDocked(False)
			self.container.hide()

			self._showAction.setChecked(False)
		else:
			self.container.show()
			self._showAction.setChecked(True)
			self._adjust_position()
