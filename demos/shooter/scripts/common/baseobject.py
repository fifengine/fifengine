#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Base classes and utilities for shooter demo space objects."""

from math import sqrt

from fife import fife
from fife.fife import FloatRect as Rect


def old_div(a, b):
    """Divide a by b, returning 0 when b is zero.

    Returns
    -------
    float
        The result of dividing ``a`` by ``b`` or ``0`` if ``b`` is zero.
    """
    return a / b if b != 0 else 0


SHTR_DEFAULT = 0
SHTR_PLAYER = 1
SHTR_LASTBOSS = 2
SHTR_PROJECTILE = 3
SHTR_ENEMYSHIP = 4
SHTR_POWERUP = 5


class SpaceObject:
    """Base class for all space game objects."""

    def __init__(self, scene, name, findInstance=True):
        """Initialize the space object."""
        self._scene = scene
        self._model = self._scene.model
        self._layer = self._scene.objectlayer
        self._name = name
        self._xscale = self._layer.getCellGrid().getXScale()
        self._yscale = self._layer.getCellGrid().getYScale()
        self._velocity = fife.DoublePoint(0, 0)
        self._maxvelocity = 1.25
        self._boundingBox = Rect(0, 0, 0, 0)
        self._running = False
        self._changedPosition = False
        self._scenenodeid = -1
        self._type = SHTR_DEFAULT

        if findInstance:
            self._instance = self._layer.getInstance(self._name)
            self._instance.thisown = 0
        else:
            self._instance = None

    def start(self):
        """Start updating the object."""
        if self._instance:
            self._running = True

    def update(self):
        """Update the FIFE instance location based on velocity and elapsed time."""
        if self._running:
            shiploc = self.location
            exactloc = shiploc.getExactLayerCoordinates()

            exactloc.x += (
                self._velocity.x * (old_div(self._scene.timedelta, 1000.0)) / self._xscale
            )
            exactloc.y += (
                self._velocity.y * (old_div(self._scene.timedelta, 1000.0)) / self._yscale
            )

            self._boundingBox.x = exactloc.x * self._xscale - old_div(
                self._boundingBox.w, 2
            )
            self._boundingBox.y = exactloc.y * self._yscale - old_div(
                self._boundingBox.h, 2
            )

            shiploc.setExactLayerCoordinates(exactloc)

            if shiploc == self.location:
                self._changePosition = False
            else:
                self._changedPosition = True

            self.location = shiploc

    def stop(self):
        """Stop updating the object."""
        self._running = False

    def destroy(self):
        """Destroy the object (override to implement cleanup)."""
        self._running = False

    def applyThrust(self, vector):
        """Apply a thrust vector to the object."""
        self._velocity.x += old_div(
            (vector.x * (old_div(self._scene.timedelta, 1000.0))), self._xscale
        )
        self._velocity.y += old_div(
            (vector.y * (old_div(self._scene.timedelta, 1000.0))), self._yscale
        )

        if self._velocity.length() > self._maxvelocity:
            norm = fife.DoublePoint(self._velocity)
            norm.normalize()
            self._velocity.x = norm.x * self._maxvelocity
            self._velocity.y = norm.y * self._maxvelocity

    def applyBrake(self, brakingForce):
        """Apply braking force opposite to the current velocity."""
        if self._velocity.length() <= 0.01:
            self._velocity.x = 0
            self._velocity.y = 0
            return

        # first normalize to get a unit vector of the direction we are traveling
        norm = fife.DoublePoint(self._velocity)
        norm.normalize()

        if norm.length() == 0:
            self._velocity.x = 0
            self._velocity.y = 0
            return

        # negate to get opposite direction
        norm.x = norm.x * -1
        norm.y = norm.y * -1

        # apply braking deceleration
        norm.x *= brakingForce
        norm.y *= brakingForce

        self._velocity.x += old_div(
            (norm.x * (old_div(self._scene.timedelta, 1000.0))), self._xscale
        )
        self._velocity.y += old_div(
            (norm.y * (old_div(self._scene.timedelta, 1000.0))), self._yscale
        )

    def removeFromScene(self):
        """Queue this object for removal from the scene."""
        self._scene.queueObjectForRemoval(self)

    def _isRunning(self):
        return self._running

    def _getMaxVelocity(self):
        return self._maxvelocity

    def _setMaxVelocity(self, maxvel):
        self._maxvelocity = old_div(maxvel, sqrt(self._xscale * self._yscale))

    def _getLocation(self):
        return self._instance.getLocation()

    def _setLocation(self, loc):
        self._instance.setLocation(loc)

    def _getInstance(self):
        return self._instance

    def _setInstance(self, instance):
        self._instance = instance
        if self._instance:
            self._instance.thisown = 0

    def _getVelocity(self):
        return self._velocity

    def _setVelocity(self, velocity):
        self._velocity = velocity

    def _getBoundingBox(self):
        return self._boundingBox

    def _getW(self):
        return self._boundingBox.w

    def _getH(self):
        return self._boundingBox.h

    def _setW(self, w):
        self._boundingBox.w = w

    def _setH(self, h):
        self._boundingBox.h = h

    def _changedPosition(self):
        return self._changedPosition

    def _getNodeId(self):
        return self._scenenodeid

    def _setNodeId(self, id):
        self._scenenodeid = id

    def _getType(self):
        return self._type

    def _setType(self, objtype):
        self._type = objtype

    type = property(_getType, _setType)
    width = property(_getW, _setW)
    height = property(_getH, _setH)
    boundingbox = property(_getBoundingBox)
    location = property(_getLocation, _setLocation)
    instance = property(_getInstance, _setInstance)
    velocity = property(_getVelocity, _setVelocity)
    maxvelocity = property(_getMaxVelocity, _setMaxVelocity)
    running = property(_isRunning)
    changedposition = property(_changedPosition)
    scenenodeid = property(_getNodeId, _setNodeId)
