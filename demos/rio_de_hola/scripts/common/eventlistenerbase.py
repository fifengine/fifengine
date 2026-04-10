# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Event listener base helpers for rio_de_hola demo.

Provide a simple base class that registers for FIFE input and console
events and exposes no-op handlers that subclasses can override.
"""

from fife import fife
from fife.extensions.pychan.internal import get_manager


class EventListenerBase(
    fife.IKeyListener, fife.ICommandListener, fife.IMouseListener, fife.ConsoleExecuter
):
    """Base event listener that hooks into FIFE event systems.

    Subclasses should override the provided event handler methods as
    needed. Handlers are no-ops by default.
    """

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
        """Handle mouse press events."""
        pass

    def mouseReleased(self, evt):
        """Handle mouse release events."""
        pass

    def mouseEntered(self, evt):
        """Handle mouse enter events."""
        pass

    def mouseExited(self, evt):
        """Handle mouse exit events."""
        pass

    def mouseClicked(self, evt):
        """Handle mouse click events."""
        pass

    def mouseWheelMovedUp(self, evt):
        """Handle mouse wheel up events."""
        pass

    def mouseWheelMovedDown(self, evt):
        """Handle mouse wheel down events."""
        pass

    def mouseMoved(self, evt):
        """Handle mouse move events."""
        pass

    def mouseDragged(self, evt):
        """Handle mouse drag events."""
        pass

    def keyPressed(self, evt):
        """Handle key press events."""
        pass

    def keyReleased(self, evt):
        """Handle key release events."""
        pass

    def onCommand(self, command):
        """Execute a received command string."""
        pass

    def onToolsClick(self):
        """Handle a tools button click in the UI."""
        print("No tools set up yet")

    def onConsoleCommand(self, command):
        """Execute a console command invoked via the UI."""
        pass

    def onWidgetAction(self, evt):
        """Handle UI widget actions (pychan events)."""
        pass
