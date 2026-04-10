#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Event listener base classes and adapters for the shooter demo."""

from fife import fife
from fife.extensions.pychan.internal import get_manager


class EventListenerBase(
    fife.IKeyListener, fife.ICommandListener, fife.IMouseListener, fife.ConsoleExecuter
):
    """Base class providing empty event handler implementations."""

    def __init__(
        self,
        engine,
        regKeys=False,
        regCmd=False,
        regMouse=False,
        regConsole=False,
        regWidget=False,
    ):
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
            get_manager().getConsole().setConsoleExecuter(self)

    def mousePressed(self, evt):
        """Handle mouse pressed events (override as needed)."""
        pass

    def mouseReleased(self, evt):
        """Handle mouse released events (override as needed)."""
        pass

    def mouseEntered(self, evt):
        """Handle mouse entered events (override as needed)."""
        pass

    def mouseExited(self, evt):
        """Handle mouse exited events (override as needed)."""
        pass

    def mouseClicked(self, evt):
        """Handle mouse clicked events (override as needed)."""
        pass

    def mouseWheelMovedUp(self, evt):
        """Handle mouse wheel moved up events (override as needed)."""
        pass

    def mouseWheelMovedDown(self, evt):
        """Handle mouse wheel moved down events (override as needed)."""
        pass

    def mouseMoved(self, evt):
        """Handle mouse moved events (override as needed)."""
        pass

    def mouseDragged(self, evt):
        """Handle mouse dragged events (override as needed)."""
        pass

    def keyPressed(self, evt):
        """Handle key pressed events (override as needed)."""
        pass

    def keyReleased(self, evt):
        """Handle key released events (override as needed)."""
        pass

    def onCommand(self, command):
        """Handle console/command events (override as needed)."""
        pass

    def onToolsClick(self):
        """Handle tools click; default prints a message."""
        print("No tools set up yet")

    def onConsoleCommand(self, command):
        """Handle console command input (override as needed)."""
        pass

    def onWidgetAction(self, evt):
        """Handle widget actions (override as needed)."""
        pass
