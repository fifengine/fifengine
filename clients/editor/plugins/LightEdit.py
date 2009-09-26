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

""" a tool for FIFEdit to test global light """

import fife
import pychan
import pychan.widgets as widgets
from pychan.tools import callbackWithArguments as cbwa

from fife_timer import Timer

import scripts
import scripts.plugin as plugin
from scripts.events import *
from scripts.gui.action import Action

import random

DEFAULT_GLOBAL_LIGHT = {
	"R"	:	1.0,
	"G"	:	1.0,
	"B"	:	1.0,
	"A" :	1.0,
}
DEFAULT_LIGHT_ID = "LightEdit"

class LightEdit(plugin.Plugin):
	""" The B{LightEdit} module is a plugin for FIFedit and allows to change the
	global light value
	
		FEATURES:
			- enable FIFE lighting renderer
			- generate random light values
			- test lightsetups by manipulating the color channels
			- reset to default
	"""
	def __init__(self):
		self.active = False
		
		self._renderer = None
		self._camera = None
		self._enabled = False
		self._light = False
		
		self.map_loaded = False
		
		self._color = {}
		self._color.update(DEFAULT_GLOBAL_LIGHT)
		
		random.seed()
		
		if "LightRenderer" in dir(fife):
			self._renderer_available = True
		else:
			self._renderer_available = False

	def _reset(self):
		""" 	resets all dynamic vars	"""
		pass		

	def enable(self):
		""" plugin method """
		if not self._renderer_available:
			self._enabled = False
			return
		if self._enabled is True:
			return
			
		self._editor = scripts.editor.getEditor()
		self.engine = self._editor.getEngine()
		
		self._showAction = Action(unicode(self.getName(),"utf-8"), checkable=True)
		scripts.gui.action.activated.connect(self.toggle_gui, sender=self._showAction)
		
		self._editor._tools_menu.addAction(self._showAction)
		
		events.postMapShown.connect(self.update_renderer)
		events.onMapChanged.connect(self.update_renderer)
		
		self._reset()		
		self.create_gui()

	def disable(self):
		""" plugin method """
		if self._enabled is False:
			return
			
		self._reset()
		self.container.hide()
		self.removeAllChildren()
		
		self._editor._tools_menu.removeAction(self._showAction)
		
		events.postMapShown.disconnect(self.update_renderer)
		events.onMapChanged.disconnect(self.update_renderer)

	def isEnabled(self):
		""" plugin method """
		return self._enabled;

	def getName(self):
		""" plugin method """
		return "Light editor"

	def create_gui(self):
		""" create gui container and setup callbacks """
		self.container = pychan.loadXML('gui/lightedit.xml')
		self.container.mapEvents({
			"enable_global_light"	:	self.toggle_light,
			"random_global_light"	:	self.random_color,
			"reset_global_light"	:	self.reset_global_light,
			
			"increase_R"			:	cbwa(self.increase_color, r=True),
			"decrease_R"			:	cbwa(self.decrease_color, r=True),
			"value_R/mouseWheelMovedUp"			:	cbwa(self.increase_color, step=0.2, r=True),
			"value_R/mouseWheelMovedDown"		:	cbwa(self.decrease_color, step=0.2, r=True),
			
			"increase_G"			:	cbwa(self.increase_color, g=True),
			"decrease_G"			:	cbwa(self.decrease_color, g=True),
			"value_G/mouseWheelMovedUp"			:	cbwa(self.increase_color, step=0.2, g=True),
			"value_G/mouseWheelMovedDown"		:	cbwa(self.decrease_color, step=0.2, g=True),
			
			"increase_B"			:	cbwa(self.increase_color, b=True),
			"decrease_B"			:	cbwa(self.decrease_color, b=True),
			"value_B/mouseWheelMovedUp"			:	cbwa(self.increase_color, step=0.2, b=True),
			"value_B/mouseWheelMovedDown"		:	cbwa(self.decrease_color, step=0.2, b=True),
			
			"increase_A"			:	cbwa(self.increase_color, a=True),
			"decrease_A"			:	cbwa(self.decrease_color, a=True),			
			"value_A/mouseWheelMovedUp"			:	cbwa(self.increase_color, step=0.2, a=True),
			"value_A/mouseWheelMovedDown"		:	cbwa(self.decrease_color, step=0.2, a=True),			
		})
		self._widgets = {
			"enable_global_light"	:	self.container.findChild(name="enable_global_light"),
			"random_global_light"	:	self.container.findChild(name="random_global_light"),
			"reset_global_light"	:	self.container.findChild(name="reset_global_light"),
			
			"value_R"				:	self.container.findChild(name="value_R"),
			"value_G"				:	self.container.findChild(name="value_G"),
			"value_B"				:	self.container.findChild(name="value_B"),
			"value_A"				:	self.container.findChild(name="value_A"),			
		}
		
	def toggle_gui(self):
		""" show / hide the gui """
		if self.active:
			self.active = False
			if self.container.isVisible() or self.container.isDocked():
				self.container.setDocked(False)
				self.container.hide()
			self._showAction.setChecked(False)
		else:
			self.active = True
			self._showAction.setChecked(True)
			self.container.show()
			
	def toggle_light(self):
		""" toggle light on / off """
		if not self._renderer:
			self._widgets['enable_global_light']._setToggled(False)
			return
		
		if self._light:
			self._light = False
			self.reset_global_light()
			self._renderer.setEnabled(False)
		else:
			self._light = True
			self._renderer.setEnabled(True)
			
	def update_renderer(self):
		""" sets current camera and renderer
			bound to FIFedit core (updated on map change)
		"""
		self._camera = self._editor.getActiveMapView().getCamera()
		self._renderer = fife.LightRenderer.getInstance(self._camera)
		
	def update_gui(self):
		""" update gui widgets according to plugin data """
		self._widgets["value_R"].text = unicode(str(self._color["R"]))
		self._widgets["value_G"].text = unicode(str(self._color["G"]))
		self._widgets["value_B"].text = unicode(str(self._color["B"]))
		self._widgets["value_A"].text = unicode(str(self._color["A"]))
						
	def reset_global_light(self):
		""" reset global light to default values (1.0) """
		if not self._renderer: return

		self._color.update(DEFAULT_GLOBAL_LIGHT)
		self.update_gui()
		self.set_global_light()
			
	def increase_color(self, step=0.1, r=None, g=None, b=None, a=None):
		"""	increase a given color value by step value
		
		@type	step	float
		@param	step	the step for changing the color channel
		@type	r		bool
		@param	r		flag to alter red color value
		@type	g		bool
		@param	g		flag to alter green color value
		@type	b		bool
		@param	b		flag to alter blue color value
		@type	a		bool
		@type	a		flag to alter alpha channel value (no effect atm)		
		"""
		if r:
			self._color["R"] += step
		if g:
			self._color["G"] += step
		if b:
			self._color["B"] += step
		if a:
			self._color["A"] += step

		self.update_gui()					
		self.set_global_light()
			
	def decrease_color(self, step=0.1, r=None, g=None, b=None, a=None):
		"""	decrease a given color value by step value
		
		@type	step	float
		@param	step	the step for changing the color channel
		@type	r		bool
		@param	r		flag to alter red color value
		@type	g		bool
		@param	g		flag to alter green color value
		@type	b		bool
		@param	b		flag to alter blue color value
		@type	a		bool
		@type	a		flag to alter alpha channel value (no effect atm)		
		"""
		if r:
			self._color["R"] -= step
		if g:
			self._color["G"] -= step
		if b:
			self._color["B"] -= step
		if a:
			self._color["A"] -= step
			
		self.update_gui()					
		self.set_global_light()
			
	def random_color(self):
		""" generate random values for color channels """
		if not self._renderer: return	
		
		self._color["R"] = random.uniform(0,2)
		self._color["G"] = random.uniform(0,2)
		self._color["B"] = random.uniform(0,2)
		self._color["A"] = random.uniform(0,2)

		self.update_gui()					
		self.set_global_light()
		
	def set_global_light(self):
		""" update the global light with the current set colors """
		if not self._renderer: return

		self._renderer.removeAll(DEFAULT_LIGHT_ID)
		self._renderer.setglobalLight(
			DEFAULT_LIGHT_ID,
			1,
			self._color["R"],
			self._color["G"],
			self._color["B"],
			self._color["A"]
		)
		
