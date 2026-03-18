#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

# ------------------------------------------------------------------------------
# This is the shooter demo of Fifengine.
# ------------------------------------------------------------------------------

import os
import sys

fife_path = os.path.join("..", "..", "engine", "python")
if os.path.isdir(fife_path) and fife_path not in sys.path:
    sys.path.insert(0, fife_path)

from fife import fife  # noqa: E402

print("Using the FIFE python module found here: ", os.path.dirname(fife.__file__))

from fife.extensions.pychan.fife_pychansettings import FifePychanSettings  # noqa: E402
from fife.extensions.pychan.pychanbasicapplication import (  # noqa: E402
    PychanApplicationBase,
)
from scripts import world  # noqa: E402
from scripts.common import eventlistenerbase  # noqa: E402

TDS = FifePychanSettings(app_name="shooter", settings_file="./settings.xml")

TDS.setAvailableScreenResolutions(["1024x768"])


class ApplicationListener(eventlistenerbase.EventListenerBase):
    def __init__(self, engine, world):
        super(ApplicationListener, self).__init__(
            engine,
            regKeys=True,
            regCmd=True,
            regMouse=False,
            regConsole=True,
            regWidget=True,
        )
        self._engine = engine
        self._world = world
        self._quit = False

    def keyPressed(self, evt):
        keyval = evt.getKey().getValue()
        if keyval == fife.Key.ESCAPE:
            self._world.showMainMenu()
            evt.consume()

    def onCommand(self, command):
        if command.getCommandType() == fife.CMD_QUIT_GAME:
            self._quit = True
            command.consume()


class Shooter(PychanApplicationBase):
    def __init__(self):
        super(Shooter, self).__init__(TDS)
        self.engine.getVFS().addNewSource(os.getcwd())

        self._world = world.World(self, self.engine, self._setting)
        self._listener = ApplicationListener(self.engine, self._world)

    def requestQuit(self):
        cmd = fife.Command()
        cmd.setSource(None)
        cmd.setCommandType(fife.CMD_QUIT_GAME)
        self.engine.getEventManager().dispatchCommand(cmd)

    def createListener(self):
        pass  # already created in constructor

    def _pump(self):
        if self._listener._quit:
            self.breakRequested = True
        else:
            self._world.pump()


def main():
    app = Shooter()
    app.run()


if __name__ == "__main__":
    main()
