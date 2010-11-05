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

"""
The basic application and main loop.

See the L{ApplicationBase} documentation.
"""

from fife import fife
from fife.extensions import fifelog
from fife.extensions.serializers.xmlanimation import XMLAnimationLoader
from fife.extensions import pychan
from fife.extensions.fife_settings import Setting

class ExitEventListener(fife.IKeyListener):
	"""
	Default, rudimentary event listener.

	Will cause the application to quit on pressing ESC.
	"""
	def __init__(self, app):
		self.app = app
		self.engine = app.engine
		eventmanager = self.engine.getEventManager()
		#eventmanager.setNonConsumableKeys([fife.Key.ESCAPE])
		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)
		self.quitRequested = False

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		if keyval == fife.Key.ESCAPE:
			self.app.quit()
		elif keyval == fife.Key.F10:
			self.engine.getGuiManager().getConsole().toggleShowHide()
			evt.consume()

	def keyReleased(self, evt):
		pass

class ApplicationBase(object):
	"""
	ApplicationBase is an extendable class that provides a basic environment for a FIFE-based client.

	The unextended application reads in and initializes engine settings, sets up a simple event
	listener, and pumps the engine while listening for a quit message. Specialized applications can
	modify settings.py to change initial engine settings. They can provide their own event listener
	by overriding L{createListener}. And they can override the L{_pump} method
	to define runtime behavior of the application.

	"""
	def __init__(self, setting=None):
		if setting:
			self._setting = setting
		else:
			self._setting =  Setting(app_name="", settings_file="./settings.xml", settings_gui_xml="")

		self.engine = fife.Engine()

		self.loadSettings()
		self.initLogging()

		self.engine.init()

		self._animationloader = XMLAnimationLoader(self.engine.getImagePool(), self.engine.getVFS())
		self.engine.getAnimationPool().addResourceLoader(self._animationloader)

		pychan.init(self.engine, debug=self._setting.get("FIFE", "PychanDebug", False))
		pychan.setupModalExecution(self.mainLoop,self.breakFromMainLoop)

		self.quitRequested = False
		self.breakRequested = False
		self.returnValues = []
		
	def loadSettings(self):
		"""
		Load the settings from a python file and load them into the engine.
		Called in the ApplicationBase constructor.
		"""

		glyphDft = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&amp;`'*#=[]\\\""
		engineSetting = self.engine.getSettings()
		engineSetting.setDefaultFontGlyphs(self._setting.get("FIFE", "FontGlyphs", glyphDft))
		engineSetting.setDefaultFontPath(self._setting.get("FIFE", "Font", "fonts/FreeSans.ttf"))
		engineSetting.setDefaultFontSize(self._setting.get("FIFE", "DefaultFontSize", 12))
		engineSetting.setBitsPerPixel(self._setting.get("FIFE", "BitsPerPixel", 0))
		engineSetting.setInitialVolume(self._setting.get("FIFE", "InitialVolume", 5.0))
		engineSetting.setSDLRemoveFakeAlpha(self._setting.get("FIFE", "SDLRemoveFakeAlpha", 1))
		(width, height) = self._setting.get("FIFE", "ScreenResolution", "1024x768").split('x')
		engineSetting.setScreenWidth(int(width))
		engineSetting.setScreenHeight(int(height))
		engineSetting.setRenderBackend(self._setting.get("FIFE", "RenderBackend", "OpenGL"))
		engineSetting.setFullScreen(self._setting.get("FIFE", "FullScreen", False))
		engineSetting.setVideoDriver(self._setting.get("FIFE", "VideoDriver", ""))
		engineSetting.setLightingModel(self._setting.get("FIFE", "Lighting", 0))

		try:
			engineSetting.setColorKeyEnabled(self._setting.get("FIFE", "ColorKeyEnabled", False))
		except:
			pass

		try:
			key = self._setting.get("FIFE", "ColorKey", "255,0,255").split(',')
			engineSetting.setColorKey(int(key[0]), int(key[1]), int(key[2]))
		except:
			pass

		try:
			engineSetting.setWindowTitle(self._setting.get("FIFE", "WindowTitle", "No window title set"))
			engineSetting.setWindowIcon(self._setting.get("FIFE", "WindowIcon", ""))
		except:
			pass

		try:
			engineSetting.setImageChunkingSize(self._setting.get("FIFE", "ImageChunkSize", 256))
		except:
			pass

	def initLogging(self):
		"""
		Initialize the LogManager.
		"""

		engineSetting = self.engine.getSettings()
		logmodules = self._setting.get("FIFE", "LogModules", ["controller"])

		#log to both the console and log file
		self._log = fifelog.LogManager(self.engine,
									   self._setting.get("FIFE", "LogToPrompt", "0"),
									   self._setting.get("FIFE", "LogToFile", "0"))

		self._log.setLevelFilter(self._setting.get("FIFE", "LogLevelFilter", fife.LogManager.LEVEL_DEBUG))

		if logmodules:
			self._log.setVisibleModules(*logmodules)

	def createListener(self):
		"""
		This creates a default event listener, which will just close the program
		after pressing ESC.

		You should override this method to provide your own event handling.
		"""
		return ExitEventListener(self)

	def run(self):
		"""
		Initialize the event listener and event loop - and start it.
		"""
		eventlistener = self.createListener()
		self.engine.initializePumping()
		retval = self.mainLoop()
		self.engine.finalizePumping()
		self.engine.destroy()
		return retval

	def mainLoop(self):
		"""
		The programs main loop.

		Do not override this, instead provide your own L{_pump} method.
		You can call this recursively, e.g. to provide synchronous
		Dialogs :-) and break out of the current mainLoop by calling
		L{breakFromMainLoop}. It will return the argument passed
		to L{breakFromMainLoop}.
		"""
		self.returnValues.append(None)
		while not self.quitRequested:
			try:
				self.engine.pump()
			except RuntimeError, e:
				print str(e)

			self._pump()

			if self.breakRequested:
				self.breakRequested = False
				break

		return self.returnValues.pop()

	def breakFromMainLoop(self,returnValue):
		"""
		Break from the currently running L{mainLoop}.

		The passed argument will be returned by the mainLoop.
		"""
		self.returnValues[-1] = returnValue
		self.breakRequested = True


	def _pump(self):
		"""
		Application pump.

		Derived classes can specialize this for unique behavior.
		This is called every frame.
		"""

	def quit(self):
		"""
		Quit the application. Really!
		"""
		self.quitRequested = True
