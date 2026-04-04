#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import os
import time

from fife import fife
from fife.extensions.pychan.internal import get_manager
from fife.extensions.pychan.pychanbasicapplication import PychanApplicationBase

from scripts.gamecontroller import GameController


class KeyFilter(fife.IKeyFilter):
    """
    This is the implementation of the fife.IKeyFilter class.

    Prevents any filtered keys from being consumed by fifechan.
    """

    def __init__(self, keys):
        fife.IKeyFilter.__init__(self)
        self._keys = keys

    def isFiltered(self, event):
        return event.getKey().getValue() in self._keys


class ApplicationListener(fife.IKeyListener, fife.ICommandListener, fife.ConsoleExecuter):
    """
    Listens for window commands, console commands and keyboard input.

    Does not process game related input.
    """

    def __init__(self, engine, gamecontroller):
        """
        Initializes all listeners and registers itself with the eventmanager.
        """
        self._engine = engine
        self._gamecontroller = gamecontroller
        self._eventmanager = self._engine.getEventManager()

        fife.IKeyListener.__init__(self)
        self._eventmanager.addKeyListener(self)

        fife.ICommandListener.__init__(self)
        self._eventmanager.addCommandListener(self)

        fife.ConsoleExecuter.__init__(self)
        get_manager().getConsole().setConsoleExecuter(self)

        keyfilter = KeyFilter(
            [fife.Key.ESCAPE, fife.Key.BACKQUOTE, fife.Key.PRINT_SCREEN]
        )
        keyfilter.__disown__()

        self._eventmanager.setKeyFilter(keyfilter)

        self.quit = False

    def keyPressed(self, event):
        """
        Processes any non game related keyboar input.
        """
        if event.isConsumed():
            return

        keyval = event.getKey().getValue()

        if keyval == fife.Key.ESCAPE:
            self.quit = True
            event.consume()
        elif keyval == fife.Key.BACKQUOTE:
            get_manager().getConsole().toggleShowHide()
            event.consume()
        elif keyval == fife.Key.PRINT_SCREEN:
            self._engine.getRenderBackend().captureScreen(
                time.strftime("%Y%m%d_%H%M%S", time.localtime()) + ".png"
            )
            event.consume()

    def keyReleased(self, event):
        pass

    def onCommand(self, command):
        if command.getCommandType() == fife.CMD_QUIT_GAME:
            self.quit = True
            command.consume()

    def onConsoleCommand(self, command):
        result = ""

        args = command.split(" ")
        cmd = []
        for arg in args:
            arg = arg.strip()
            if arg != "":
                cmd.append(arg)

        if cmd[0].lower() in ("quit", "exit"):
            self.quit = True
            result = "quitting"
        elif cmd[0].lower() in ("help"):
            helptextfile = self._gamecontroller.settings.get(
                "RPG", "HelpText", "misc/help.txt"
            )
            get_manager().getConsole().println(open(helptextfile).read())
            result = "--OK--"
        elif cmd[0].lower() in ("eval"):
            try:
                result = str(eval(command.lstrip(cmd[0])))
            except Exception:
                result = "Invalid eval statement..."
        else:
            result = self._gamecontroller.onConsoleCommand(command)

        if not result:
            result = "Command Not Found..."

        return result

    def onToolsClick(self):
        print("No tools set up yet")


class RPGApplication(PychanApplicationBase):
    """
    The main application.  It inherits fife.extensions.ApplicationBase.

    Implements ApplicationBase._pump().
    """

    def __init__(self, TDS):
        super().__init__(TDS)
        self._settings = TDS
        self.engine.getVFS().addNewSource(os.getcwd())

        self._gamecontroller = GameController(self, self.engine, self._settings)

    def createListener(self):
        """
        @note: This function had to be overloaded otherwise the default
        listener would have been created.
        """
        self._listener = ApplicationListener(self.engine, self._gamecontroller)
        return self._listener

    def requestQuit(self):
        """
        Sends the quit command to the application's listener.  We could set
        self.quitRequested to true also but this is a good example on how
        to build and dispatch a fife.Command.
        """
        cmd = fife.Command()
        cmd.setSource(None)
        cmd.setCommandType(fife.CMD_QUIT_GAME)
        self.engine.getEventManager().dispatchCommand(cmd)

    def _pump(self):
        if self._listener.quit:
            self._gamecontroller.endGame()
            self.quit()
        else:
            self._gamecontroller.pump()

    def _getLogManager(self):
        return self._log

    logger = property(_getLogManager)
