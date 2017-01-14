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
from fife.extensions import fifelog
from fife.extensions.basicapplication import ApplicationBase
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
			if pychan.manager.hook.guimanager.isConsoleEnabled():
				pychan.manager.hook.guimanager.getConsole().toggleShowHide()
				evt.consume()

	def keyReleased(self, evt):
		pass

class PychanApplicationBase(ApplicationBase):
	"""
	PychanApplicationBase is an extendable class that provides a basic environment for a FIFE-based client.
	This class should be extended if you 've build fife with fifechan support, in order to use pychan.
	"""
	def __init__(self, setting=None):
		super(PychanApplicationBase, self).__init__(setting)
		
		pychan.init(self.engine, debug = self._finalSetting['PychanDebug'])
		pychan.setupModalExecution(self.mainLoop,self.breakFromMainLoop)
		
	def createListener(self):
		self._listener = ExitEventListener(self)
		return self._listener
