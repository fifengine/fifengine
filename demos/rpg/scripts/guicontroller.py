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
    """Quest dialog used to present quest details and accept/decline actions.

    The dialog is rendered as a speech bubble that stays anchored above the
    quest giver, so the tail always points back at the NPC.
    """

    #: Distance in pixels between the NPC anchor and the bubble edge.
    ANCHOR_OFFSET = 12

    def __init__(self, guicontroller, questgiver):
        super().__init__(guicontroller)
        self._widget = pychan.loadXML("gui/quest.xml")
        self._questgiver = questgiver
        self._quest = questgiver.getNextQuest()

        self._bubble = self._widget.findChild(name="questbubble")
        self._questname = self._widget.findChild(name="questname")
        self._questname.text = str(self._quest.name)

        self._questtext = self._widget.findChild(name="questtext")
        self._questtext.text = str(self._quest.text)

        eventMap = {
            "accept": self.questAccepted,
            "decline": self._widget.hide,
        }

        self._widget.mapEvents(eventMap)

    def anchorToQuestgiver(self):
        """Move the bubble next to the quest giver and aim the tail at it.

        Returns
        -------
        bool
            True when the quest giver could be located on screen.
        """
        anchor = self._screenAnchor()
        if anchor is None:
            return False

        size = self._bubble.size
        # Place the bubble above the NPC and clamp it into the viewport.
        x = min(max(anchor[0] - (size[0] // 2), 0), max(0, self._viewport()[0] - size[0]))
        y = max(anchor[1] - self.ANCHOR_OFFSET - size[1], 0)
        self._bubble.position = x, y

        # Tail sits on the closer horizontal edge, pointing back at the NPC.
        left_half = anchor[0] < x + (size[0] // 2)
        self._bubble.tail_direction = "BottomLeft" if left_half else "BottomRight"
        self._bubble.tail_offset = 0.75 if left_half else 0.25
        return True

    def _screenAnchor(self):
        """
        Return the quest giver position in screen coordinates.

        Returns
        -------
        tuple | None
            The (x, y) screen position, or None when it cannot be determined.
        """
        camera = self._guicontroller.getDefaultCamera()
        instance = self._questgiver.instance
        if camera is None or instance is None:
            return None
        screen = camera.toScreenCoordinates(instance.getLocation().getMapCoordinates())
        if screen is None:
            return None
        return int(screen.x), int(screen.y)

    def _viewport(self):
        """
        Return the drawable viewport size in pixels.

        Returns
        -------
        tuple[int, int]
            The viewport width and height.
        """
        camera = self._guicontroller.getDefaultCamera()
        if camera is None:
            return 0, 0
        viewport = camera.getViewPort()
        return viewport.w, viewport.h

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
        self._questdialog = None

    def getDefaultCamera(self):
        """
        Return the camera used by the active scene.

        Returns
        -------
        fife.Camera | None
            The default camera, or None when no scene is loaded.
        """
        scene = self._gamecontroller.scene
        if scene is None:
            return None
        camera = scene.cameras.get(
            self._settings.get("RPG", "DefaultCameraName", "camera1")
        )
        if camera is None:
            return None
        return camera

    def update(self):
        """Keep anchored dialogs attached to their quest giver."""
        if self._questdialog is not None and not self._questdialog.widget.isVisible():
            self._questdialog = None
        elif self._questdialog is not None:
            self._questdialog.anchorToQuestgiver()

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
        if self._questdialog is not None:
            self._questdialog.widget.hide()
        questdlg = QuestDialog(self._gamecontroller, questgiver)
        questdlg.anchorToQuestgiver()
        questdlg.widget.show()
        self._questdialog = questdlg
