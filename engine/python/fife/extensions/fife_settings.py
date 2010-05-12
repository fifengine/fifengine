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

class Setting(object):
	"""
	This class manages loading and saving of game settings.
	
	Usage::
		from fife.extensions.fife_settings import Setting
		settings = Setting(app_name="myapp")
		screen_width = settings.readSetting("ScreenWidth")
	"""
	def __init__(self, app_name="", settings_file="", settings_gui_xml=""):
		"""
		Initializes the Setting object.
		
		@param app_name: The applications name.  If this parameter is provided 
		alone it will try to read the settings file from the users home directory. 
		In windows this will be	something like:	C:\Documents and Settings\user\Application Data\fife
		@type app_name: C{string}
		@param settings_file: The name of the settings file.  If this parameter is
		provided it will look for the setting file as you specify it, first looking 
		in the working directory.
		@type settings_file: C{string}
		@param settings_gui_xml: If you specify this parameter you can customize the look
		of the settings dialog box.
		@note: As of now you MUST have all the elements of the default settings dialog box.
		At some point we may make it customizable.
		
		"""
		self._app_name = app_name
		self._settings_file = settings_file
		self._settings_gui_xml = settings_gui_xml
	
		if self._settings_file == "":
			self._settings_file = "settings.xml"
			self._appdata = getUserDataDirectory("fife", self._app_name)
		else:
			self._appdata = os.path.dirname(self._settings_file)
			self._settings_file = os.path.basename(self._settings_file)

		
		if self._settings_gui_xml == "":
			self.settings_gui_xml = SETTINGS_GUI_XML
		
		
		if not os.path.exists(os.path.join(self._appdata, self._settings_file)):
			shutil.copyfile('settings-dist.xml', os.path.join(self._appdata, self._settings_file))
		

	def onOptionsPress(self):
		"""
		Opends the options dialog box.  Usually you would bind this to a button.
		"""
		self.changesRequireRestart = False
		self.isSetToDefault = False
		self.Resolutions = ['640x480', '800x600', '1024x768', '1280x800', '1440x900']
		if not hasattr(self, 'OptionsDlg'):
			self.OptionsDlg = None
		if not self.OptionsDlg:
			self.OptionsDlg = pychan.loadXML(StringIO(SETTINGS_GUI_XML))
			self.OptionsDlg.distributeInitialData({
				'screen_resolution' : self.Resolutions,
				'render_backend' : ['OpenGL', 'SDL']
			})
			self.OptionsDlg.distributeData({
				'screen_resolution' : self.Resolutions.index(str(self.readSetting("ScreenWidth")) + 'x' + str(self.readSetting("ScreenHeight"))),
				'render_backend' : 0 if str(self.readSetting("RenderBackend")) == "OpenGL" else 1,
				'enable_fullscreen' : int(self.readSetting("FullScreen")),
				'enable_sound' : int(self.readSetting("PlaySounds"))
			})
			self.OptionsDlg.mapEvents({
				'okButton' : self.saveSettings,
				'cancelButton' : self.OptionsDlg.hide,
				'defaultButton' : self.setDefaults
			})
		self.OptionsDlg.show()

	def setDefaults(self):
		"""
		Overwrites the setting file with the default settings-dist.xml file.
		"""
		shutil.copyfile('settings-dist.xml', os.path.join(self._appdata, self._settings_file))
		self.isSetToDefault = True
		self.changesRequireRestart = True

	def readSetting(self, name, type='int', strip=True, text=False):
		if not hasattr(self, 'tree'):
			self.tree = ET.parse(os.path.join(self._appdata, self._settings_file))
			self.root_element = self.tree.getroot()
		element = self.root_element.find(name)
		if element is not None:
			element_value = element.text
			if element_value is None:
				if type == 'int':
					return 0
				elif type == 'list':
					list = []
					return list
			else:
				if type == 'int':
					return element_value.strip() if strip else element_value
				elif type == 'list':
					list = []
					list_s = []
					list = str(element_value.strip()).split(";")
					for item in list:
						item = item.strip()
						if text:
							item = item.replace('\\n', '\n')
						list_s.append(item)
					return list_s
				elif type == 'bool':
					return False if element_value.strip() == 'False' else True
		else:
			print 'Setting,', name, 'does not exist!'

	def setSetting(self, name, value):
		element = self.root_element.find(name)
		if element is not None:
			if value is not element.text:
				element.text = str(value)
		else:
			print 'Setting,', name, 'does not exist!'

	def saveSettings(self):
		"""
		Writes the settings file.  If a change requires a restart of the engine
		it notifies you with a small dialog box.
		"""
		screen_resolution, render_backend, enable_fullscreen, enable_sound = self.OptionsDlg.collectData('screen_resolution', 'render_backend', 'enable_fullscreen', 'enable_sound')
		render_backend = 'OpenGL' if render_backend is 0 else 'SDL'
		if render_backend != str(self.readSetting("RenderBackend")):
			self.setSetting('RenderBackend', render_backend)
			self.changesRequireRestart = True
		if int(enable_fullscreen) != int(self.readSetting("FullScreen")):
			self.setSetting('FullScreen', int(enable_fullscreen))
			self.changesRequireRestart = True
		if int(enable_sound) != int(self.readSetting("PlaySounds")):
			self.setSetting('PlaySounds', int(enable_sound))
			self.changesRequireRestart = True
		if screen_resolution != self.Resolutions.index(str(self.readSetting("ScreenWidth")) + 'x' + str(self.readSetting("ScreenHeight"))):
			self.setSetting('ScreenWidth', int(self.Resolutions[screen_resolution].partition('x')[0]))
			self.setSetting('ScreenHeight', int(self.Resolutions[screen_resolution].partition('x')[2]))
			self.changesRequireRestart = True

		if not self.isSetToDefault:
			self.tree.write(os.path.join(self._appdata, self._settings_file))
		self.OptionsDlg.hide()
		if self.changesRequireRestart:
			RestartDlg = pychan.loadXML(StringIO(CHANGES_REQUIRE_RESTART))
			RestartDlg.mapEvents({ 'closeButton' : RestartDlg.hide })
			RestartDlg.show()
