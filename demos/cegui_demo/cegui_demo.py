#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
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
import sys, os, re

fife_path = os.path.join('..','..','engine','python')
if os.path.isdir(fife_path) and fife_path not in sys.path:
	sys.path.insert(0,fife_path)

from fife import fife
from fife.extensions.fife_settings import Setting
from fife.extensions.cegui.ceguibasicapplication import CEGUIApplicationBase

import PyCEGUI

print "Using the FIFE python module found here: ", os.path.dirname(fife.__file__)

DemoSettings = Setting(app_name="CEGUI Demo")

class CEGUIDemo(CEGUIApplicationBase):
	
	def __init__(self, setting = None):
		super(CEGUIDemo, self).__init__(setting)
		
		self._loadSchemes()
		
		root = myRoot = PyCEGUI.WindowManager.getSingleton().createWindow( "DefaultWindow", "_MasterRoot" )
		PyCEGUI.System.getSingleton().setGUISheet(myRoot)
		
		newWindow = PyCEGUI.WindowManager.getSingleton().loadWindowLayout("MyConsole.layout","second_")
		root.addChildWindow(newWindow)
		
	def _loadSchemes(self):
		PyCEGUI.SchemeManager.getSingleton().create("TaharezLook.scheme")
	
	def _pump(self):
		if self._listener.quitrequested:
			self.quit()
	
if __name__ == '__main__':
	app = CEGUIDemo(DemoSettings)
	app.run()
