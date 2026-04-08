# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
Settings.

Provide a framework for loading and saving game settings.

This module provides a nice framework for loading and saving game settings.
It is by no means complete but it does provide a good starting point.
"""

import os
import shutil

from fife.extensions.fife_utils import getUserDataDirectory
from fife.extensions.serializers.simplexml import SimpleXMLSerializer

FIFE_MODULE = "FIFE"


class Setting:
    """
    Manage loading and saving of game settings.

    Usage::
            from fife.extensions.fife_settings import Setting
            settings = Setting(app_name="myapp")
            screen_width = settings.get("FIFE", "ScreenWidth", 1024)
            screen_height = settings.get("FIFE", "ScreenHeight", 768)
    """

    def __init__(
        self,
        app_name="",
        settings_file="",
        default_settings_file="settings-dist.xml",
        copy_dist=True,
        serializer=None,
    ):
        r"""
        Initialize the Setting object.

        Parameters
        ----------
        app_name : str
            The application's name. If provided, the settings file will be read
            from the user's home data directory (e.g. on Windows:
            C:\\Documents and Settings\\user\\Application Data\\fife).
        settings_file : str
            The name (or path) of the settings file. If provided as a path, the
            directory of the path is used instead of the user's home directory.
        default_settings_file : str, optional
            Filename of the default settings file; copied into place when the
            settings file does not exist.
        copy_dist : bool, optional
            If True, copy the default settings file when creating settings;
            otherwise create an empty settings file.
        serializer : SimpleSerializer, optional
            Overrides the default XML serializer.

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
                shutil.copyfile(
                    self._default_settings_file,
                    os.path.join(self._appdata, self._settings_file),
                )

        # valid values possible for the engineSettings
        self._validSetting = {}
        self._validSetting["FIFE"] = {
            "FullScreen": [True, False],
            "RefreshRate": [0, 200],
            "Display": [0, 9],
            "WindowPositionX": [
                -1,
                0,
                100,
                200,
                300,
                400,
                500,
                600,
                700,
                800,
                900,
                1000,
                1100,
                1200,
                1300,
                1400,
                1500,
                1600,
                1700,
                1800,
                1920,
            ],
            "WindowPositionY": [-1, 0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000],
            "VSync": [True, False],
            "PychanDebug": [True, False],
            "ProfilingOn": [True, False],
            "SDLRemoveFakeAlpha": [True, False],
            "GLCompressImages": [False, True],
            "GLUseFramebuffer": [False, True],
            "GLUseNPOT": [False, True],
            "GLUseMipmapping": [False, True],
            "GLTextureFiltering": ["None", "Bilinear", "Trilinear", "Anisotropic"],
            "GLUseMonochrome": [False, True],
            "GLUseDepthBuffer": [False, True],
            "GLAlphaTestValue": [0.0, 1.0],
            "RenderBackend": ["OpenGL", "SDL"],
            "ScreenResolution": [
                "640x480",
                "800x600",
                "1024x600",
                "1024x768",
                "1280x768",
                "1280x800",
                "1280x960",
                "1280x1024",
                "1366x768",
                "1440x900",
                "1600x900",
                "1600x1200",
                "1680x1050",
                "1920x1080",
                "1920x1200",
            ],
            "BitsPerPixel": [0, 16, 24, 32],
            "InitialVolume": [0.0, 10.0],
            "WindowTitle": "",
            "WindowIcon": "",
            "Font": "",
            "FontGlyphs": "",
            "DefaultFontSize": "",
            "Lighting": [0, 1],
            "ColorKeyEnabled": [True, False],
            "ColorKey": ["a", "b", "c"],
            "VideoDriver": "",
            "RenderDriver": "",
            "PlaySounds": [True, False],
            "LogToFile": [True, False],
            "LogToPrompt": [True, False],
            "LogLevelFilter": [0, 1, 2, 3],
            "LogModules": [
                "all",
                "controller",
                "script",
                "video",
                "audio",
                "loaders",
                "vfs",
                "pool",
                "view",
                "model",
                "metamodel",
                "event_channel",
                "xml",
            ],
            "FrameLimitEnabled": [True, False],
            "FrameLimit": [0],
            "MouseSensitivity": [0.0],
            "MouseAcceleration": [True, False],
            "NativeImageCursor": [True, False],
            "JoystickSupport": [True, False],
        }

        glyphDft = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&amp;`'*#=[]\\\""

        # we at this point assume default values are final values for engineSettings
        self._defaultSetting = {}
        self._defaultSetting["FIFE"] = {
            "FullScreen": False,
            "RefreshRate": 60,
            "Display": 0,
            "WindowPositionX": -1,
            "WindowPositionY": -1,
            "VSync": False,
            "PychanDebug": False,
            "ProfilingOn": False,
            "SDLRemoveFakeAlpha": False,
            "GLCompressImages": False,
            "GLUseFramebuffer": True,
            "GLUseNPOT": True,
            "GLUseMipmapping": False,
            "GLTextureFiltering": "None",
            "GLUseMonochrome": False,
            "GLUseDepthBuffer": False,
            "GLAlphaTestValue": 0.3,
            "RenderBackend": "OpenGL",
            "ScreenResolution": "1024x768",
            "BitsPerPixel": 0,
            "InitialVolume": 5.0,
            "WindowTitle": "",
            "WindowIcon": "",
            "Font": "",
            "FontGlyphs": glyphDft,
            "DefaultFontSize": 12,
            "Lighting": 0,
            "ColorKeyEnabled": False,
            "ColorKey": [255, 0, 255],
            "VideoDriver": "",
            "RenderDriver": "",
            "PlaySounds": True,
            "LogToFile": False,
            "LogToPrompt": False,
            "LogLevelFilter": 0,
            "LogModules": ["controller", "script"],
            "FrameLimitEnabled": False,
            "FrameLimit": 60,
            "MouseSensitivity": 0.0,
            "MouseAcceleration": False,
            "NativeImageCursor": False,
            "JoystickSupport": False,
        }

        # has the settings file been read
        self._readSettingsCompleted = {}

        # the global dictionary from which we will read after self._readSettingsCompleted is True
        self._settingsFromFile = {}

        # the logger needed to write in log file. It will be initialized in this file when self.getSettings()
        # method is called by logger
        self._logger = None

        # default settings
        self._resolutions = self._validSetting["FIFE"]["ScreenResolution"]
        self._renderbackends = self._validSetting["FIFE"]["RenderBackend"]
        min_display, max_display = self._validSetting["FIFE"]["Display"]
        self._displays = list(range(min_display, max_display + 1))
        self._lightingmodels = self._validSetting["FIFE"]["Lighting"]

        # Used to stylize the options gui
        self._gui_style = "default"

        # Initialize the serializer
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

        # self.setOneSetting('FIFE','Font','fonts/FreeSans.ttf',False)

        # print self.getSettingsFromFile('unknownhorizons')

    def setAllSettings(self, module, settings, validSetting=True):
        """Set all settings for a module."""
        if validSetting:
            self._validSetting[module] = settings
        else:
            self._defaultSetting[module] = settings

    def setOneSetting(self, module, name, value, validSetting=True):
        """Set a single setting entry."""
        if validSetting:
            self._validSetting[module][name] = value
        else:
            self._defaultSetting[module][name] = value

    def getAllSettings(self, module, validSetting=True):
        """Get all settings for a module."""
        if validSetting:
            return self._validSetting[module]
        else:
            return self._defaultSetting[module]

    def getOneSetting(self, module, name, validSetting=True):
        """Get a single setting entry."""
        if validSetting:
            return self._validSetting[module][name]
        else:
            return self._defaultSetting[module][name]

    def setValidResolutions(self, options):
        """Set valid resolution options."""
        if options:
            self._resolutions = options
        self.createAndAddEntry(
            FIFE_MODULE,
            "ScreenResolution",
            initialdata=self._resolutions,
            requiresrestart=True,
        )

    def setValidDisplays(self, options):
        """Set valid display options."""
        if options:
            normalized = sorted({int(display) for display in options})
            if len(normalized) <= 1:
                normalized = [0, 1]
            self._displays = normalized
        elif len(self._displays) <= 1:
            self._displays = [0, 1]
        self.createAndAddEntry(
            FIFE_MODULE,
            "Display",
            initialdata=self._displays,
            requiresrestart=True,
        )

    def initSerializer(self):
        """Initialize the serializer."""
        self._serializer.load(os.path.join(self._appdata, self._settings_file))

    def _initDefaultSettingEntries(self):
        """Initialize default setting entries.

        Not to be called from outside this class.
        """
        self.createAndAddEntry(FIFE_MODULE, "PlaySounds", requiresrestart=True)

        self.createAndAddEntry(FIFE_MODULE, "FullScreen", requiresrestart=True)

        self.createAndAddEntry(
            FIFE_MODULE,
            "ScreenResolution",
            initialdata=self._resolutions,
            requiresrestart=True,
        )

        self.createAndAddEntry(
            FIFE_MODULE,
            "RenderBackend",
            initialdata=self._renderbackends,
            requiresrestart=True,
        )

        self.createAndAddEntry(
            FIFE_MODULE,
            "Display",
            initialdata=self._displays,
            requiresrestart=True,
        )

    def createAndAddEntry(
        self, module, name, applyfunction=None, initialdata=None, requiresrestart=False
    ):
        """Create and add a new setting entry.

        Parameters
        ----------
        module : str
            The Setting module this entry belongs to.
        name : str
            The setting's name.
        applyfunction : callable, optional
            Function that makes the changes when the setting is saved.
        initialdata : str or bool, optional
            Initial data for widgets that support it.
        requiresrestart : bool, optional
            Whether changing this setting requires a restart.
        """
        entry = SettingEntry(module, name, applyfunction, initialdata, requiresrestart)
        self.addEntry(entry)

    def addEntry(self, entry):
        """Add a new :class:`SettingEntry` to the settings.

        Parameters
        ----------
        entry : SettingEntry
            The entry to add.
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
        """Write the settings to the settings file.

        Parameters
        ----------
        filename : str, optional
            File to save the settings to. If not specified, the original
            settings file is used.
        """
        if self._serializer:
            if filename == "":
                self._serializer.save(os.path.join(self._appdata, self._settings_file))
            else:
                self._serializer.save(filename)

    def getSettingsFromFile(self, module, logger=None):
        """Get all settings from the file for a module."""
        if self._serializer:
            self._logger = logger
            modules = self._serializer.getModuleNameList()
            if module not in modules:
                if self._logger:
                    self._logger.log_log(
                        f"Settings module '{module}' was not found in serializer input."
                    )
                self._settingsFromFile[module] = None
                return self._settingsFromFile[module]
            self._settingsFromFile[module] = self._serializer.getAllSettings(module)

            if self._logger:
                self._logger.log_log("Loading Settings From File ...")

            if self._settingsFromFile[module] is not None:
                self._readSettingsCompleted[module] = True

            # we need validation for the module FIFE only
            if module != "FIFE":
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
                        if (
                            e_value >= self._validSetting[module][name][0]
                            and e_value <= self._validSetting[module][name][1]
                        ):
                            self._settingsFromFile[module][name] = e_value
                        else:
                            if self._logger:
                                self._logger.log_log(
                                    "InitalVolume must have a value between 0.0 and 10.0"
                                )

                    elif name == "GLAlphaTestValue":
                        if (
                            e_value >= self._validSetting[module][name][0]
                            and e_value <= self._validSetting[module][name][1]
                        ):
                            self._settingsFromFile[module][name] = e_value
                        else:
                            if self._logger:
                                self._logger.log_log(
                                    "GLAlphaTestValue must have a value between 0.0 and 1.0"
                                )

                    elif name == "ColorKey":
                        e_value = e_value.split(",")
                        if (
                            int(e_value[0]) in range(0, 256)
                            and int(e_value[1]) in range(0, 256)
                            and int(e_value[2]) in range(0, 256)
                        ):
                            self._settingsFromFile[module][name] = [
                                int(e_value[0]),
                                int(e_value[1]),
                                int(e_value[2]),
                            ]

                        else:
                            if self._logger:
                                self._logger.log_log(
                                    "ColorKey values must be within 0 and 255. Setting to Default Value."
                                )

                    elif name == "ScreenResolution":
                        temp = e_value.split("x")
                        if len(temp) == 2:
                            self._settingsFromFile[module][name] = e_value
                        else:
                            if self._logger:
                                self._logger.log_log(
                                    "Invalid Screen Resolution value. We expect two integer separated by x"
                                )

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
                                    if not already_in:
                                        self._settingsFromFile[module][name].append(
                                            checking_element
                                        )
                            if not module_valid:
                                if self._logger:
                                    self._logger.log_log(
                                        checking_element + " is not a valid logModule"
                                    )
                    elif name == "FrameLimit":
                        if e_value > 0:
                            self._settingsFromFile[module][name] = e_value
                        else:
                            if self._logger:
                                self._logger.log_log(
                                    e_value
                                    + " is not a valid FrameLimit setting.  You must specify a positive integer!"
                                )
                    elif name == "MouseSensitivity":
                        self._settingsFromFile[module][name] = e_value
                    elif name == "MouseAcceleration":
                        self._settingsFromFile[module][name] = e_value
                    elif name == "NativeImageCursor":
                        self._settingsFromFile[module][name] = e_value
                    elif name == "JoystickSupport":
                        self._settingsFromFile[module][name] = e_value

                    elif name == "Display":
                        min_display, max_display = self._validSetting[module][name]
                        if type(e_value) is int and min_display <= e_value <= max_display:
                            self._settingsFromFile[module][name] = e_value
                        else:
                            if self._logger:
                                self._logger.log_log(
                                    "Invalid Display setting. Setting to Default."
                                )

                    elif name in ("SDLRemoveFakeAlpha", "LogToPrompt", "LogToFile"):
                        if type(e_value) is int:
                            try:
                                e_value = (False, True)[e_value]
                            except IndexError:
                                self._logger.log_warn(
                                    f"Invalid int-value for {name}. Defaulted to False!"
                                )
                                e_value = False
                            self._logger.log_warn(
                                f"Use of type int for {name} is deprecated. Use bool instead!"
                            )
                        self._settingsFromFile[module][name] = e_value

                    else:
                        if isinstance(
                            self._settingsFromFile[module][name], list
                        ) or isinstance(self._settingsFromFile[module][name], dict):
                            valid = False
                            for value in self._validSetting[module][name]:
                                if value == e_value:
                                    valid = True
                                    self._settingsFromFile[module][name] = e_value
                            if not valid:
                                if self._logger:
                                    self._logger.log_log(
                                        "Setting "
                                        + name
                                        + " got invalid value. Setting to Default."
                                    )
                        else:
                            self._settingsFromFile[module][name] = e_value

                # name is unknown
                else:
                    if self._logger:
                        self._logger.log_log("Setting " + name + " is unknown")

            if self._logger:
                self._logger.log_log("Settings Loaded ...")

            """
			Upto this point we have validated all the settings that are in settings.xml file. But, what if a setting is valid and still it is
			not present in the settings.xml file. For this, we should give them the default Values that are in defaultSetting.
			"""

            for name in self._defaultSetting[module]:
                if name not in self._settingsFromFile[module]:
                    self._settingsFromFile[module][name] = self._defaultSetting[module][
                        name
                    ]

            return self._settingsFromFile[module]

        else:
            return None

    def get(self, module, name, defaultValue=None):
        """Get the value of a specified setting.

        Parameters
        ----------
        module : str
            Name of the module to get the setting from.
        name : str
            Setting name.
        defaultValue : Any, optional
            Default value to return if the setting is not found.

        Returns
        -------
        Any
            The value of the setting or ``defaultValue`` if not present.
        """
        if self._serializer:
            if module == "FIFE":
                # check whether getAllSettings has been called already
                if self._readSettingsCompleted[module] is not True:
                    value = self._serializer.get(module, name, defaultValue)

                    if value is not None:
                        return value
                    else:
                        if name in self._defaultSetting[module]:
                            return self._defaultSetting[module][name]
                        else:
                            raise Exception(
                                str(name)
                                + " is neither in settings.xml nor it has a default value set"
                            )
                else:
                    if name in self._settingsFromFile[module]:
                        return self._settingsFromFile[module][name]
                    else:
                        raise Exception(
                            str(name)
                            + " is neither in settings.xml nor it has a default value set"
                        )
            else:
                return self._serializer.get(module, name, defaultValue)
        else:
            """
            serializer not set, reading from default value
            """
            if name in self._defaultSetting:
                return self._defaultSetting[module][name]
            else:
                raise Exception(
                    str(name)
                    + " is neither in settings.xml nor it has a default value set"
                )

    def set(self, module, name, value, extra_attrs={}):
        """
        Set a setting to a specified value.

        Parameters
        ----------
        module : str
            Module where the setting should be set.
        name : str
            Name of the setting.
        value : Any
            Value to assign to the setting.
        extra_attrs : dict, optional
            Extra attributes to be stored in the XML file.
        """
        # update the setting cache
        if module in self._settingsFromFile:
            self._settingsFromFile[module][name] = value
        else:
            self._settingsFromFile[module] = {name: value}

        if self._serializer:
            self._serializer.set(module, name, value, extra_attrs)

    def remove(self, module, name):
        """
        Remove a variable.

        Parameters
        ----------
        module : str
            Module where the variable should be set.
        name : str
            Name of the variable.
        """
        # update the setting cache
        if module in self._settingsFromFile:
            del self._settingsFromFile[module][name]

        if self._serializer:
            self._serializer.remove(module, name)

    def setAvailableScreenResolutions(self, reslist):
        """
        Set available screen resolutions.

        This should be called once right after you instantiate Settings.

        Valid screen resolutions must be strings in the form of: WIDTHxHEIGHT

        Example::

            settings.setAvailableScreenResolutions(["800x600", "1024x768"])
        """
        self._resolutions = reslist

    def setDefaults(self):
        """Overwrite the setting file with the default settings file."""
        shutil.copyfile(
            self._default_settings_file, os.path.join(self._appdata, self._settings_file)
        )
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


class SettingEntry:
    """Represent a single setting entry."""

    def __init__(
        self, module, name, applyfunction=None, initialdata=None, requiresrestart=False
    ):
        """Initialize a SettingEntry.

        Parameters
        ----------
        module : str
            The Setting module this entry belongs to.
        name : str
            The setting's name.
        applyfunction : callable, optional
            Function that makes the changes when the setting is saved.
        initialdata : str or bool, optional
            Initial data for widgets that support it.
        requiresrestart : bool, optional
            Whether changing this setting requires a restart.
        """
        self._module = module
        self._name = name
        self._requiresrestart = requiresrestart
        self._initialdata = initialdata
        self._applyfunction = applyfunction

    def onApply(self, data):
        """Apply the setting change.

        Implement actions that need to be taken when the setting is changed here.
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
        """Return string representation of the setting entry."""
        return (
            "SettingEntry: "
            + self.name
            + " Module: "
            + self.module
            + " requiresrestart: "
            + str(self.requiresrestart)
            + " initialdata: "
            + str(self.initialdata)
        )
