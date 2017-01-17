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
Settings
==================================

This module provides a nice framework for loading and saving game settings.
It is by no means complete but it does provide a good starting point.
"""

import shutil
import os
from StringIO import StringIO

from fife.extensions import fifelog
from fife.extensions.fife_utils import getUserDataDirectory
from fife.extensions.serializers.simplexml import SimpleXMLSerializer

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

	def __init__(self, app_name="", settings_file="", default_settings_file= "settings-dist.xml", copy_dist=True, serializer=None):
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
		@param copy_dist: Copies the default settings file to the settings_file location.  If
		this is False it will create a new empty setting file.
		@param serializer: Overrides the default XML serializer
		@type serializer: C{SimpleSerializer}

		"""
		self._app_name = app_name
		self._settings_file = settings_file
		self._default_settings_file = default_settings_file

		# Holds SettingEntries
		self._entries = {}

		if self._settings_file == "":
			self._settings_file = "settings.xml"
			self._appdata = getUserDataDirectory("fife", self._app_name)
		else:
			self._appdata = os.path.dirname(self._settings_file)
			self._settings_file = os.path.basename(self._settings_file)

		if not os.path.exists(os.path.join(self._appdata, self._settings_file)):
			if os.path.exists(self._default_settings_file) and copy_dist:
				shutil.copyfile(self._default_settings_file, os.path.join(self._appdata, self._settings_file))

		# valid values possible for the engineSettings
		self._validSetting = {}
		self._validSetting['FIFE'] = {
			'FullScreen':[True,False], 'RefreshRate':[0,200], 'Display':[0,9], 'VSync':[True,False], 'PychanDebug':[True,False]
			, 'ProfilingOn':[True,False], 'SDLRemoveFakeAlpha':[True,False], 'GLCompressImages':[False,True], 'GLUseFramebuffer':[False,True], 'GLUseNPOT':[False,True],
			'GLUseMipmapping':[False,True], 'GLTextureFiltering':['None', 'Bilinear', 'Trilinear', 'Anisotropic'], 'GLUseMonochrome':[False,True],
			'GLUseDepthBuffer':[False,True], 'GLAlphaTestValue':[0.0,1.0],
			'RenderBackend':['OpenGL', 'SDL'],
			'ScreenResolution':['640x480', '800x600', '1024x600', '1024x768', '1280x768',
								'1280x800', '1280x960', '1280x1024', '1366x768', '1440x900',
								'1600x900', '1600x1200', '1680x1050', '1920x1080', '1920x1200'],
			'BitsPerPixel':[0,16,24,32],
			'InitialVolume':[0.0,10.0], 'WindowTitle':"", 'WindowIcon':"", 'Font':"",
			'FontGlyphs':"", 'DefaultFontSize':"", 'Lighting':[0,1],
			'ColorKeyEnabled':[True,False], 'ColorKey':['a','b','c'], 'VideoDriver':"", 'RenderDriver':"",
			'PlaySounds':[True,False], 'LogToFile':[True,False],
			'LogToPrompt':[True,False], 'LogLevelFilter':[0,1,2,3],
			'LogModules':['all', 'controller','script','video','audio','loaders','vfs','pool','view','model','metamodel','event_channel','xml'],
			'FrameLimitEnabled':[True,False], 'FrameLimit':[0], 'MouseSensitivity':[0.0], 'MouseAcceleration':[True,False]
			}

		glyphDft = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&amp;`'*#=[]\\\""

		# we at this point assume default values are final values for engineSettings
		self._defaultSetting = {}
		self._defaultSetting['FIFE'] = {
			'FullScreen':False, 'RefreshRate':60, 'Display':0, 'VSync':False, 'PychanDebug':False,
			'ProfilingOn':False, 'SDLRemoveFakeAlpha':False, 'GLCompressImages':False, 'GLUseFramebuffer':True, 'GLUseNPOT':True,
			'GLUseMipmapping':False, 'GLTextureFiltering':'None', 'GLUseMonochrome':False, 'GLUseDepthBuffer':False, 'GLAlphaTestValue':0.3,
			'RenderBackend':'OpenGL', 'ScreenResolution':"1024x768", 'BitsPerPixel':0,
			'InitialVolume':5.0, 'WindowTitle':"", 'WindowIcon':"", 'Font':"",
			'FontGlyphs':glyphDft, 'DefaultFontSize':12, 'Lighting':0,
			'ColorKeyEnabled':False, 'ColorKey':[255,0,255], 'VideoDriver':"", 'RenderDriver':"",
			'PlaySounds':True, 'LogToFile':False,
			'LogToPrompt':False,'LogLevelFilter':0,
			'LogModules':['controller','script'],
			'FrameLimitEnabled':False, 'FrameLimit':60,
			'MouseSensitivity':0.0,
			'MouseAcceleration':False
			}

		# has the settings file been read
		self._readSettingsCompleted = {}

		# the global dictionary from which we will read after self._readSettingsCompleted is True
		self._settingsFromFile = {}

		# the logger needed to write in log file. It will be initialized in this file when self.getSettings()
		# method is called by logger
		self._logger = None

		#default settings
		self._resolutions = self._validSetting['FIFE']['ScreenResolution']
		self._renderbackends = self._validSetting['FIFE']['RenderBackend']
		self._lightingmodels = self._validSetting['FIFE']['Lighting']

		#Used to stylize the options gui
		self._gui_style = "default"

		#Initialize the serializer
		if serializer:
			self._serializer = serializer
		else:
			self._serializer = SimpleXMLSerializer()

		self.initSerializer()

		# if there's no FIFE module assume the settings file is broken
		# and replace with default settings file
		if "FIFE" not in self._serializer.getModuleNameList():
			self.setDefaults()

		# Get all modules and initialize reading of them from xml file as false
		self._allModules = self._serializer.getModuleNameList()
		# print("All Module Names:",self._allModules)
		for module in self._allModules:
			self._readSettingsCompleted[module] = False

		self._initDefaultSettingEntries()

		#self.setOneSetting('FIFE','Font','fonts/FreeSans.ttf',False)

		#print self.getSettingsFromFile('unknownhorizons')

	# set all Settings in either validSetting or defaultSetting
	def setAllSettings(self,module,settings,validSetting = True):
		if validSetting:
			self._validSetting[module] = settings
		else:
			self._defaultSetting[module] = settings

	# set an entry in the validSetting or defaultSetting dictionary
	def setOneSetting(self,module,name,value,validSetting = True):
		if validSetting:
			self._validSetting[module][name] = value
		else:
			self._defaultSetting[module][name] = value

	# get all the Settings(either validSetting or defaultSetting)
	def getAllSettings(self,module,validSetting = True):
		if validSetting:
			return self._validSetting[module]
		else:
			return self._defaultSetting[module]

	# get an entry from either validSetting or defaultSetting
	def getOneSetting(self,module,name,validSetting = True):
		if validSetting:
			return self._validSetting[module][name]
		else:
			return self._defaultSetting[module][name]

	# sets valid resolution options in the settings->Resolution
	def setValidResolutions(self, options):
		if options:
			self._resolutions = options
		self.createAndAddEntry(FIFE_MODULE, "ScreenResolution", initialdata = self._resolutions,
		              requiresrestart=True)

	def initSerializer(self):
		self._serializer.load(os.path.join(self._appdata, self._settings_file))

	def _initDefaultSettingEntries(self):
		"""Initializes the default fife setting entries. Not to be called from
		outside this class."""
		self.createAndAddEntry(FIFE_MODULE, "PlaySounds", requiresrestart=True)

		self.createAndAddEntry(FIFE_MODULE, "FullScreen", requiresrestart=True)

		self.createAndAddEntry(FIFE_MODULE, "ScreenResolution", initialdata = self._resolutions, requiresrestart=True)

		self.createAndAddEntry(FIFE_MODULE, "RenderBackend", initialdata = self._renderbackends, requiresrestart=True)

	def createAndAddEntry(self, module, name, applyfunction=None, initialdata=None, requiresrestart=False):
		""""
		@param module: The Setting module this Entry belongs to
		@type module: C{String}
		@param name: The Setting's name
		@type name: C{String}
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
		entry = SettingEntry(module, name, applyfunction, initialdata, requiresrestart)
		self.addEntry(entry)

	def addEntry(self, entry):
		"""Adds a new C{SettingEntry} to the Settting
		@param entry: A new SettingEntry that is to be added
		@type entry: C{SettingEntry}
		"""
		if entry.module not in self._entries:
			self._entries[entry.module] = {}
		self._entries[entry.module][entry.name] = entry

		"""
		# Make sure the new entry is available
		if self.get(entry.module, entry.name) is None:
			print "Updating", self._settings_file, "to the default, it is missing the entry:"\
			      , entry.name ,"for module", entry.module

			#self.setDefaults()
		if self.get(entry.module, entry.name) is None:
			print "WARNING:", entry.module, ":", entry.name, "still not found!"
		"""

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

	# get all the settings of a module name module
	def getSettingsFromFile(self, module, logger=None):
		if self._serializer:

			self._logger = logger
			modules = self._serializer.getModuleNameList()
			self._settingsFromFile[module] = self._serializer.getAllSettings(module)

			if self._logger:
				self._logger.log_log("Loading Settings From File ...")

			if self._settingsFromFile[module] is not None:
				self._readSettingsCompleted[module] = True

			# we need validation for the module FIFE only
			if module is not "FIFE":
				return self._settingsFromFile[module]
			"""
			Now we have all the settings we needed. We have to validate the settings. Applicable for module
			FIFE only
			"""
			for name in self._settingsFromFile[module]:
				# if the setting name is known, so that it is
				# both in self._settingsFromFile and validSetting
				if name in self._validSetting[module]:

					e_value = self._settingsFromFile[module][name]

					if name == "InitialVolume":
						if e_value >= self._validSetting[module][name][0] and e_value <= self._validSetting[module][name][1]:
							self._settingsFromFile[module][name] = e_value
						else:
							if self._logger:
								self._logger.log_log("InitalVolume must have a value between 0.0 and 10.0")

					elif name == "GLAlphaTestValue":
						if e_value >= self._validSetting[module][name][0] and e_value <= self._validSetting[module][name][1]:
							self._settingsFromFile[module][name] = e_value
						else:
							if self._logger:
								self._logger.log_log("GLAlphaTestValue must have a value between 0.0 and 1.0")

					elif name == "ColorKey":
						e_value = e_value.split(',')
						if int(e_value[0]) in range(0,256) and int(e_value[1]) in range(0,256) and int(e_value[2]) in range(0,256):
							self._settingsFromFile[name] = [int(e_value[0]),int(e_value[1]),int(e_value[2])];

						else:
							if self._logger:
								self._logger.log_log("ColorKey values must be within 0 and 255. Setting to Default Value.")

					elif name == "ScreenResolution":
						temp = e_value.split('x')
						if len(temp) == 2:
							self._settingsFromFile[module][name]=e_value
						else:
							if self._logger:
								self._logger.log_log("Invalid Screen Resolution value. We expect two integer separated by x")

					elif len(self._validSetting[module][name]) == 0:
						self._settingsFromFile[module][name] = e_value

					elif name == "LogModules":
						for checking_element in e_value:
							module_valid = False
							for base_element in self._validSetting[module][name]:
								# checking_element is valid

								if checking_element == base_element:
									module_valid = True
									already_in = False
									for element in self._settingsFromFile[module][name]:
										if element == checking_element:
											already_in = True
									if already_in == False:
										self._settingsFromFile[module][name].append(checking_element)
							if module_valid == False:
								if self._logger:
									self._logger.log_log(checking_element +" is not a valid logModule")
					elif name == "FrameLimit":
						if e_value > 0:
							self._settingsFromFile[module][name] = e_value
						else:
							if self._logger:
								self._logger.log_log(e_value + " is not a valid FrameLimit setting.  You must specify a positive integer!")
					elif name == "MouseSensitivity":
						self._settingsFromFile[module][name] = e_value
					elif name == "MouseAcceleration":
						self._settingsFromFile[module][name] = e_value

					elif name in ("SDLRemoveFakeAlpha", "LogToPrompt", "LogToFile"):
						if type(e_value) == int:
							try:
								e_value = (False, True)[e_value]
							except IndexError:
								self._logger.log_warn("Invalid int-value for %s. Defaulted to False!"%name)
								e_value = False
							self._logger.log_warn("Use of type int for %s is deprecated. Use bool instead!"%name)
						self._settingsFromFile[module][name] = e_value

					else:

						if isinstance(self._settingsFromFile[module][name],list) == True or isinstance(self._settingsFromFile[module][name],dict) == True:
							valid = False
							for value in self._validSetting[module][name]:
								if value == e_value:
									valid = True
									self._settingsFromFile[module][name] = e_value;
							if valid == False:
								if self._logger:
									self._logger.log_log("Setting " + name + " got invalid value. Setting to Default.")
						else: self._settingsFromFile[module][name] = e_value

				# name is unknown
				else:
					if self._logger:
						self._logger.log_log("Setting "+ name + " is unknown")

			if self._logger:
				self._logger.log_log("Settings Loaded ...")

			"""
			Upto this point we have validated all the settings that are in settings.xml file. But, what if a setting is valid and still it is
			not present in the settings.xml file. For this, we should give them the default Values that are in defaultSetting.
			"""

			for name in self._defaultSetting[module]:
				if name not in self._settingsFromFile[module]:
					self._settingsFromFile[module][name] = self._defaultSetting[module][name]

			return self._settingsFromFile[module]

		else:
			return None

	def get(self, module, name, defaultValue=None):
		""" Gets the value of a specified setting

		@param module: Name of the module to get the setting from
		@param name: Setting name
		@param defaultValue: Specifies the default value to return if the setting is not found
		@type defaultValue: C{str} or C{unicode} or C{int} or C{float} or C{bool} or C{list} or C{dict}
		"""

		if self._serializer:
			if module is "FIFE":
				# check whether getAllSettings has been called already
				if self._readSettingsCompleted[module] is not True:
					value = self._serializer.get(module, name, defaultValue)

					if value is not None:
						return value
					else:
						if name in self._defaultSetting[module]:
							return self._defaultSetting[module][name]
						else:
							raise Exception(str(name) + ' is neither in settings.xml nor it has a default value set')
				else:
					if name in self._settingsFromFile[module]:
						return self._settingsFromFile[module][name]
					else:
						raise Exception(str(name) + ' is neither in settings.xml nor it has a default value set')
			else:
				return self._serializer.get(module, name, defaultValue)
		else:
			"""
			serializer not set, reading from default value
			"""
			if name in self._defaultSetting:
				return self._defaultSetting[module][name]
			else:
				raise Exception(str(name) + ' is neither in settings.xml nor it has a default value set')

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

		#update the setting cache
		if module in self._settingsFromFile:
			self._settingsFromFile[module][name] = value
		else:
			self._settingsFromFile[module] = { name: value }

		if self._serializer:
			self._serializer.set(module, name, value, extra_attrs)

	def remove(self, module, name):
		"""
		Removes a variable

		@param module: Module where the variable should be set
		@param name: Name of the variable
		"""
		#update the setting cache
		if module in self._settingsFromFile:
			del self._settingsFromFile[module][name]

		if self._serializer:
			self._serializer.remove(module, name)

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

	def _getEntries(self):
		return self._entries

	def _setEntries(self, entries):
		self._entries = entries

	def _getSerializer(self):
		return self._serializer

	entries = property(_getEntries, _setEntries)
	serializer = property(_getSerializer)

class SettingEntry(object):

	def __init__(self, module, name, applyfunction=None, initialdata=None, requiresrestart=False):
		"""
		@param module: The Setting module this Entry belongs to
		@type module: C{String}
		@param name: The Setting's name
		@type name: C{String}
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
		self._requiresrestart = requiresrestart
		self._initialdata = initialdata
		self._applyfunction = applyfunction

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
	requiresrestart = property(_getRequiresRestart, _setRequiresRestart)
	initialdata = property(_getInitialData, _setInitialData)
	applyfunction = property(_getApplyFunction, _setApplyFunction)

	def __str__(self):
		return "SettingEntry: " +  self.name + " Module: " + self.module +  \
		        " requiresrestart: " + str(self.requiresrestart) + \
		        " initialdata: " + str(self.initialdata)
