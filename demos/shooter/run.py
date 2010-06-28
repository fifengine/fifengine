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
              
TDS.setAvailableScreenResolutions(["1024x768"])

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
		super(Shooter,self).__init__(TDS)
		
		self._world = world.World(self, self.engine, self._setting)
		self._listener = ApplicationListener(self.engine, self._world)

	def requestQuit(self):
		cmd = fife.Command()
		cmd.setSource(None)
		cmd.setCommandType(fife.CMD_QUIT_GAME)
		self.engine.getEventManager().dispatchCommand(cmd)
		
			
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
