#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Base ship objects and listeners for the shooter demo."""

from fife import fife

from scripts.common.baseobject import SHTR_PLAYER, SpaceObject


class ShipActionListener(fife.InstanceActionListener):
    """Action listener for ship instance actions.

    Used to respond to completed instance actions such as flash and explode.
    """

    def __init__(self, ship):
        """Initialize the action listener for `ship`."""
        fife.InstanceActionListener.__init__(self)

        self._ship = ship
        self._ship.instance.addActionListener(self)

    def onInstanceActionFinished(self, instance, action):
        """Handle a finished action for the instance."""
        if action.getId() == "flash":
            if self._ship._flashing and self._ship._flashnumber > 0:
                self._ship.instance.actOnce(
                    "flash", self._ship.instance.getFacingLocation()
                )
                self._ship._flashnumber -= 1
            else:
                self._ship._flashing = False
                self._ship._flashnumber = 0

        if action.getId() == "explode" and not self._ship.type == SHTR_PLAYER:
            self._ship.removeFromScene()

    def onInstanceActionCancelled(self, instance, action):
        """Handle a cancelled action (no-op)."""
        pass


class Ship(SpaceObject):
    """Base ship object.

    The Player and Enemy ship classes inherit from this class. It extends
    `SpaceObject` with weapon support and hit point management.
    """

    def __init__(self, scene, name, findInstance=True):
        """Initialize the ship.

        Parameters
        ----------
        scene : Scene
            Scene containing the ship.
        name : str
            Name of the ship object.
        findInstance : bool, optional
            If True, look up an existing instance; otherwise create one.
        """
        super().__init__(scene, name, findInstance)

        self._weapon = None

        self._flashnumber = 0
        self._flashing = False

        self._hitpoints = 0
        self._scorevalue = 0

        self._hitclip = self._scene.soundmanager.createEmitter("sounds/hit.ogg")
        scene.soundmanager.addEmitterToSoundEffect(scene._soundeffect, self._hitclip)
        self._explodclip = self._scene.soundmanager.createEmitter("sounds/explode.ogg")
        scene.soundmanager.addEmitterToSoundEffect(scene._soundeffect, self._explodclip)

    def _setWeapon(self, weapon):
        self._weapon = weapon

    def _getWeapon(self):
        return self._weapon

    def flash(self, number):
        """Play the flash animation the specified number of times."""
        if self._running:
            self._instance.actOnce("flash", self._instance.getFacingLocation())
            self._flashnumber = number
            self._flashing = True

    def fire(self, direction):
        """Fire the current weapon in the specified direction.

        Parameters
        ----------
        direction : fife.DoublePoint
            The direction to fire the weapon.

        Returns
        -------
        Any
            The result returned by the weapon's `fire` method, or `None`.
        """
        if self._weapon and self._hitpoints > 0:
            return self._weapon.fire(direction)

        return None

    def applyHit(self, hp):
        """Remove the specified number of hit points and destroy if needed."""
        self._hitpoints -= hp
        if self._hitpoints <= 0:
            self.destroy()
        else:
            location = self.location.getExactLayerCoordinates()
            self._hitclip.setPosition(location)
            self._hitclip.play()

    def destroy(self):
        """Play the explode animation and associated effects."""
        if self._running:
            self._instance.actOnce("explode", self._instance.getFacingLocation())
            location = self.location.getExactLayerCoordinates()
            self._explodclip.setPosition(location)
            self._explodclip.play()
            super().destroy()

    def _getHitPoints(self):
        return self._hitpoints

    def _setHitPoints(self, hp):
        self._hitpoints = hp

    def _getScoreValue(self):
        return self._scorevalue

    def _setScoreValue(self, value):
        self._scorevalue = value

    weapon = property(_getWeapon, _setWeapon)
    hitpoints = property(_getHitPoints, _setHitPoints)
    scorevalue = property(_getScoreValue, _setScoreValue)
