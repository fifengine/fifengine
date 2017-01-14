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
The basic application and main loop.

See the L{ApplicationBase} documentation.
"""

from fife import fife
from fife.extensions.basicapplication import ApplicationBase

import rocket

class RocketEventListener(fife.IKeyListener, fife.ICommandListener):
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
		fife.ICommandListener.__init__(self)
		eventmanager.addCommandListener(self)
		self.quitrequested = False
		self.debuggeractive = False
		
	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		
		if keyval == fife.Key.ESCAPE:
			self.app.quit()

	def keyReleased(self, evt):
		keyval = evt.getKey().getValue()
		
		if keyval == fife.Key.F12:
			if not self.debuggeractive:
				self.app.guimanager.showDebugger()
				self.debuggeractive = True
			else:
				self.app.guimanager.hideDebugger()
				self.debuggeractive = False

	def onCommand(self, command):
		if command.getCommandType() == fife.CMD_QUIT_GAME:
			self.quitrequested = True
			command.consume()
			
class RocketApplicationBase(ApplicationBase):
	"""
	PychanApplicationBase is an extendable class that provides a basic environment for a FIFE-based client.
	This class should be extended if you 've built fife with librocket support and want to create a game
	using it.
	"""
	def __init__(self, setting=None):
		super(RocketApplicationBase, self).__init__(setting)
		
		settings = self.engine.getSettings()
		guimanager = fife.LibRocketManager()
		
		#transfer ownership to the engine
		guimanager.thisown = 0
		
		#initialize gui manager and set it as the engine's active gui manager
		guimanager.init(settings.getRenderBackend(), settings.getScreenWidth(), settings.getScreenHeight())
		self.engine.setGuiManager(guimanager)
		self.guimanager = guimanager
		self.engine.getEventManager().addSdlEventListener(guimanager)
		
		#get reference to the active rocket context
		self.rocketcontext = rocket.contexts['default']
		
	def createListener(self):
		self._listener = RocketEventListener(self)
		return self._listener
		
	def quit(self):
		self.rocketcontext.UnloadAllDocuments()
		self.rocketcontext.UnloadAllMouseCursors()

		#release reference to rocket context
		self.rocketcontext = None
		
		#call parent's quit
		super(RocketApplicationBase, self).quit()
