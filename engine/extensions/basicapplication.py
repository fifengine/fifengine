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

import fife
import fifelog
from serializers.xmlanimation import XMLAnimationLoader

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
	def __init__(self):
		self.engine = fife.Engine()

		self.loadSettings()
		self.initLogging()

		self.engine.init()

		self._animationloader = XMLAnimationLoader(self.engine.getImagePool(), self.engine.getVFS())
		self.engine.getAnimationPool().addResourceLoader(self._animationloader)

		self.quitRequested = False
		self.breakRequested = False
		self.returnValues = []

	def loadSettings(self):
		"""
		Load the settings from a python file and load them into the engine.
		Called in the ApplicationBase constructor.
		"""
		import settings
		self.settings = settings

		engineSetting = self.engine.getSettings()
		engineSetting.setDefaultFontGlyphs(settings.FontGlyphs)
		engineSetting.setDefaultFontPath(settings.Font)
		engineSetting.setDefaultFontSize(12)
		engineSetting.setBitsPerPixel(settings.BitsPerPixel)
		engineSetting.setFullScreen(settings.FullScreen)
		engineSetting.setInitialVolume(settings.InitialVolume)
		engineSetting.setRenderBackend(settings.RenderBackend)
		engineSetting.setSDLRemoveFakeAlpha(settings.SDLRemoveFakeAlpha)
		engineSetting.setScreenWidth(settings.ScreenWidth)
		engineSetting.setScreenHeight(settings.ScreenHeight)


		try:
			engineSetting.setWindowTitle(settings.WindowTitle)
			engineSetting.setWindowIcon(settings.WindowIcon)
		except:
			pass			
		try:
			engineSetting.setImageChunkingSize(settings.ImageChunkSize)
		except:
			pass

	def initLogging(self):
		"""
		Initialize the LogManager.
		"""
		self.log = fifelog.LogManager(self.engine, self.settings.LogToPrompt, self.settings.LogToFile)
		if self.settings.LogModules:
			self.log.setVisibleModules(*self.settings.LogModules)

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
