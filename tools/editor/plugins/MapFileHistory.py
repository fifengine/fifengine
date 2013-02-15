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

""" a simple mapfile history plugin

	stores the last used mapfiles in the editors setting.xml file
	(uses fife settings extension)

	Usage:
		- nothing special to do here, clicking on a list entry triggers
		  a mapload (if possible)
		- the plugin pretty much takes care about itself
		- some enhanced docking mechanism are at work to use the editors
		  settings file to save and load previous docking states
"""

import os

from fife import fife
from fife.extensions import pychan

import scripts.plugin as plugin
import scripts.editor

from scripts.editor import TDS
from scripts.gui.action import Action
from scripts.gui.panel import Panel
from scripts.events import onOpenMapFile

# the number of entries in settings.xml / settings-dist.xml
# altering this value only has an effect if the settings files provide the same number
_HISTORY_LEN = 5
# module name in settings file
_MODULE = "MapHistory"
_ITEM_PREFIX = "Mapfile_"

# settings for gui
_POSITION = (30,200)
_MIN_SIZE = (110,20)

# plugin default settings
_PLUGIN_SETTINGS = {
	'module' : "MapHistorySettings",
	'items' : {
		'dockarea' : 'left',
		'docked' : True,
		
	},
}

class MapFileHistory(plugin.Plugin):
	""" The B{MapFileHistory} uses the fife_settings module to store the last
		used mapfiles and provides the means to both display them and load them
	"""
	def __init__(self):
		super(MapFileHistory, self).__init__()
		# editor instance
		self._editor = scripts.editor.getEditor()

		# Plugin variables
		self._enabled = False
		self._action_show = None
		self.history = []
		self.names = []
		
		# gui vars
		self.container = None
		self.default_x, self.default_y = _POSITION
		
		self.default_settings = _PLUGIN_SETTINGS
		self.eds = self._editor._settings
		self.update_settings()

	def enable(self):
		""" enables the plugin and connects to the editor """
		if self._enabled: return
		
		self._enabled = True
			
		# Fifedit plugin data
		self._action_show = Action(self.getName(), checkable=True)
		scripts.gui.action.activated.connect(self.toggle, sender=self._action_show)
		self._editor._tools_menu.addAction(self._action_show)

		self.load_map_history()
		self.create()		
		self.toggle()
		
		onOpenMapFile.connect(self.update)
		
		if self.settings['docked']:
			self._editor.dockWidgetTo(self.container, self.settings['dockarea'])
	
	def disable(self):
		""" deactivates plugin """
		if not self._enabled: return
		self._enabled = False
		
		onOpenMapFile.disconnect(self.update)		
		self._editor._tools_menu.removeAction(self._action_show)

	def isEnabled(self):
		return self._enabled

	def getName(self):
		return u"Mapfile history"
		
	#--- These are not so important ---#
	def getAuthor(self):
		return "Zero-Projekt"
		
	def getDescription(self):
		return ""

	def getLicense(self):
		return ""
		
	def getVersion(self):
		return "0.1"
		
	def load_map(self, event=None, widget=None):
		""" load the the selected map """
		index = widget.selected
		
		path = self.history[index]

		# make sure the mapfile exists, otherwise delete the entry
		if not self._editor.engine.getVFS().exists(path):
			self.delete_entry(path)
			self.update_gui()
			return
		
		self._editor.openFile(path)
		
	def load_map_history(self):
		""" loads the mapfile history from settings file """		
		for i in range(_HISTORY_LEN):
			entry = _ITEM_PREFIX+str(i)
			item = self.eds.get(_MODULE, entry)
			self.add_entry(item)
		
	def add_entry(self, path, save=False):
		""" adds an entry to the history
		
		@type	path:	str
		@param	path:	path to map file
		@type	save:	bool
		@param	save:	flag to either save entry to settings file or not
		"""
		if not path: return
		
		if path in self.history: return
		
		cur_len = len(self.history)

		parts = path.split(os.sep)
		name = parts[-1]

		if cur_len >= _HISTORY_LEN:
			self.history[0] = path
			self.names[0] = name
		elif cur_len < _HISTORY_LEN:
			self.history.append(path)
			self.names.append(name)
		
		if save:
			index = self.history.index(path)			
			self.eds.set(_MODULE, _ITEM_PREFIX+str(index), path)
		
	def delete_entry(self, path):
		""" delete an (outdated) entry from the history
		
		@type	path:	str
		@param	path:	path to map file
		"""
		if path not in self.history: return
		
		index = self.history.index(path)
		
		del self.history[index]
		del self.names[index]
		
		self.eds.set(_MODULE, _ITEM_PREFIX+str(index), '')			
		
	def create(self):
		""" creates the gui """
		if self.container is not None: return
		
		self.container = Panel(title=self.getName())
		self.container.position_technique = 'explicit'
		self.container.position = _POSITION
		self.container.min_size = _MIN_SIZE
		self.container.name = _MODULE
		
		self.lst_box = pychan.widgets.ListBox()
		self.lst_box.items = self.names
		self.lst_box.capture(self.load_map)
		
		self.container.addChild(self.lst_box)

		# overwrite Panel.afterUndock
		self.container.afterUndock = self.on_undock
		self.container.afterDock = self.on_dock
		
	def on_dock(self):
		""" callback for dock event of B{Panel}	widget """
		side = self.container.dockarea.side
		if not side: return

		module = self.default_settings['module']
		self.eds.set(module, 'dockarea', side)
		self.eds.set(module, 'docked', True)
	
	def on_undock(self):
		""" callback for undock event of B{Panel} widget """
		self.container.hide()
		self.toggle()

		module = self.default_settings['module']
		self.eds.set(module, 'dockarea', '')
		self.eds.set(module, 'docked', False)		

	def toggle(self):
		""" shows / hides the gui """
		if self.container.isVisible():
			self.last_dockarea = self.container.dockarea
			self.container.hide()
			self._action_show.setChecked(False)			
		else:
			if not self.container.isDocked():
				self.container.show()
				self.container.x = self.default_x
				self.container.y = self.default_y
			else:
				self.container.setDocked(True)
				self.dockWidgetTo(self.container, self.last_dockarea)
			self._action_show.setChecked(True)			
			
	def update_gui(self):
		""" refresh the listbox and update the container layout """
		self.lst_box.items = self.names
		self.container.adaptLayout()
			
	def update(self, path):
		""" updates the history with a new mapfile """
		self.add_entry(path, save=True)
		self.update_gui()		
		
		
