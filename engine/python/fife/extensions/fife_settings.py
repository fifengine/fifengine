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

@note:  Please note that you MUST provide a default settings-dist.xml file
in the root directory of your project for this module to function correctly.
"""

import shutil
import os
from StringIO import StringIO

from fife.extensions import pychan
from fife.extensions.fife_utils import getUserDataDirectory
try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET


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

EMPTY_SETTINGS="""\
<?xml version='1.0' encoding='UTF-8'?>
<Settings>
	<Module name="FIFE">

	</Module>

</Settings>
"""

class Setting(object):
	"""
	This class manages loading and saving of game settings.

	Usage::
		from fife.extensions.fife_settings import Setting
		settings = Setting(app_name="myapp")
		screen_width = settings.get("FIFE", "ScreenWidth", 1024)
		screen_height = settings.get("FIFE", "ScreenHeight", 768)
	"""

	def __init__(self, app_name="", settings_file="", settings_gui_xml="", changes_gui_xml=""):
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
		@param settings_gui_xml: If you specify this parameter you can customize the look
		of the settings dialog box.
		@note: As of now you MUST have all the elements of the default settings dialog box.
		At some point we may make it customizable.

		"""
		self._app_name = app_name
		self._settings_file = settings_file
		self._settings_gui_xml = settings_gui_xml
		self._changes_gui_xml = changes_gui_xml

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
			if os.path.exists('settings-dist.xml'):
				shutil.copyfile('settings-dist.xml', os.path.join(self._appdata, self._settings_file))
			else:
				#no settings file found
				tree = ET.parse(StringIO(EMPTY_SETTINGS))
				tree.write(os.path.join(self._appdata, self._settings_file), 'UTF-8')

		#default settings
		self._resolutions = ['640x480', '800x600', '1024x768', '1280x800', '1440x900']

		self.loadSettings()

	def loadSettings(self):
		self._tree = ET.parse(os.path.join(self._appdata, self._settings_file))
		self._root_element = self._tree.getroot()
		self.validateTree()

	def validateTree(self):
		""" Iterates the settings tree and prints warning when an invalid tag is found """
		for c in self._root_element.getchildren():
			if c.tag != "Module":
				print "Invalid tag in settings.xml. Expected Module, got: ", c.tag
			elif c.get("name", "") == "":
				print "Invalid tag in settings.xml. Module name is empty."
			else:
				for e in c.getchildren():
					if e.tag != "Setting":
						print "Invalid tag in settings.xml in module: ",c.tag,
						print ". Expected Setting, got: ", e.tag
					elif c.get("name", "") == "":
						print "Invalid tag in settings.xml in module: ",c.tag,
						print ". Setting name is empty", e.tag

	def getModuleTree(self, module):
		"""
		Returns a module element from the settings tree. If no module with the specified
		name exists, a new element will be created.

		@param module: The module to get from the settings tree
		@type module: C{string}
		"""
		if not isinstance(module, str) and not isinstance(module, unicode):
			raise AttributeError("Settings:getModuleTree: Invalid type for module argument.")

		for c in self._root_element.getchildren():
			if c.tag == "Module" and c.get("name", "") == module:
				return c

		# Create module
		return ET.SubElement(self._root_element, "Module", {"name":module})

	def get(self, module, name, defaultValue=None):
		""" Gets the value of a specified setting

		@param module: Name of the module to get the setting from
		@param name: Setting name
		@param defaultValue: Specifies the default value to return if the setting is not found
		@type defaultValue: C{str} or C{unicode} or C{int} or C{float} or C{bool} or C{list} or C{dict}
		"""
		if not isinstance(name, str) and not isinstance(name, unicode):
			raise AttributeError("Settings:get: Invalid type for name argument.")

		moduleTree = self.getModuleTree(module)
		element = None
		for e in moduleTree.getchildren():
			if e.tag == "Setting" and e.get("name", "") == name:
				element = e
				break
		else:
			return defaultValue

		e_value = element.text
		e_strip = element.get("strip", "1").strip().lower()
		e_type	= str(element.get("type", "str")).strip()

		if e_value is None:
			return defaultValue

		# Strip value
		if e_strip == "" or e_strip == "false" or e_strip == "no" or e_strip == "0":
			e_strip = False
		else: e_strip = True

		if e_type == "str" or e_type == "unicode":
			if e_strip: e_value = e_value.strip()
		else:
			e_value = e_value.strip()

		# Return value
		if e_type == 'int':
			return int(e_value)
		elif e_type == 'float':
			return float(e_value)
		elif e_type == 'bool':
			e_value = e_value.lower()
			if e_value == "" or e_value == "false" or e_value == "no" or e_value == "0":
				return False
			else:
				return True
		elif e_type == 'str':
			return str(e_value)
		elif e_type == 'unicode':
			return unicode(e_value)
		elif e_type == 'list':
			return self._deserializeList(e_value)
		elif e_type == 'dict':
			return self._deserializeDict(e_value)

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
		if not isinstance(name, str) and not isinstance(name, unicode):
			raise AttributeError("Settings:set: Invalid type for name argument.")

		moduleTree = self.getModuleTree(module)
		e_type = "str"

		if isinstance(value, bool): # This must be before int
			e_type = "bool"
			value = str(value)
		elif isinstance(value, int):
			e_type = "int"
			value = str(value)
		elif isinstance(value, float):
			e_type = "float"
			value = str(value)
		elif isinstance(value, unicode):
			e_type = "unicode"
			value = unicode(value)
		elif isinstance(value, list):
			e_type = "list"
			value = self._serializeList(value)
		elif isinstance(value, dict):
			e_type = "dict"
			value = self._serializeDict(value)
		else:
			e_type = "str"
			value = str(value)

		for e in moduleTree.getchildren():
			if e.tag != "Setting": continue
			if e.get("name", "") == name:
				e.text = value
				break
		else:
			attrs = {"name":name, "type":e_type}
			for k in extra_attrs:
				if k not in attrs:
					attrs[k] = extra_args[k]
			elm = ET.SubElement(moduleTree, "Setting", attrs)
			elm.text = value

	def saveSettings(self):
		""" Writes the settings to the settings file """
		self._indent(self._root_element)
		self._tree.write(os.path.join(self._appdata, self._settings_file), 'UTF-8')

	def _indent(self, elem, level=0):
		"""
		Adds whitespace, so the resulting XML-file is properly indented.
		Shamelessly stolen from http://effbot.org/zone/element-lib.htm
		"""
		i = "\n" + level*"  "
		if len(elem):
			if not elem.text or not elem.text.strip():
				elem.text = i + "  "
			if not elem.tail or not elem.tail.strip():
				elem.tail = i
			for elem in elem:
				self._indent(elem, level+1)
			if not elem.tail or not elem.tail.strip():
				elem.tail = i
		else:
			if level and (not elem.tail or not elem.tail.strip()):
				elem.tail = i

	# FIXME:
	# These serialization functions are not reliable at all
	# This will only serialize the first level of a dict or list
	# It will not check the types nor the content for conflicts.
	# Perhaps we should add a small serialization library?
	def _serializeList(self, list):
		""" Serializes a list, so it can be stored in a text file """
		return " ; ".join(list)

	def _deserializeList(self, string):
		""" Deserializes a list back into a list object """
		return string.split(" ; ")

	def _serializeDict(self, dict):
		""" Serializes a list, so it can be stored in a text file """
		serial = ""
		for key in dict:
			value = dict[key]
			if serial != "": serial += " ; "
			serial += str(key)+" : "+str(value)

		return serial

	def _deserializeDict(self, serial):
		""" Deserializes a list back into a dict object """
		dict = {}
		items = serial.split(" ; ")
		for i in items:
			kv_pair = i.split(" : ")
			dict[kv_pair[0]] = kv_pair[1]
		return dict

	def onOptionsPress(self):
		"""
		Opens the options dialog box.  Usually you would bind this to a button.
		"""
		self.changesRequireRestart = False
		self.isSetToDefault = False

		if os.path.isfile(self._settings_gui_xml):
			self.OptionsDlg = pychan.loadXML(self._settings_gui_xml)
		else:
			self.OptionsDlg = pychan.loadXML(StringIO(self._settings_gui_xml))
		self.OptionsDlg.distributeInitialData({
			'screen_resolution' : self._resolutions,
			'render_backend' : ['OpenGL', 'SDL']
		})
		self.OptionsDlg.distributeData({
			'screen_resolution' : self._resolutions.index(str(self.get("FIFE", "ScreenWidth")) + 'x' + str(self.get("FIFE", "ScreenHeight"))),
			'render_backend' : 0 if self.get("FIFE", "RenderBackend") == "OpenGL" else 1,
			'enable_fullscreen' : self.get("FIFE", "FullScreen"),
			'enable_sound' : self.get("FIFE", "PlaySounds")
		})
		self.OptionsDlg.mapEvents({
			'okButton' : self.applySettings,
			'cancelButton' : self.OptionsDlg.hide,
			'defaultButton' : self.setDefaults
		})
		self.OptionsDlg.show()

	def applySettings(self):
		"""
		Writes the settings file.  If a change requires a restart of the engine
		it notifies you with a small dialog box.
		"""
		screen_resolution, render_backend, enable_fullscreen, enable_sound = self.OptionsDlg.collectData('screen_resolution', 'render_backend', 'enable_fullscreen', 'enable_sound')
		render_backend = 'OpenGL' if render_backend is 0 else 'SDL'
		if render_backend != self.get("FIFE", "RenderBackend"):
			self.set("FIFE", 'RenderBackend', render_backend)
			self.changesRequireRestart = True
		if int(enable_fullscreen) != int(self.get("FIFE", "FullScreen")):
			self.set("FIFE", 'FullScreen', int(enable_fullscreen))
			self.changesRequireRestart = True
		if int(enable_sound) != int(self.get("FIFE", "PlaySounds")):
			self.set("FIFE", 'PlaySounds', int(enable_sound))
			self.changesRequireRestart = True
		if screen_resolution != self._resolutions.index(str(self.get("FIFE", "ScreenWidth")) + 'x' + str(self.get("FIFE", "ScreenHeight"))):
			self.set("FIFE", 'ScreenWidth', int(self._resolutions[screen_resolution].partition('x')[0]))
			self.set("FIFE", 'ScreenHeight', int(self._resolutions[screen_resolution].partition('x')[2]))
			self.changesRequireRestart = True

		self.saveSettings()

		self.OptionsDlg.hide()
		if self.changesRequireRestart:
			RestartDlg = pychan.loadXML(StringIO(self._changes_gui_xml))
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
		Overwrites the setting file with the default settings-dist.xml file.
		"""
		shutil.copyfile('settings-dist.xml', os.path.join(self._appdata, self._settings_file))
		self.changesRequireRestart = True
		self.loadSettings()
		self.applySettings()

		if self.OptionsDlg:
			self.OptionsDlg.hide()
