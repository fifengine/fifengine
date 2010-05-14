# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from math import sqrt

from fife import fife
from scripts.common.baseobject import *
from scripts.weapons import Weapon


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
		if action.getId() == 'flash':
			if self._ship._flashing and self._ship._flashnumber > 0:
				self._ship.instance.act('flash', self._ship.instance.getFacingLocation())
				self._ship._flashnumber -= 1
			else:
				self._ship._flashing = False
				self._ship._flashnumber = 0

		if action.getId() == 'explode' and not self._ship.type == SHTR_PLAYER:
			self._ship.removeFromScene()

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

		self._hitclip = self._scene.soundmanager.createSoundEmitter("sounds/hit.ogg")
		self._explodclip = self._scene.soundmanager.createSoundEmitter("sounds/explode.ogg")

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
			self._instance.act('flash', self._instance.getFacingLocation())
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
			self._hitclip.play()

	def destroy(self):
		"""
		Plays the explode animation (or action)
		"""
		if self._running:
			self._instance.act('explode', self._instance.getFacingLocation())
			location = self.location.getExactLayerCoordinates()
			self._explodclip.position = (location.x, location.y)
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
