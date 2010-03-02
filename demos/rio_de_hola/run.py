#!/usr/bin/env python

# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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
from settings import Setting
from fife.extensions.fife_utils import getUserDataDirectory

TDS = Setting()

class ApplicationListener(eventlistenerbase.EventListenerBase):
	def __init__(self, engine, world):
		super(ApplicationListener, self).__init__(engine,regKeys=True,regCmd=True, regMouse=False, regConsole=True, regWidget=True)
		self.engine = engine
		self.world = world
		engine.getEventManager().setNonConsumableKeys([
			fife.Key.ESCAPE,])

		self.quit = False
		self.aboutWindow = None

		self.rootpanel = pychan.loadXML('gui/rootpanel.xml')
		self.rootpanel.mapEvents({ 
			'quitButton' : self.onQuitButtonPress,
			'aboutButton' : self.onAboutButtonPress,
			'optionsButton' : TDS.onOptionsPress
		})
		self.rootpanel.show()

	def keyPressed(self, evt):
		print evt
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		consumed = False
		if keyval == fife.Key.ESCAPE:
			self.quit = True
			evt.consume()
		elif keyval == fife.Key.F10:
			self.engine.getGuiManager().getConsole().toggleShowHide()
			evt.consume()
		elif keystr == 'p':
			self.engine.getRenderBackend().captureScreen('screenshot.png')
			evt.consume()

	def onCommand(self, command):
		self.quit = (command.getCommandType() == fife.CMD_QUIT_GAME)
		if self.quit:
			command.consume()

	def onConsoleCommand(self, command):
		result = ''
		if command.lower() in ('quit', 'exit'):
			self.quit = True
			result = 'quitting'
		elif command.lower() in ( 'help', 'help()' ):
			self.engine.getGuiManager().getConsole().println( open( 'misc/infotext.txt', 'r' ).read() )
			result = "-- End of help --"
		else:
			result = self.world.onConsoleCommand(command)
		if not result:
			try:
				result = str(eval(command))
			except:
				pass
		if not result:
			result = 'no result'
		return result

	def onQuitButtonPress(self):
		cmd = fife.Command()
		cmd.setSource(None)
		cmd.setCommandType(fife.CMD_QUIT_GAME)
		self.engine.getEventManager().dispatchCommand(cmd)

	def onAboutButtonPress(self):
		if not self.aboutWindow:
			self.aboutWindow = pychan.loadXML('gui/help.xml')
			self.aboutWindow.mapEvents({ 'closeButton' : self.aboutWindow.hide })
			self.aboutWindow.distributeData({ 'helpText' : open("misc/infotext.txt").read() })
		self.aboutWindow.show()

class IslandDemo(ApplicationBase):
	def __init__(self):
		super(IslandDemo,self).__init__()
		pychan.init(self.engine, debug=TDS.readSetting("PychanDebug", type='bool'))
		self.world = world.World(self.engine)
		self.listener = ApplicationListener(self.engine, self.world)
		self.world.load(str(TDS.readSetting("MapFile")))

		self.soundmanager = self.engine.getSoundManager()
		self.soundmanager.init()

		if int(TDS.readSetting("PlaySounds")):
			# play track as background music
			emitter = self.soundmanager.createEmitter()
			id = self.engine.getSoundClipPool().addResourceFromFile('music/rio_de_hola.ogg')
			emitter.setSoundClip(id)
			emitter.setLooping(True)
			emitter.play()

	def loadSettings(self):
		"""
		Load the settings from a python file and load them into the engine.
		Called in the ApplicationBase constructor.
		"""
		import settings
		self.settings = settings

		engineSetting = self.engine.getSettings()
		engineSetting.setDefaultFontGlyphs(str(TDS.readSetting("FontGlyphs", strip=False)))
		engineSetting.setDefaultFontPath(str(TDS.readSetting("Font")))
		engineSetting.setDefaultFontSize(12)
		engineSetting.setBitsPerPixel(int(TDS.readSetting("BitsPerPixel")))
		engineSetting.setInitialVolume(float(TDS.readSetting("InitialVolume")))
		engineSetting.setSDLRemoveFakeAlpha(int(TDS.readSetting("SDLRemoveFakeAlpha")))
		engineSetting.setScreenWidth(int(TDS.readSetting("ScreenWidth")))
		engineSetting.setScreenHeight(int(TDS.readSetting("ScreenHeight")))
		engineSetting.setRenderBackend(str(TDS.readSetting("RenderBackend")))
		engineSetting.setFullScreen(int(TDS.readSetting("FullScreen")))

		try:
			engineSetting.setWindowTitle(str(TDS.readSetting("WindowTitle")))
			engineSetting.setWindowIcon(str(TDS.readSetting("WindowIcon")))
		except:
			pass			
		try:
			engineSetting.setImageChunkingSize(int(TDS.readSetting("ImageChunkSize")))
		except:
			pass

	def initLogging(self):
		"""
		Initialize the LogManager.
		"""
		LogModules = TDS.readSetting("LogModules", type='list')
		self.log = fifelog.LogManager(self.engine, int(TDS.readSetting("LogToPrompt")), int(TDS.readSetting("LogToFile")))
		if LogModules:
			self.log.setVisibleModules(*LogModules)

	def createListener(self):
		pass # already created in constructor

	def _pump(self):
		if self.listener.quit:
			self.breakRequested = True
			
			# get the correct directory to save the map file to
			mapSaveDir = getUserDataDirectory("fife", "rio_de_hola") + "/maps"
			
			# create the directory structure if it does not exist
			if not os.path.isdir(mapSaveDir):
				os.makedirs(mapSaveDir)
			
			# save map file to directory
			self.world.save(mapSaveDir + "/savefile.xml")
		else:
			self.world.pump()

def main():
	app = IslandDemo()
	app.run()


if __name__ == '__main__':
	if TDS.readSetting("ProfilingOn", type='bool'):
		import hotshot, hotshot.stats
		print "Starting profiler"
		prof = hotshot.Profile("fife.prof")
		prof.runcall(main)
		prof.close()
		print "analysing profiling results"
		stats = hotshot.stats.load("fife.prof")
		stats.strip_dirs()
		stats.sort_stats('time', 'calls')
		stats.print_stats(20)
	else:
		if TDS.readSetting("UsePsyco", type='bool'):
			# Import Psyco if available
			try:
				import psyco
				psyco.full()
				print "Psyco acceleration in use"
			except ImportError:
				print "Psyco acceleration not used"
		else:
			print "Psyco acceleration not used"
		main()
