#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""GUI controller helpers for the RPG demo (menus and dialog windows)."""

from fife.extensions import pychan


class Window:
    """Base window wrapper that holds common GUI references."""

    def __init__(self, gamecontroller):
        """Initialize the window with references to controllers and settings."""
        self._guicontroller = gamecontroller.guicontroller
        self._gamecontroller = gamecontroller
        self._settings = gamecontroller.settings

        self._widget = None

    def _getWidget(self):
        """Return the underlying pychan widget instance.

        Returns
        -------
        pychan.Widget | None
            The underlying widget instance, or None if not initialized.
        """
        return self._widget

    widget = property(_getWidget)


class MainMenu(Window):
    """Main menu window wrapper."""

    def __init__(self, gamecontroller):
        super().__init__(gamecontroller)
        self._widget = pychan.loadXML("gui/mainmenu.xml")

        self._newgame = self._widget.findChild(name="new_game")
        self._credits = self._widget.findChild(name="credits")
        self._quit = self._widget.findChild(name="quit")

        self._widget.position = (0, 0)

        eventMap = {
            "new_game": self._gamecontroller.newGame,
            "settings": self._settings.showSettingsDialog,
            "credits": self._guicontroller.showCredits,
            "quit": self._gamecontroller.quit,
        }

        self._widget.mapEvents(eventMap)


class Credits(Window):
    """Credits window wrapper."""

    def __init__(self, gamecontroller):
        super().__init__(gamecontroller)
        self._widget = pychan.loadXML("gui/credits.xml")

        eventMap = {
            "close": self._guicontroller.hideCredits,
        }

        self._widget.mapEvents(eventMap)


class QuestDialog(Window):
    """Quest dialog used to present quest details and accept/decline actions."""

    def __init__(self, guicontroller, questgiver):
        super().__init__(guicontroller)
        self._widget = pychan.loadXML("gui/quest.xml")
        self._questgiver = questgiver
        self._quest = questgiver.getNextQuest()

        self._questname = self._widget.findChild(name="questname")
        self._questname.text = str(self._quest.name)

        self._questtext = self._widget.findChild(name="questtext")
        self._questtext.text = str(self._quest.text)

        eventMap = {
            "accept": self.questAccepted,
            "decline": self._widget.hide,
        }

        self._widget.mapEvents(eventMap)

    def questAccepted(self):
        """Handle the quest acceptance action from the dialog."""
        self._guicontroller._gamecontroller.logger.log_debug(
            "Quest [" + self._quest.name + "] has been accepted"
        )
        self._questgiver.activateQuest(self._quest)
        self._widget.hide()


class GUIController:
    """High-level GUI controller exposing menu and dialog helpers."""

    def __init__(self, gamecontroller):
        self._gamecontroller = gamecontroller
        self._engine = gamecontroller.engine
        self._settings = gamecontroller.settings

        self._mainmenu = None
        self._credits = None

    def showMainMenu(self):
        """Show the main menu, loading it if necessary."""
        if self._mainmenu:
            self._mainmenu.widget.show()
        else:
            # load and show the main menu
            self._mainmenu = MainMenu(self._gamecontroller)
            self._mainmenu.widget.show()

    def hideMainMenu(self):
        """Hide the main menu and release its resources."""
        if self._mainmenu:
            self._mainmenu.widget.hide()
            self._mainmenu = None

    def showCredits(self):
        """Show the credits window, loading it if necessary."""
        if self._credits:
            self._credits.widget.show()
        else:
            self._credits = Credits(self._gamecontroller)
            self._credits.widget.show()

    def hideCredits(self):
        """Hide the credits window and release its resources."""
        if self._credits:
            self._credits.widget.hide()
            self._credits = None

    def showQuestDialog(self, questgiver):
        """Instantiate and show the quest dialog for the given questgiver."""
        questdlg = QuestDialog(self._gamecontroller, questgiver)
        questdlg.widget.show()
