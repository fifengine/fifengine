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
Settings
==================================

This module provides a nice framework for loading and saving game settings.
It is by no means complete but it does provide a good starting point.
"""

import shutil
import os
from StringIO import StringIO

from fife.extensions import pychan
from fife.extensions.fife_utils import getUserDataDirectory
from fife.extensions.serializers.simplexml import SimpleXMLSerializer

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

class Setting(object):
	"""
	This class manages loading and saving of game settings.

	Usage::
		from fife.extensions.fife_settings import Setting
		settings = Setting(app_name="myapp")
		screen_width = settings.get("FIFE", "ScreenWidth", 1024)
		screen_height = settings.get("FIFE", "ScreenHeight", 768)
	"""

	def __init__(self, app_name="", settings_file="", default_settings_file= "settings-dist.xml", settings_gui_xml="", changes_gui_xml="", copy_dist=True, serializer=None):
		"""
		Initializes the Setting object.

		@param app_name: The applications name.  If this parameter is provided
		alone it will try to read the settings file from the users home directory.
		In windows this will be	something like:	C:\Documents and Settings\user\Application Data\fife
		@type app_name: C{string}
		@param settings_file: The name of the settings file.  If this parameter is
		provided it will look for the setting file as you specify it, first looking
		in the working directory.  It will NOT look in the users home directory.
		@type settings_file: C{string}
		@param default_settings_file: The name of the default settings file.  If the settings_file
		does not exist this file will be copied into the place of the settings_file.  This file
		must exist in the root directory of your project!
		@type default_settings_file: C{string}
		@param settings_gui_xml: If you specify this parameter you can customize the look
		of the settings dialog box.
		@param copy_dist: Copies the default settings file to the settings_file location.  If
		this is False it will create a new empty setting file.
		@param serializer: Overrides the default XML serializer
		@type serializer: C{SimpleSerializer}

		"""
		self._app_name = app_name
		self._settings_file = settings_file
		self._default_settings_file = default_settings_file
		self._settings_gui_xml = settings_gui_xml
		self._changes_gui_xml = changes_gui_xml
		self.OptionsDlg = None

		# Holds SettingEntries
		self._entries = {}
		
		if self._settings_file == "":
			self._settings_file = "settings.xml"
			self._appdata = getUserDataDirectory("fife", self._app_name)
		else:
			self._appdata = os.path.dirname(self._settings_file)
			self._settings_file = os.path.basename(self._settings_file)


		if self._settings_gui_xml == "":
			self._settings_gui_xml = SETTINGS_GUI_XML

		if self._changes_gui_xml == "":
			self._changes_gui_xml = CHANGES_REQUIRE_RESTART


		if not os.path.exists(os.path.join(self._appdata, self._settings_file)):
			if os.path.exists(self._default_settings_file) and copy_dist:
				shutil.copyfile(self._default_settings_file, os.path.join(self._appdata, self._settings_file))

		#default settings
		self._resolutions = ['640x480', '800x600', '1024x768', '1280x800', '1440x900']
		self._renderbackends = ['OpenGL', 'SDL']
		self._lightingmodels = [0, 1, 2]

		#Used to stylize the options gui
		self._gui_style = "default"

		#Initialize the serializer
		if serializer:
			self._serializer = serializer
		else:
			self._serializer = SimpleXMLSerializer()
	
		self.initSerializer()
	
		self._initDefaultSettingEntries()

	def initSerializer(self):
		self._serializer.load(os.path.join(self._appdata, self._settings_file))
		
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
		self.createAndAddEntry(FIFE_MODULE, "Lighting", "lighting_model", initialdata = self._lightingmodels,
		              requiresrestart=True)

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
		entry = SettingEntry(module, name, widgetname, applyfunction, initialdata, requiresrestart)
		self.addEntry(entry)

	def addEntry(self, entry):
		"""Adds a new C{SettingEntry} to the Settting
		@param entry: A new SettingEntry that is to be added
		@type entry: C{SettingEntry}
		"""
		if entry.module not in self._entries:
			self._entries[entry.module] = {}
		self._entries[entry.module][entry.name] = entry

		# Make sure the new entry is available
		if self.get(entry.module, entry.name) is None:
			print "Updating", self._settings_file, "to the default, it is missing the entry:"\
			      , entry.name ,"for module", entry.module
			self.setDefaults()
		if self.get(entry.module, entry.name) is None:
			print "WARNING:", entry.module, ":", entry.name, "still not found!"


	def saveSettings(self, filename=""):
		""" Writes the settings to the settings file 
		
		@param filename: Specifies the file to save the settings to.  If it is not specified
		the original settings file is used.
		@type filename: C{string}
		"""
		if self._serializer:
			if filename == "":
				self._serializer.save(os.path.join(self._appdata, self._settings_file))
			else:
				self._serializer.save(filename)

	def get(self, module, name, defaultValue=None):
		""" Gets the value of a specified setting

		@param module: Name of the module to get the setting from
		@param name: Setting name
		@param defaultValue: Specifies the default value to return if the setting is not found
		@type defaultValue: C{str} or C{unicode} or C{int} or C{float} or C{bool} or C{list} or C{dict}
		"""
		if self._serializer:
			return self._serializer.get(module, name, defaultValue)
		else:
			return None
	
	def set(self, module, name, value, extra_attrs={}):
		"""
		Sets a setting to specified value.

		@param module: Module where the setting should be set
		@param name: Name of setting
		@param value: Value to assign to setting
		@type value: C{str} or C{unicode} or C{int} or C{float} or C{bool} or C{list} or C{dict}
		@param extra_attrs: Extra attributes to be stored in the XML-file
		@type extra_attrs: C{dict}
		"""
		if self._serializer:
			self._serializer.set(module, name, value, extra_attrs)

	def setGuiStyle(self, style):
		""" Set a custom gui style used for the option dialog.
		@param style: Pychan style to be used
		@type style: C{string}
		"""
		self._gui_style = style
		
	def onOptionsPress(self):
		"""
		Opens the options dialog box.  Usually you would bind this to a button.
		"""
		self.changesRequireRestart = False
		self.isSetToDefault = False
		if not self.OptionsDlg:
			self.loadSettingsDialog()
		self.fillWidgets()
		self.OptionsDlg.show()

	def loadSettingsDialog(self):
		"""
		Load up the settings xml and return the widget.
		"""
		self.OptionsDlg = self._loadWidget(self._settings_gui_xml)
		self.OptionsDlg.stylize(self._gui_style)
		self.OptionsDlg.mapEvents({
			'okButton' : self.applySettings,
			'cancelButton' : self.OptionsDlg.hide,
			'defaultButton' : self.setDefaults
		})
		return self.OptionsDlg

	def _loadWidget(self, dialog):
		"""Loads a widget. Can load both files and pure xml strings"""
		if os.path.isfile(self._settings_gui_xml):
			return pychan.loadXML(dialog)
		else:
			return pychan.loadXML(StringIO(dialog))

	def fillWidgets(self):
		for module in self._entries.itervalues():
			for entry in module.itervalues():
				widget = self.OptionsDlg.findChildByName(entry.settingwidgetname)
				value = self.get(entry.module, entry.name)
				if type(entry.initialdata) is list:
					try:
						value = entry.initialdata.index(value)
					except ValueError:
						raise ValueError("\"" + value + "\" is not a valid value for " + entry.name + ". Valid options: " + str(entry.initialdata))
				entry.initializeWidget(widget, value)

	def applySettings(self):
		"""
		Writes the settings file.  If a change requires a restart of the engine
		it notifies you with a small dialog box.
		"""
		for module in self._entries.itervalues():
			for entry in module.itervalues():
				widget = self.OptionsDlg.findChildByName(entry.settingwidgetname)
				data = widget.getData()

				# If the data is a list we need to get the correct selected data
				# from the list. This is needed for e.g. dropdowns or listboxs
				if type(entry.initialdata) is list:
					data = entry.initialdata[data]

				# only take action if something really changed
				if data != self.get(entry.module, entry.name):
					self.set(entry.module, entry.name, data)
					entry.onApply(data)

					if entry.requiresrestart:
						self.changesRequireRestart = True

		self.saveSettings()

		self.OptionsDlg.hide()
		if self.changesRequireRestart:
			self._showChangeRequireRestartDialog()


	def _showChangeRequireRestartDialog(self):
		"""Shows a dialog that informes the user that a restart is required
		to perform the changes."""
		RestartDlg = self._loadWidget(self._changes_gui_xml)
		RestartDlg.stylize(self._gui_style)
		RestartDlg.mapEvents({ 'closeButton' : RestartDlg.hide })
		RestartDlg.show()


	def setAvailableScreenResolutions(self, reslist):
		"""
		A list of valid default screen resolutions.   This should be called once
		right after you instantiate Settings.

		Valid screen resolutions must be strings in the form of: WIDTHxHEIGHT

		Example:
			settings.setAvailableScreenResolutions(["800x600", "1024x768"])
		"""
		self._resolutions = reslist

	def setDefaults(self):
		"""
		Overwrites the setting file with the default settings file.
		"""
		shutil.copyfile(self._default_settings_file, os.path.join(self._appdata, self._settings_file))
		self.changesRequireRestart = True
		self.initSerializer()
		
		#update all widgets with the new data
		self.fillWidgets()

	def _getEntries(self):
		return self._entries

	def _setEntries(self, entries):
		self._entries = entries
		
	def _getSerializer(self):
		return self._serializer

	entries = property(_getEntries, _setEntries)
	serializer = property(_getSerializer)

class SettingEntry(object):

	def __init__(self, module, name, widgetname, applyfunction=None, initialdata=None, requiresrestart=False):
		"""
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
		self._module = module
		self._name = name
		self._settingwidgetname = widgetname
		self._requiresrestart = requiresrestart
		self._initialdata = initialdata
		self._applyfunction = applyfunction

	def initializeWidget(self, widget, currentValue):
		"""Initialize the widget with needed data"""
		if self._initialdata is not None:
			widget.setInitialData(self._initialdata)
		widget.setData(currentValue)

	def onApply(self, data):
		"""Implement actions that need to be taken when the setting is changed
		here.
		"""
		if self._applyfunction is not None:
			self._applyfunction(data)

	def _getModule(self):
		return self._module

	def _setModule(self, module):
		self._module = module

	def _getName(self):
		return self._name

	def _setName(self, name):
		self._name = name

	def _getSettingWidgetName(self):
		return self._settingwidgetname

	def _setSettingWidgetName(self, settingwidgetname):
		self._settingwidgetname = settingwidgetname

	def _getRequiresRestart(self):
		return self._requiresrestart

	def _setRequiresRestart(self, requiresrestart):
		self._requiresrestart = requiresrestart

	def _getInitialData(self):
		return self._initialdata

	def _setInitialData(self, initialdata):
		self._initialdata = initialdata

	def _getApplyFunction(self):
		return self._applyfunction

	def _setApplyFunction(self, applyfunction):
		self._applyfunction = applyfunction

	module = property(_getModule, _setModule)
	name = property(_getName, _setName)
	settingwidgetname = property(_getSettingWidgetName, _setSettingWidgetName)
	requiresrestart = property(_getRequiresRestart, _setRequiresRestart)
	initialdata = property(_getInitialData, _setInitialData)
	applyfunction = property(_getApplyFunction, _setApplyFunction)

	def __str__(self):
		return "SettingEntry: " +  self.name + " Module: " + self.module + " Widget: " + \
		       self.settingwidgetname + " requiresrestart: " + str(self.requiresrestart) + \
		       " initialdata: " + str(self.initialdata)


