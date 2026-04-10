#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""RPG demo game controller and input listener implementations."""

import glob
import os
import uuid

from fife import fife
from fife.extensions.pychan.internal import get_manager

from scripts.actors.baseactor import (
    AttackAction,
    EnterPortalAction,
    PickUpItemAction,
    TalkAction,
)
from scripts.guicontroller import GUIController
from scripts.misc.exceptions import ObjectAlreadyInSceneError, ObjectNotFoundError
from scripts.objects.baseobject import GameObjectTypes
from scripts.quests.questmanager import QuestManager
from scripts.scene import Scene


class KeyState:
    """Track keyboard key states (False = down, True = up)."""

    def __init__(self):
        self._keystate = {}

    def updateKey(self, key, state):
        """Update the stored state for a keyboard key."""
        self._keystate[key] = state

    def checkKey(self, key):
        """Return the stored state for a key, or False if unknown.

        Returns
        -------
        bool
            The stored key state, or ``False`` if the key is not tracked.
        """
        if key in self._keystate:
            return self._keystate[key]
        else:
            return False

    def reset(self):
        """Reset the keystate dictionary (assumes all keys are False)."""
        self._keystate.clear()


class GameListener(fife.IKeyListener, fife.IMouseListener):
    """Main game listener for mouse and keyboard events; attachable to the event manager."""

    def __init__(self, gamecontroller):
        self._engine = gamecontroller.engine
        self._gamecontroller = gamecontroller
        self._settings = gamecontroller.settings
        self._eventmanager = self._engine.getEventManager()

        fife.IMouseListener.__init__(self)
        fife.IKeyListener.__init__(self)

        self._attached = False

        self._lastmousepos = (0.0, 0.0)

    def attach(self):
        """Attach this listener to the engine event manager."""
        if not self._attached:
            self._gamecontroller.keystate.reset()
            self._eventmanager.addMouseListenerFront(self)
            self._eventmanager.addKeyListenerFront(self)
            self._attached = True

    def detach(self):
        """Detach this listener from the engine event manager."""
        if self._attached:
            self._eventmanager.removeMouseListener(self)
            self._eventmanager.removeKeyListener(self)
            self._attached = False

    def mousePressed(self, event):
        """Handle mouse press events and dispatch possible game actions."""
        # mouse press was consumed by some pychan widget
        if event.isConsumedByWidgets():
            return

        clickpoint = fife.Point3D(event.getX(), event.getY(), 0)

        if event.getButton() == fife.MouseEvent.LEFT:
            self._lastmousepos = (clickpoint.x, clickpoint.y)

            # cancel last action
            self._gamecontroller.scene.player.nextaction = None

            self._gamecontroller.scene.player.walk(
                self._gamecontroller.scene.getLocationAt(clickpoint)
            )

            actor_instances = self._gamecontroller.scene.getInstancesAt(
                clickpoint, self._gamecontroller.scene.actorlayer
            )
            item_instances = self._gamecontroller.scene.getInstancesAt(
                clickpoint, self._gamecontroller.scene.itemlayer
            )

            if actor_instances:
                actor_instance = None
                for actor in actor_instances:
                    if actor.getId() == "player":
                        continue
                    else:
                        actor_instance = actor
                        break

                if actor_instance:
                    obj = self._gamecontroller.scene.objectlist[actor_instance.getId()]
                    if obj.type == GameObjectTypes["QUESTGIVER"]:
                        action = TalkAction(self._gamecontroller.scene.player, obj)
                        self._gamecontroller.scene.player.nextaction = action
                    elif obj.type == GameObjectTypes["ENEMY"]:
                        action = AttackAction(self._gamecontroller.scene.player, obj)
                        self._gamecontroller.scene.player.nextaction = action
                    else:
                        action = TalkAction(self._gamecontroller.scene.player, obj)
                        self._gamecontroller.scene.player.nextaction = action

            if item_instances:
                obj = self._gamecontroller.scene.objectlist[item_instances[0].getId()]
                if obj.type == GameObjectTypes["ITEM"]:
                    action = PickUpItemAction(self._gamecontroller.scene.player, obj)
                    self._gamecontroller.scene.player.nextaction = action
                elif obj.type == GameObjectTypes["PORTAL"]:
                    action = EnterPortalAction(self._gamecontroller.scene.player, obj)
                    self._gamecontroller.scene.player.nextaction = action

        if event.getButton() == fife.MouseEvent.RIGHT:
            instances = self._gamecontroller.scene.getInstancesAt(
                clickpoint, self._gamecontroller.scene.actorlayer
            )
            if instances:
                self._gamecontroller.logger.log_debug(
                    "Selected instance on actor layer: " + instances[0].getId()
                )

    def mouseReleased(self, event):
        """Handle mouse release events (no-op by default)."""
        pass

    def mouseMoved(self, event):
        """Handle mouse move events and update instance outlines."""
        renderer = self._gamecontroller.instancerenderer
        if renderer:
            renderer.removeAllOutlines()
        else:
            return

        pt = fife.Point3D(event.getX(), event.getY(), 0)
        actor_instances = self._gamecontroller.scene.getInstancesAt(
            pt, self._gamecontroller.scene.actorlayer
        )
        item_instances = self._gamecontroller.scene.getInstancesAt(
            pt, self._gamecontroller.scene.itemlayer
        )
        for i in actor_instances:
            if i.getId() != "player":
                renderer.addOutlined(i, 173, 255, 47, 2)

        for j in item_instances:
            renderer.addOutlined(j, 255, 173, 47, 2)

    def mouseEntered(self, event):
        """Handle mouse entered events (no-op)."""
        pass

    def mouseExited(self, event):
        """Handle mouse exited events (no-op)."""
        pass

    def mouseClicked(self, event):
        """Handle mouse click events (no-op by default)."""
        pass

    def mouseWheelMovedUp(self, event):
        """Handle mouse wheel up events (no-op)."""
        pass

    def mouseWheelMovedDown(self, event):
        """Handle mouse wheel down events (no-op)."""
        pass

    def mouseDragged(self, event):
        """Handle mouse drag events by moving the player if dragged enough."""
        if event.isConsumedByWidgets():
            return

        clickpoint = fife.Point3D(event.getX(), event.getY(), 0)
        if event.getButton() == fife.MouseEvent.LEFT:
            if (
                clickpoint.x > self._lastmousepos[0] + 5
                or clickpoint.x < self._lastmousepos[0] - 5
                or clickpoint.y > self._lastmousepos[1] + 5
                or clickpoint.y < self._lastmousepos[1] - 5
            ):
                self._gamecontroller.scene.player.walk(
                    self._gamecontroller.scene.getLocationAt(clickpoint)
                )

            self._lastmousepos = (clickpoint.x, clickpoint.y)

    def keyPressed(self, event):
        """Handle key pressed events and update key state bookkeeping."""
        keyval = event.getKey().getValue()
        if keyval == fife.Key.ESCAPE:
            self.detach()
            self._gamecontroller.guicontroller.showMainMenu()
            event.consume()

        self._gamecontroller.keystate.updateKey(keyval, True)

    def keyReleased(self, event):
        """Handle key released events and update key state bookkeeping."""
        keyval = event.getKey().getValue()

        self._gamecontroller.keystate.updateKey(keyval, False)


