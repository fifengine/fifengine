#!/usr/bin/env python

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
# This is the rio de hola client for FIFE.

import sys, os, re, math, random, shutil

fife_path = os.path.join('..','..','engine','python')
if os.path.isdir(fife_path) and fife_path not in sys.path:
	sys.path.insert(0,fife_path)

from fife import fife
print "Using the FIFE python module found here: ", os.path.dirname(fife.__file__)

from fife.extensions import *
from scripts import world
from scripts.common import eventlistenerbase
from fife.extensions.basicapplication import ApplicationBase
from fife.extensions import pychan
from fife.extensions.pychan import widgets
from fife.extensions.fife_settings import Setting

TDS = Setting(app_name="shooter", 
              settings_file="./settings.xml", 
              settings_gui_xml="")

class ApplicationListener(eventlistenerbase.EventListenerBase):
	def __init__(self, engine, world):
		super(ApplicationListener, self).__init__(engine,regKeys=True,regCmd=True, regMouse=False, regConsole=True, regWidget=True)
		self._engine = engine
		self._world = world
		self._quit = False
		
	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		consumed = False
		if keyval == fife.Key.ESCAPE:
			self._world.showMainMenu()
			evt.consume()

	def onCommand(self, command):
		self._quit = (command.getCommandType() == fife.CMD_QUIT_GAME)
		if self._quit:
			command.consume()

class Shooter(ApplicationBase):
	def __init__(self):
		super(Shooter,self).__init__()
		pychan.init(self.engine, debug=False)
		
		#This is requred if you want to use modal dialog boxes
		pychan.setupModalExecution(self.mainLoop,self.breakFromMainLoop)

		self._setting = TDS
		
		self._world = world.World(self, self.engine, self._setting)
		self._listener = ApplicationListener(self.engine, self._world)

	def requestQuit(self):
		cmd = fife.Command()
		cmd.setSource(None)
		cmd.setCommandType(fife.CMD_QUIT_GAME)
		self.engine.getEventManager().dispatchCommand(cmd)
		
	def loadSettings(self):
		"""
		Load the settings from a python file and load them into the engine.
		Called in the ApplicationBase constructor.  I hard coded all the
		settings in here to remove the complexity of loading settings from
		an XML file which would be out of scope of this demo.
		"""
		
		self._setting = TDS

		glyphDft = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&amp;`'*#=[]\\\""
		engineSetting = self.engine.getSettings()
		engineSetting.setDefaultFontGlyphs(self._setting.get("FIFE", "FontGlyphs", glyphDft))
		engineSetting.setDefaultFontPath(self._setting.get("FIFE", "Font", "fonts/FreeSans.ttf"))
		engineSetting.setDefaultFontSize(self._setting.get("FIFE", "DefaultFontSize", 12))
		engineSetting.setBitsPerPixel(self._setting.get("FIFE", "BitsPerPixel", 0))
		engineSetting.setInitialVolume(self._setting.get("FIFE", "InitialVolume", 5.0))
		engineSetting.setSDLRemoveFakeAlpha(self._setting.get("FIFE", "SDLRemoveFakeAlpha", 1))
		engineSetting.setScreenWidth(self._setting.get("FIFE", "ScreenWidth", 1024))
		engineSetting.setScreenHeight(self._setting.get("FIFE", "ScreenHeight", 768))
		engineSetting.setRenderBackend(self._setting.get("FIFE", "RenderBackend", "OpenGL"))
		engineSetting.setFullScreen(self._setting.get("FIFE", "FullScreen", 0))

		engineSetting.setWindowTitle(self._setting.get("FIFE", "WindowTitle", "No window title set"))
		engineSetting.setWindowIcon(self._setting.get("FIFE", "WindowIcon", ""))
		engineSetting.setImageChunkingSize(self._setting.get("FIFE", "ImageChunkSize", 256))

	def initLogging(self):
		"""
		Initialize the LogManager.
		"""
		
		engineSetting = self.engine.getSettings()
		logmodules = self._setting.get("FIFE", "LogModules", "controller")

		#log to both the console and log file
		self._log = fifelog.LogManager(self.engine, 
									   self._setting.get("FIFE", "LogToPrompt", "0"), 
									   self._setting.get("FIFE", "LogToFile", "0"))

		self._log.setLevelFilter(fife.LogManager.LEVEL_DEBUG)
		
		if logmodules:
			self._log.setVisibleModules(*logmodules)
			
	def createListener(self):
		pass # already created in constructor

	def _pump(self):
		if self._listener._quit:
			self.breakRequested = True
		else:
			self._world.pump()

def main():
	app = Shooter()
	app.run()


if __name__ == '__main__':
	main()
