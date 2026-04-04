# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
The basic application and main loop.

See the L{ApplicationBase} documentation.
"""

import PyCEGUI

from fife import fife
from fife.extensions.basicapplication import ApplicationBase


class CEGUIEventListener(fife.IKeyListener, fife.ICommandListener):
    """
    Default, rudimentary event listener.

    Will cause the application to quit on pressing ESC.
    """

    def __init__(self, app):
        self.app = app
        self.engine = app.engine
        eventmanager = self.engine.getEventManager()
        # eventmanager.setNonConsumableKeys([fife.Key.ESCAPE])
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
        super().__init__(setting)

        self._initGuiManager()
        self._loadCEGuiSettings()

    def _initGuiManager(self):
        settings = self.engine.getSettings()
        if not settings:
            raise RuntimeError("Engine settings unavailable during CEGUI initialization")

        major_v, minor_v = list(map(int, PyCEGUI.Version__.split(".")[:2]))

        # For CEGUI versions lower than 0.8.0 we use the old CEGuiManager
        if major_v == 0 and minor_v <= 7:
            guimanager = fife.CEGuiManager()
        else:
            guimanager = fife.CEGuiManager()

        # transfer ownership to the engine
        guimanager.thisown = 0

        self.guimanager = guimanager
        self.engine.setGuiManager(self.guimanager)
        self.engine.getEventManager().addSdlEventListener(self.guimanager)

    def _loadCEGuiSettings(self):
        self._loadResourcePaths()

    def _loadResourcePaths(self):

        resourceprovider = PyCEGUI.System.getSingleton().getResourceProvider()

        major_v, minor_v = list(map(int, PyCEGUI.Version__.split(".")[:2]))
        if major_v == 0 and minor_v <= 7:
            resourcetypemap = {
                "schemes": PyCEGUI.Scheme.setDefaultResourceGroup,
                "imagesets": PyCEGUI.Imageset.setDefaultResourceGroup,
                "fonts": PyCEGUI.Font.setDefaultResourceGroup,
                "layouts": PyCEGUI.WindowManager.setDefaultResourceGroup,
                "looksnfeels": PyCEGUI.WidgetLookManager.setDefaultResourceGroup,
            }
        else:
            resourcetypemap = {
                "schemes": PyCEGUI.Scheme.setDefaultResourceGroup,
                "imagesets": PyCEGUI.ImageManager.setImagesetDefaultResourceGroup,
                "fonts": PyCEGUI.Font.setDefaultResourceGroup,
                "layouts": PyCEGUI.WindowManager.setDefaultResourceGroup,
                "looksnfeels": PyCEGUI.WidgetLookManager.setDefaultResourceGroup,
            }

        if not self._setting:
            for restype, res_setfunc in resourcetypemap.items():
                resourceprovider.setResourceGroupDirectory(
                    restype, DEFAULT_GUI_DIR + restype
                )
                res_setfunc(restype)
        else:
            for restype, res_setfunc in resourcetypemap.items():
                path = self._setting.get("CEGUI", restype)
                if path:
                    resourceprovider.setResourceGroupDirectory(restype, path)
                    res_setfunc(restype)
                else:
                    # set default path
                    resourceprovider.setResourceGroupDirectory(
                        restype, DEFAULT_GUI_DIR + restype
                    )
                    res_setfunc(restype)

        parser = PyCEGUI.System.getSingleton().getXMLParser()
        if parser.isPropertyPresent("SchemaDefaultResourceGroup"):
            path = self._setting.get("CEGUI", "schemas")
            if path:
                resourceprovider.setResourceGroupDirectory("schemas", path)
            else:
                resourceprovider.setResourceGroupDirectory(
                    "schemas", DEFAULT_GUI_DIR + "schemas"
                )

            parser.setProperty("SchemaDefaultResourceGroup", "schemas")

    def createListener(self):
        self._listener = CEGUIEventListener(self)
        return self._listener
