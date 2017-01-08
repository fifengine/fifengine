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

import PyCEGUI

class CEGUIEventListener(fife.IKeyListener, fife.ICommandListener):
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
		pass

	def onCommand(self, command):
		if command.getCommandType() == fife.CMD_QUIT_GAME:
			self.quitrequested = True
			command.consume()

DEFAULT_GUI_DIR = "gui/"

class CEGUIApplicationBase(ApplicationBase):
	def __init__(self, setting=None):
		super(CEGUIApplicationBase, self).__init__(setting)

		self._initGuiManager()
		self._loadCEGuiSettings()

	def _initGuiManager(self):
		settings = self.engine.getSettings()
		
		major_v, minor_v = map(int, PyCEGUI.Version__.split('.')[:2])

		#For CEGUI versions lower than 0.8.0 we use the old CEGuiManager
		if major_v == 0 and minor_v <= 7:
			guimanager = fife.CEGuiManager()
		else:
			guimanager = fife.CEGuiManager()

		#transfer ownership to the engine
		guimanager.thisown = 0

		self.guimanager = guimanager
		self.engine.setGuiManager(self.guimanager)
		self.engine.getEventManager().addSdlEventListener(self.guimanager)

	def _loadCEGuiSettings(self):
		self._loadResourcePaths()

	def _loadResourcePaths(self):

		resourceprovider = PyCEGUI.System.getSingleton().getResourceProvider()

		major_v, minor_v = map(int, PyCEGUI.Version__.split('.')[:2])
		if major_v == 0 and minor_v <= 7:
			resourcetypemap = { "schemes" : PyCEGUI.Scheme.setDefaultResourceGroup,
							    "imagesets" : PyCEGUI.Imageset.setDefaultResourceGroup,
							    "fonts" : PyCEGUI.Font.setDefaultResourceGroup,
							    "layouts" : PyCEGUI.WindowManager.setDefaultResourceGroup,
							    "looksnfeels" : PyCEGUI.WidgetLookManager.setDefaultResourceGroup,
							   }
		else:
			resourcetypemap = { "schemes" : PyCEGUI.Scheme.setDefaultResourceGroup,
							    "imagesets" : PyCEGUI.ImageManager.setImagesetDefaultResourceGroup,
							    "fonts" : PyCEGUI.Font.setDefaultResourceGroup,
							    "layouts" : PyCEGUI.WindowManager.setDefaultResourceGroup,
							    "looksnfeels" : PyCEGUI.WidgetLookManager.setDefaultResourceGroup,
							   }


		if not self._setting:
			for restype, res_setfunc in resourcetypemap.iteritems():
				resourceprovider.setResourceGroupDirectory(restype, DEFAULT_GUI_DIR + restype)
				res_setfunc(restype)
		else:
			for restype, res_setfunc in resourcetypemap.iteritems():
				path = self._setting.get("CEGUI", restype)
				if path:
					resourceprovider.setResourceGroupDirectory(restype, path)
					res_setfunc(restype)
				else:
					#set default path
					resourceprovider.setResourceGroupDirectory(restype, DEFAULT_GUI_DIR + restype)
					res_setfunc(restype)

		parser = PyCEGUI.System.getSingleton().getXMLParser()
		if parser.isPropertyPresent("SchemaDefaultResourceGroup"):
			path = self._setting.get("CEGUI", "schemas")
			if path:
				rp.setResourceGroupDirectory("schemas", path)
			else:
				rp.setResourceGroupDirectory("schemas", DEFAULT_GUI_DIR + "schemas")

			parser.setProperty("SchemaDefaultResourceGroup", "schemas")


	def createListener(self):
		self._listener = CEGUIEventListener(self)
		return self._listener
