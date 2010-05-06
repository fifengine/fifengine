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

from fife import fife

class EventListenerBase(fife.IKeyListener, fife.ICommandListener, fife.IMouseListener, fife.ConsoleExecuter):
	def __init__(self, engine, regKeys=False, regCmd=False, regMouse=False, regConsole=False, regWidget=False):
		self.eventmanager = engine.getEventManager()

		fife.IKeyListener.__init__(self)
		if regKeys:
			self.eventmanager.addKeyListener(self)
		fife.ICommandListener.__init__(self)
		if regCmd:
			self.eventmanager.addCommandListener(self)
		fife.IMouseListener.__init__(self)
		if regMouse:
			self.eventmanager.addMouseListener(self)
		fife.ConsoleExecuter.__init__(self)
		if regConsole:
			engine.getGuiManager().getConsole().setConsoleExecuter(self)


	def mousePressed(self, evt):
		pass
	def mouseReleased(self, evt):
		pass	
	def mouseEntered(self, evt):
		pass
	def mouseExited(self, evt):
		pass
	def mouseClicked(self, evt):
		pass
	def mouseWheelMovedUp(self, evt):
		pass	
	def mouseWheelMovedDown(self, evt):
		pass
	def mouseMoved(self, evt):
		pass
	def mouseDragged(self, evt):
		pass
	def keyPressed(self, evt):
		pass
	def keyReleased(self, evt):
		pass
	def onCommand(self, command):
		pass
	def onToolsClick(self):
		print "No tools set up yet"
	def onConsoleCommand(self, command):
		pass
	def onWidgetAction(self, evt):
		pass
