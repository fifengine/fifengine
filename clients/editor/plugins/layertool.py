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
			layer_name_widget.name = "select_" + layerid
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
		else:
			layer.setInstancesVisible(True)
			
		
	def select_active_layer(self, event, widget):
		""" callback for Labels 
		
		We hand the layerid over to the mapeditor module to select a 
		new active layer

		@type	event:	object
		@param	event:	pychan mouse event
		@type	widget:	object
		@param	widget:	the pychan widget where the event occurs, transports the layer id in it's name
		"""
		if not self.data: return
		
		layerid = widget.name[7:]	
		self._mapedit._editLayer(layerid)
