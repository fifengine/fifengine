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

""" an advanced layer tool for FIFedit """

import fife
import plugin
import pychan
import pychan.widgets as widgets
from pychan.tools import callbackWithArguments as cbwa

import settings as Settings

# default should be pychan default, highlight can be choosen (format: r,g,b)
_DEFAULT_BACKGROUND_COLOR = (0,0,100)
_HIGHLIGHT_BACKGROUND_COLOR = (173,216,230)

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
		
	The plugin has to register itself in the mapeditor module
	to get actual content when a new map is loaded.
	"""
	def __init__(self, engine, mapedit):	
		# Fifedit plugin data
		self.menu_items = { 'LayerTool' : self.toggle }
		self._mapedit = mapedit
		self.data = False
		self.previous_active_layer = None
		
		# "register" at mapeditor module 
		self._mapedit.layertool = self
		
		self.subwrappers = []
		self.__create_gui()
		
	def __create_gui(self):
		""" create the basic gui container """
		self.container =  pychan.loadXML('gui/layertool.xml')
		self.wrapper = self.container.findChild(name="layers_wrapper")

	def _adjust_position(self):
		"""	adjusts the position of the container - we don't want to
		let the window appear at the center of the screen.
		(new default position: left, beneath the tools window)
		"""
		self.container.position = (10, 200)
		
	def clear(self):
		""" remove all subwrappers """
		if self.subwrappers is []: return
		
		for subwrapper in self.subwrappers:
			self.wrapper.removeChild(subwrapper)
			
		self.subwrappers = []
		
	def update(self):
		""" dump new layer informations into the wrapper 
		
		We group one ToggleButton and one Lable into a HBox, the main wrapper
		itself is a VBox and we also capture both the Button and the Label to listen
		for mouse actions
		"""
		layers = self._mapedit._map.getLayers()
		
		self.clear()
		
		for layer in layers:
			layerid = layer.getId()
			subwrapper = pychan.widgets.HBox()

			visibility_widget = pychan.widgets.ToggleButton(up_image="icons/is_visible.png",down_image="icons/quit.png")
			visibility_widget.name = "toggle_" + layerid
			visibility_widget.capture(self.toggle_layer_visibility,"mousePressed")
			
			layer_name_widget = pychan.widgets.Label()
			layer_name_widget.text = layerid
			layer_name_widget.name = _LABEL_NAME_PREFIX + layerid
			layer_name_widget.capture(self.select_active_layer,"mousePressed")
			
			subwrapper.addChild(visibility_widget)
			subwrapper.addChild(layer_name_widget)
			
			self.wrapper.addChild(subwrapper)
			self.subwrappers.append(subwrapper)
		
		self.container.adaptLayout()	
		self.data = True			

	def toggle(self):
		""" toggle visibility of the main gui container """
		if self.container.isVisible():
			self.container.hide()
		else:
			self.container.show()
			self._adjust_position()
			
	def toggle_layer_visibility(self, event, widget):
		""" callback for ToggleButtons 
		
		Toggle the chosen layer visible / invisible
		
		NOTE:
			- if a layer is set to invisible, it also shouldn't be the active layer anymore
		
		@type	event:	object
		@param	event:	pychan mouse event
		@type	widget:	object
		@param	widget:	the pychan widget where the event occurs, transports the layer id in it's name
		"""
		if not self.data: return
		
		layerid = widget.name[7:]
		
		layer = self._mapedit._map.getLayer(layerid)
		
		if layer.areInstancesVisible():
			layer.setInstancesVisible(False)
			self.select_no_layer()
#			self.select_different_active_layer(layerid)
		else:
			layer.setInstancesVisible(True)
			
			
	def select_no_layer(self):
		""" the exception approach - as soon as the user hides a layer, the mapedit module should stop to use this
		one, too.
		
		A bunch of exceptions is the result (each click on the map will result in a exception as no layer is set etc...)	
		"""
		self._mapedit._editLayer(None)
		
	def select_different_active_layer(self, layerid):
		""" a helper method to pick either the previous or next layer in the layerlist
		by using the given layerid as pivot element
		
		NOTE:
			- dropped for now, we set self.mapedit._layer to None if a layer gets invisible
		
		FIXME:
			- either drop this feature or find a solution for boderline cases:
				- user hides all layers (which one should be active?)
				- worst case would be that this algo has to check all layers recursive until it knows that all are invisible
				  to return with no result (no selection of an active layer, I'm not sure if FIFEdit supports that at all)
		
		@type	layerid:	string
		@param	layerid:	the layerid of the pivot element		
		"""
		layers = [layer.getId() for layer in self._mapedit._map.getLayers()]
		pivot_index = layers.index(layerid)

		if len(layers) == 1:
			return

		if pivot_index == len(layers) - 1:
			different_layer = layers[pivot_index - 1]
		else:
			different_layer = layers[pivot_index + 1]

		widget = self.container.findChild(name=_LABEL_NAME_PREFIX + different_layer)
		self.select_active_layer(None, widget)
		
	def select_active_layer(self, event, widget):
		""" callback for Labels 
		
		We hand the layerid over to the mapeditor module to select a 
		new active layer
		
		Additionally, we mark the active layer widget (changing base color) and reseting the previous one
		
		FIXME:
			- styled widgets don't accept layout changes (might be a bug in the pychan layout engine)
			- therefore we can only mark the active layer via text (I added a * to the label text)

		@type	event:	object
		@param	event:	pychan mouse event
		@type	widget:	object
		@param	widget:	the pychan widget where the event occurs, transports the layer id in it's name
		"""
		if not self.data: return
		
		if self.previous_active_layer is not None:
			previous_layer_id = str(self.previous_active_layer)
			previous_active_widget = self.container.findChild(name="select_" + previous_layer_id)
			previous_active_widget.background_color = _DEFAULT_BACKGROUND_COLOR
			previous_active_widget.foreground_color = _DEFAULT_BACKGROUND_COLOR
			previous_active_widget.base_color = _DEFAULT_BACKGROUND_COLOR
			previous_active_widget.text = previous_layer_id
		
		layerid = widget.name[7:]	
		
		widget.background_color = _HIGHLIGHT_BACKGROUND_COLOR
		widget.foreground_color = _HIGHLIGHT_BACKGROUND_COLOR
		widget.base_color = _HIGHLIGHT_BACKGROUND_COLOR
		widget.text = widget.text + " *"
		self.previous_active_layer = layerid
		self.container.adaptLayout()
		
		self._mapedit._editLayer(layerid)
