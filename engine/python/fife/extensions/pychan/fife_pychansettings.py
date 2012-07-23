# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
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

"""
PychanSettings
==================================

This module provides gui window that sets basic fife settings through
pychan.
"""

import os
from StringIO import StringIO

from fife.extensions import fife_settings
from fife.extensions import pychan

SETTINGS_GUI_XML="""\
<Window name="Settings" title="Settings">
	<Label text="Settings menu!" />
	<HBox>
		<VBox>
			<Label text="Resolution:" />
			<Label text="Renderer:" />
			<Label text="Light Model:" />
		</VBox>
		<VBox min_size="120,60">
			<DropDown name="screen_resolution" min_size="120,0" />
			<DropDown name="render_backend" min_size="120,0" />
			<DropDown name="lighting_model" min_size="120,0" />
		</VBox>
	</HBox>
	<CheckBox name="enable_fullscreen" text="Use the full screen mode" />
	<CheckBox name="enable_sound" text="Enable sound" />
	<HBox>
		<Spacer />
		<Button name="cancelButton" text="Cancel" />
		<Button name="okButton" text="Ok" />
		<Button name="defaultButton" text="Defaults" />
	</HBox>
</Window>
"""

CHANGES_REQUIRE_RESTART="""\
<Window title="Changes require restart">
	<Label text="Some of your changes require you to restart." />
	<HBox>
		<Spacer />
		<Button name="closeButton" text="Ok" />
	</HBox>
</Window>
"""

FIFE_MODULE = "FIFE"

class FifePychanSettings(object):
	def __init__(self, fife_settings, settings_gui_xml = "", changes_gui_xml = ""):
		self.fife_settings = fife_settings
		self._optionsDialog = None
		self._gui_style = "default"
		self.changesRequireRestart = False
		
		if settings_gui_xml == "":
			self._settings_gui_xml = SETTINGS_GUI_XML
		else:
			self._settings_gui_xml = settings_gui_xml
			
		if changes_gui_xml == "":
			self._changes_gui_xml = CHANGES_REQUIRE_RESTART
		else:
			self._changes_gui_xml = changes_gui_xml
			
		self.mapped_entries = { }
		self._setDefaultWidgetNames()
	
	def _setDefaultWidgetNames(self):
		self.mapWidgetToEntry(FIFE_MODULE, "PlaySounds", "enable_sound")
		self.mapWidgetToEntry(FIFE_MODULE, "FullScreen", "enable_fullscreen")
		self.mapWidgetToEntry(FIFE_MODULE, "ScreenResolution", "screen_resolution")
		self.mapWidgetToEntry(FIFE_MODULE, "RenderBackend", "render_backend")
		self.mapWidgetToEntry(FIFE_MODULE, "Lighting", "lighting_model")
		#print self.mapped_entries
		
	def mapWidgetToEntry(self, module, entry, widget_name):
		"""
		Maps a setting entry to a widget name
		@param module: Module of the setting
		@type module: C{string}
		@param entry: Entry of the setting
		@type entry: C{string}
		@param widget_name: Name of the setting's widget
		@type widget_name: C{string}
		"""
		#print 'Entry: ', entry, ' Module: ' , module, ' Exists: ', entry in self.fife_settings.entries[module]
		if entry in self.fife_settings.entries[module]:
			if module not in self.mapped_entries:
				self.mapped_entries[module] = { }
			self.mapped_entries[module][entry] = widget_name

	def setGuiStyle(self, style):
		""" 
		Set a custom gui style used for the option dialog.
		@param style: Pychan style to be used
		@type style: C{string}      
		"""
		self._gui_style = style
		
	def showSettingsDialog(self):
		"""
		Shows the default settings dialog.
		"""
		self.changesRequireRestart = False
		self.isSetToDefault = False
		if not self._optionsDialog:
			self._loadSettingsDialog()
		
		self._fillWidgets()
		self._optionsDialog.show()
		
	def _loadSettingsDialog(self):
		"""
		Loads up the .xml with the default settings dialog.
		"""
		self._optionsDialog = self._loadWidget(self._settings_gui_xml)
		self._optionsDialog.stylize(self._gui_style)
		self._optionsDialog.mapEvents({
			'okButton' : self._applySettings,
			'cancelButton': self._optionsDialog.hide,
			'defaultButton': self._setDefaults
		})
		return self._optionsDialog
		
	def _loadWidget(self, widget):
		"""
		Loads a widget
		"""
		if os.path.isfile(self._settings_gui_xml):
			return pychan.loadXML(widget)
		else:
			return pychan.loadXML(StringIO(widget))

	def _fillWidgets(self):
		for module, entries in self.fife_settings.entries.items():
			for entry in entries.itervalues():
				if entry.name in self.mapped_entries[module]:
					widget = self._optionsDialog.findChildByName(self.mapped_entries[module][entry.name])
					"""
					little change to prevent crash from no settings
					in settings.xml file
					"""
					"""
					The checking of value for None is specially for the clients who use settings
					with different names under modules other than "FIFE" for which we have no
					default value to set. This will prevent the settings widget from crash
					"""
					value = self.fife_settings.get(entry.module, entry.name)
					#print entry.name, entry.initialdata
					if type(entry.initialdata) is list:
						try:
							value = entry.initialdata.index(value)
						except ValueError:
							raise ValueError("\"" + str(value) + "\" is not a valid value for " + entry.name + ". Valid options: " + str(entry.initialdata))
				
					if entry.initialdata is not None:
						widget.setInitialData(entry.initialdata)
					widget.setData(value)
	
	def _applySettings(self):
		"""
		Writes the settings file.  If a change requires a restart of the engine
		it notifies you with a small dialog box.
		"""
		for module, entries in self.fife_settings.entries.items():
			for entry in entries.itervalues():
				if entry.name in self.mapped_entries[module]:
					widget = self._optionsDialog.findChildByName(self.mapped_entries[module][entry.name])
					data = widget.getData()
				
					# If the data is a list we need to get the correct selected data
					# from the list. This is needed for e.g. dropdowns or listboxs
					if type(entry.initialdata) is list:
						data = entry.initialdata[data]

					# only take action if something really changed
					if data != self.fife_settings.get(entry.module, entry.name):
						self.fife_settings.set(entry.module, entry.name, data)
						entry.onApply(data)
						if entry.requiresrestart:
							self.changesRequireRestart = True

		self.fife_settings.saveSettings()

		self._optionsDialog.hide()
		if self.changesRequireRestart:
			self._showChangeRequireRestartDialog()
			
	def _showChangeRequireRestartDialog(self):
		"""Shows a dialog that informes the user that a restart is required
		to perform the changes."""
		RestartDlg = self._loadWidget(self._changes_gui_xml)
		RestartDlg.stylize(self._gui_style)
		RestartDlg.mapEvents({ 'closeButton' : RestartDlg.hide })
		RestartDlg.show()
		
	def _setDefaults(self):
		self.fife_settings.setDefaults()
		
		#On startup the settings dialog is not yet initialized.  We dont
		#fill the widgets with data in that case.
		if self._optionsDialog:
			self._fillWidgets()