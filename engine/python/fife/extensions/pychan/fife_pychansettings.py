# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
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

from fife.extensions.fife_settings import Setting
from fife.extensions.fife_settings import SettingEntry
from fife.extensions import pychan

SETTINGS_GUI_XML="""\
<Window name="Settings" title="Settings">
	<Label text="Settings menu!" />
	<HBox>
		<VBox>
			<Label text="Resolution:" />
			<Label text="Renderer:" />
		</VBox>
		<VBox min_size="120,60">
			<DropDown name="screen_resolution" min_size="120,0" />
			<DropDown name="render_backend" min_size="120,0" />
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

class FifePychanSettings(Setting):
	def __init__(self, app_name="", settings_file="", default_settings_file= "settings-dist.xml", settings_gui_xml="", changes_gui_xml="", copy_dist=True, serializer=None):
		
		super(FifePychanSettings, self).__init__(app_name, settings_file, default_settings_file, copy_dist, serializer)
		
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
		
	def createAndAddEntry(self, module, name, widgetname, applyfunction=None, initialdata=None, requiresrestart=False):
		""""
		@param module: The Setting module this Entry belongs to
		@type module: C{String}
		@param name: The Setting's name
		@type name: C{String}
		@param widgetname: The name of the widget that is used to change this
		setting
		@type widgetname: C{String}
		@param applyfunction: function that makes the changes when the Setting is
		saved
		@type applyfunction: C{function}
		@param initialdata: If the widget supports the setInitialData() function
		this can be used to set the initial data
		@type initialdata: C{String} or C{Boolean}
		@param requiresrestart: Whether or not the changing of this setting
		requires a restart
		@type requiresrestart: C{Boolean}
		"""
		entry = PychanSettingEntry(module, name, widgetname, applyfunction, initialdata, requiresrestart)
		self.addEntry(entry)
		

	def _initDefaultSettingEntries(self):
		"""Initializes the default fife setting entries. Not to be called from
		outside this class."""
		self.createAndAddEntry(FIFE_MODULE, "PlaySounds", "enable_sound",
		              requiresrestart=True)
		self.createAndAddEntry(FIFE_MODULE, "FullScreen", "enable_fullscreen",
		              requiresrestart=True)
		self.createAndAddEntry(FIFE_MODULE, "ScreenResolution", "screen_resolution", initialdata = self._resolutions,
		              requiresrestart=True)
		self.createAndAddEntry(FIFE_MODULE, "RenderBackend", "render_backend", initialdata = self._renderbackends,
		              requiresrestart=True)
		
	# sets valid resolution options in the settings->Resolution
	def setValidResolutions(self, options):
		if options:
			self._resolutions = options
		self.createAndAddEntry(FIFE_MODULE, "ScreenResolution", "screen_resolution", initialdata = self._resolutions,
		              requiresrestart=True)

		              
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
		
		self.fillWidgets()
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
			'defaultButton': self.setDefaults
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

	def fillWidgets(self):
		for module in self._entries.itervalues():
			for entry in module.itervalues():
				widget = self._optionsDialog.findChildByName(entry.settingwidgetname)
				
				"""
				little change to prevent crash from no settings
				in settings.xml file
				"""
				"""
				The checking of value for None is specially for the clients who use settings
				with different names under modules other than "FIFE" for which we have no
				default value to set. This will prevent the settings widget from crash
				"""
				value = self.get(entry.module, entry.name)
				
				if isinstance(entry.initialdata, list):
					try:
						value = entry.initialdata.index(value)
					except ValueError:
						raise ValueError("\"" + str(value) + "\" is not a valid value for " + entry.name + ". Valid options: " + str(entry.initialdata))
				elif isinstance(entry.initialdata, dict):
					try:
						value = entry.initialdata.keys().index(value)
					except ValueError:
						raise ValueError("\"" + str(value) + "\" is not a valid value for " + entry.name + ". Valid options: " + str(entry.initialdata.keys()))
				entry.initializeWidget(widget, value)
	
	def _applySettings(self):
		"""
		Writes the settings file.  If a change requires a restart of the engine
		it notifies you with a small dialog box.
		"""
		for module in self._entries.itervalues():
			for entry in module.itervalues():
				widget = self._optionsDialog.findChildByName(entry.settingwidgetname)
				data = widget.getData()
				
				# If the data is a list we need to get the correct selected data
				# from the list. This is needed for e.g. dropdowns or listboxs
				if type(entry.initialdata) is list:
					data = entry.initialdata[data]
				elif isinstance(entry.initialdata, dict):
					data = entry.initialdata.keys()[data]

				# only take action if something really changed
				if data != self.get(entry.module, entry.name):
					self.set(entry.module, entry.name, data)
					entry.onApply(data)

					if entry.requiresrestart:
						self.changesRequireRestart = True

		super(FifePychanSettings, self).saveSettings()

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
		
	def setDefaults(self):
		super(FifePychanSettings, self).setDefaults()
		
		#On startup the settings dialog is not yet initialized.  We dont
		#fill the widgets with data in that case.
		if self._optionsDialog:
			self.fillWidgets()
			
class PychanSettingEntry(SettingEntry):
	
	def __init__(self, module, name, widgetname, applyfunction=None, initialdata=None, requiresrestart=False):
		super(PychanSettingEntry, self).__init__(module, name, applyfunction, initialdata, requiresrestart)
		
		self._settingwidgetname = widgetname
		
	def initializeWidget(self, widget, currentValue):
		"""Initialize the widget with needed data"""
		if self._initialdata is not None:
			widget.setInitialData(self._initialdata)
		widget.setData(currentValue)
	
	def _getSettingWidgetName(self):
		return self._settingwidgetname

	def _setSettingWidgetName(self, settingwidgetname):
		self._settingwidgetname = settingwidgetname
		
	settingwidgetname = property(_getSettingWidgetName, _setSettingWidgetName)
	
	def __str__(self):
		return "SettingEntry: " +  self.name + " Module: " + self.module + " Widget: " + \
		       self.settingwidgetname + " requiresrestart: " + str(self.requiresrestart) + \
		       " initialdata: " + str(self.initialdata)

	
