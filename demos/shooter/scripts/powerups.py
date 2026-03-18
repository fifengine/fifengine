#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife  # noqa: F401

from scripts.common.baseobject import SHTR_POWERUP, SpaceObject
from scripts.weapons import CannonSpread5


class PowerUp(SpaceObject):
    def __init__(self, scene, powerupName, instance, findInstance=True):
        super(PowerUp, self).__init__(scene, powerupName, findInstance)

        self.instance = instance
        self._type = SHTR_POWERUP

        self.width = 0.025
        self.height = 0.025

    def applyPowerUp(self, ship):
        self.destroy()
        self._scene.queueObjectForRemoval(self)


class CannonSpread5PU(PowerUp):
    def __init__(self, scene, powerupName, instance, findInstance=True):
        super(CannonSpread5PU, self).__init__(scene, powerupName, instance, findInstance)

        self._dir = 0
        self._time = 1500
        self._velocity.x = -0.25
        self._velocity.y = 0

        self._pickupclip = self._scene.soundmanager.createEmitter("sounds/pickup.ogg")
        self._pickupclip.setRelativePositioning(True)

    def applyPowerUp(self, ship):
        ship.weapon = CannonSpread5(self._scene, ship, 300)
        self._pickupclip.play()
        self.destroy()
        self._scene.queueObjectForRemoval(self)

    def update(self):
        if self._dir == 1:
            self._velocity.y = -0.25
        elif self._dir == 0:
            self._velocity.y = 0.25

        if self._time >= 3000:
            if self._dir == 1:
                self._dir = 0
            elif self._dir == 0:
                self._dir = 1

            self._time = 0

        self._time += self._scene.timedelta

        super(CannonSpread5PU, self).update()


class ExtraLifePU(PowerUp):
    def __init__(self, scene, powerupName, instance, findInstance=True):
        super(ExtraLifePU, self).__init__(scene, powerupName, instance, findInstance)

        self._pickupclip = self._scene.soundmanager.createEmitter("sounds/pickup.ogg")
        self._pickupclip.setRelativePositioning(True)

    def applyPowerUp(self, ship):
        ship.lives += 1
        self._pickupclip.play()
        self.destroy()
        self._scene.queueObjectForRemoval(self)