class GameController:
    """
    The main game class.

    This handles all game related code including setting up the scene, displaying user
    interfaces, managing sound, etc etc.
    """

    def __init__(self, application, engine, settings):
        self._application = application
        self._engine = engine
        self._settings = settings

        self._soundmanager = engine.getSoundManager()
        self._soundmanager.init()

        self._keystate = KeyState()

        self._guicontroller = GUIController(self)

        self._listener = GameListener(self)

        self._guicontroller.showMainMenu()

        self._questmanager = QuestManager(self)

        self._scene = None
        self._instancerenderer = None
        self._floatingtextrenderer = None

        self._switchmaprequested = False
        self._newmap = None

    def onConsoleCommand(self, command):
        """Parse game related console commands.

        Returns
        -------
        str
            A result string describing the outcome or error of the command.
        """
        result = ""

        args = command.split(" ")
        cmd = []
        for arg in args:
            arg = arg.strip()
            if arg != "":
                cmd.append(arg)

        if cmd[0] == "spawn":
            result = "Usage: spawn [object template] [posx] [posy] "
            if len(cmd) != 4:
                return result
            else:
                try:
                    id = str(uuid.uuid1())
                    valdict = {"posx": float(cmd[2]), "posy": float(cmd[3])}
                    obj = self._scene.loadObject(cmd[1], id, valdict)
                except ObjectNotFoundError:
                    result = (
                        "Error: Cannot load [" + cmd[1] + "].  It could not be found!"
                    )
                    obj = None

                if obj:
                    try:
                        self._scene.addObjectToScene(obj)
                        obj.position = (float(cmd[2]), float(cmd[3]))
                        result = "--OK--"
                    except ObjectAlreadyInSceneError:
                        result = "Error: [" + cmd[1] + "] is already on the scene."

        elif cmd[0] == "move":
            result = "Usage: move [object id] [posx] [posy]"
            if len(cmd) != 4:
                return result
            else:
                obj = self._scene.getObject(cmd[1])
                if obj:
                    obj.position = (float(cmd[2]), float(cmd[3]))
                    result = "--OK--"
                else:
                    result = "Error: [" + cmd[1] + "] does not exist on the scene."

        elif cmd[0] == "loadmap":
            result = "Usage: loadmap [map name]"

        return result

    def newGame(self):
        """Remove any save games and start a new game."""
        self._guicontroller.hideMainMenu()

        for filename in glob.glob(os.path.join("saves", "*.xml")):
            os.remove(filename)

        self._questmanager.reset()
        self._questmanager.deserialize()

        mapname = self._settings.get("RPG", "TownMapFile", "town")
        self.loadMap(mapname)

    def loadMap(self, mapname):
        """Create the scene for the map and attach the listener."""
        if self._listener:
            self._listener.detach()

        self._keystate.reset()

        if self._scene:
            self._scene.destroyScene()
            self._scene = None

        self._scene = Scene(self)

        self._scene.createScene(mapname)

        self._instancerenderer = fife.InstanceRenderer.getInstance(
            self._scene.cameras[self._settings.get("RPG", "DefaultCameraName", "camera1")]
        )
        self._floatingtextrenderer = fife.FloatingTextRenderer.getInstance(
            self._scene.cameras[self._settings.get("RPG", "DefaultCameraName", "camera1")]
        )
        self._floatingtextrenderer.addActiveLayer(self._scene.actorlayer)
        font = get_manager().getDefaultFont()
        self._floatingtextrenderer.setFont(font)
        self._floatingtextrenderer.setBackground(255, 100, 100, 165)
        self._floatingtextrenderer.setBorder(255, 50, 50)
        self._floatingtextrenderer.setEnabled(True)

        if self._listener:
            self._listener.attach()

    def switchMap(self, newmapname):
        """Queue a map switch for the next frame to allow current events to finish."""
        self._switchmaprequested = True
        self._newmap = newmapname

        # save before switching scenes
        self._scene.serialize()

    def endGame(self):
        """Save the game state and destroy the scene."""
        if self._scene:
            self._scene.serialize()

            self._listener.detach()
            self._scene.destroyScene()
            self._questmanager.reset()

            self._scene = None
            self._instancerenderer = None
            self._floatingtextrenderer = None

    def quit(self):
        """Quit the game and request the application to exit."""
        self.endGame()
        self._application.requestQuit()

    def pump(self):
        """Process pending map switch requests and update the scene."""
        if self._switchmaprequested:
            self.loadMap(self._newmap)
            self._newmap = None
            self._switchmaprequested = False

        if self._scene:
            self._scene.updateScene()

    def _getGUIController(self):
        return self._guicontroller

    def _getQuestManager(self):
        return self._questmanager

    def _getEngine(self):
        return self._engine

    def _getSettings(self):
        return self._settings

    def _getScene(self):
        return self._scene

    def _getKeyState(self):
        return self._keystate

    def _getInstanceRenderer(self):
        return self._instancerenderer

    def _getLogManager(self):
        return self._application.logger

    guicontroller = property(_getGUIController)
    questmanager = property(_getQuestManager)
    engine = property(_getEngine)
    settings = property(_getSettings)
    scene = property(_getScene)
    keystate = property(_getKeyState)
    instancerenderer = property(_getInstanceRenderer)
    logger = property(_getLogManager)
