# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
The basic application and main loop.

See the L{ApplicationBase} documentation.
"""

from fife import fife
from fife.extensions import pychan
from fife.extensions.basicapplication import ApplicationBase


class ExitEventListener(fife.IKeyListener):
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

        pychan.init(self.engine, debug=self._finalSetting["PychanDebug"])
        pychan.setupModalExecution(self.mainLoop, self.breakFromMainLoop)

    def createListener(self):
        self._listener = ExitEventListener(self)
        return self._listener
