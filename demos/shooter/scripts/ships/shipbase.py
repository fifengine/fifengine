#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife

from scripts.common.baseobject import SHTR_PLAYER, SpaceObject


class ShipActionListener(fife.InstanceActionListener):
    """
    This class is an action listener that listens for instance actions
    that are complete.  It is used in this demo to specify what should happen
    after the object has completed flashing or exploding.

    There should be an instance of this listener for every ship that you want
    to be removed from the scene after exploding.
    """

    def __init__(self, ship):
        """
        @param ship The ship that this actionlistener belongs to.
        """

        fife.InstanceActionListener.__init__(self)

        self._ship = ship
        self._ship.instance.addActionListener(self)

    def onInstanceActionFinished(self, instance, action):
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
        pass


class Ship(SpaceObject):
    """
    Ship

    This is the base ship object.  The Player and Enemy ship classes inherit
    this class.  This class extends SpaceObject by allowing Weapons to
    be attached to the object as well as hit points.
    """

    def __init__(self, scene, name, findInstance=True):
        """
        @param scene: A reference to the Scene
        @type scene: L{Scene}
        @param name: The name of the ship
        @type name: C{string}
        @param findInstance: True if the instance you are looking for is already loaded
                             False if you want to load the instance yourself

        @type findInstance: C{boolean}

        """

        super(Ship, self).__init__(scene, name, findInstance)

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
        """
        Playes the flash animation (or action) the specified number of times

        @param number: An integer specifying the number of times to play the flash animation
        """
        if self._running:
            self._instance.actOnce("flash", self._instance.getFacingLocation())
            self._flashnumber = number
            self._flashing = True

    def fire(self, direction):
        """
        Fires the current weapon in the specified direction

        @param direction: A L{fife.DoublePoint()} specifying the direction to fire
        """
        if self._weapon and self._hitpoints > 0:
            return self._weapon.fire(direction)

        return None

    def applyHit(self, hp):
        """
        Removes the specified number of hit points.  Destroys the ship if necessary.

        @param hp: The number of hit points to remove from the ship.
        """
        self._hitpoints -= hp
        if self._hitpoints <= 0:
            self.destroy()
        else:
            location = self.location.getExactLayerCoordinates()
            self._hitclip.setPosition(location)
            self._hitclip.play()

    def destroy(self):
        """
        Plays the explode animation (or action)
        """
        if self._running:
            self._instance.actOnce("explode", self._instance.getFacingLocation())
            location = self.location.getExactLayerCoordinates()
            self._explodclip.setPosition(location)
            self._explodclip.play()
            super(Ship, self).destroy()

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
