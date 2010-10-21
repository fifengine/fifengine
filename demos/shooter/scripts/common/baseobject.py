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

from fife import fife
from fife.fife import FloatRect as Rect


SHTR_DEFAULT = 0
SHTR_PLAYER = 1
SHTR_LASTBOSS = 2
SHTR_PROJECTILE = 3
SHTR_ENEMYSHIP = 4
SHTR_POWERUP = 5


class SpaceObject(object):
	"""
	Space Object is the base class for all game objects.
	"""
	def __init__(self, scene, name, findInstance=True):
		"""
		@param scene: A reference to the Scene
		@type scene: L{Scene}
		@param name: The name of the space object
		@type name: C{string}
		@param findInstance: True if the instance you are looking for is already loaded
		                     False if you want to load the instance yourself
		@type findInstance: C{boolean}
		
		"""
		self._scene = scene
		self._model = self._scene.model
		self._layer = self._scene.objectlayer
		self._name = name
		self._xscale = self._layer.getCellGrid().getXScale()
		self._yscale = self._layer.getCellGrid().getYScale()		
		self._velocity = fife.DoublePoint(0,0)
		self._maxvelocity = 1.25
		self._boundingBox = Rect(0,0,0,0)
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
		"""
		You must execute this function for the object to be updated
		"""
		if self._instance:
			self._running = True

	def update(self):
		"""
		If the object is running this updates the FIFE instance location based on
		the objects current velocity and time since last frame
		"""
		if self._running:
			shiploc = self.location
			exactloc = shiploc.getExactLayerCoordinates()
		
			exactloc.x += self._velocity.x * (self._scene.timedelta/1000.0)/self._xscale
			exactloc.y += self._velocity.y * (self._scene.timedelta/1000.0)/self._yscale
		
			self._boundingBox.x = (exactloc.x * self._xscale - self._boundingBox.w/2)
			self._boundingBox.y = (exactloc.y * self._yscale - self._boundingBox.h/2)
				
			shiploc.setExactLayerCoordinates(exactloc)
			
			if shiploc == self.location:
				self._changePosition = False
			else:
				self._changedPosition = True
			
			self.location = shiploc
	
	def stop(self):
		"""
		Stops the object from being updated.
		"""
		self._running = False
		
	def destroy(self):
		"""
		You are meant to override this function to specify what happens when the object
		gets destroyed
		"""
		self._running = False
		
	def applyThrust(self, vector):
		"""
		Applies a thrust vector to the object.  
		
		@note: Objects do not have mass and therefore no inertia.
		
		@param vector A vector specifying the direction and intensity of thrust.
		@type vector: L{fife.DoublePoint}
		"""
		self._velocity.x += (vector.x * (self._scene.timedelta/1000.0))/self._xscale
		self._velocity.y += (vector.y * (self._scene.timedelta/1000.0))/self._yscale
		
		if self._velocity.length() > self._maxvelocity:
			norm = fife.DoublePoint(self._velocity)
			norm.normalize()
			self._velocity.x = norm.x * self._maxvelocity
			self._velocity.y = norm.y * self._maxvelocity
		
	
	def applyBrake(self, brakingForce):
		"""
		Applies a braking thrust in the opposite direction of the current velocity
		
		@param brakingForce: a floating point value specifying how fast the object should decelerate
		@type brakingForce: C{float}
		"""
		if self._velocity.length() <= .01:
			self._velocity.x = 0
			self._velocity.y = 0			
			return
		
		#first normalize to get a unit vector of the direction we are traveling
		norm = fife.DoublePoint(self._velocity)
		norm.normalize()
		
		if norm.length() == 0:
			self._velocity.x = 0
			self._velocity.y = 0
			return
			
		#negate to get opposite direction
		norm.x = norm.x * -1
		norm.y = norm.y * -1
		
		#apply braking deceleration  
		norm.x *= brakingForce
		norm.y *= brakingForce
		
		self._velocity.x += (norm.x * (self._scene.timedelta/1000.0))/self._xscale
		self._velocity.y += (norm.y * (self._scene.timedelta/1000.0))/self._yscale
		
	def removeFromScene(self):
		"""
		Queues this object to be removed from the scene.  The scene will remove the object
		next time the garbage collection routines are called.
		"""
		self._scene.queueObjectForRemoval(self)

	def _isRunning(self):
		return self._running

	def _getMaxVelocity(self):
		return self._maxvelocity
		
	def _setMaxVelocity(self, maxvel):
		self._maxvelocity = maxvel/sqrt(self._xscale * self._yscale)

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
	location = property(_getLocation,_setLocation)
	instance = property(_getInstance, _setInstance)
	velocity = property(_getVelocity, _setVelocity)
	maxvelocity = property(_getMaxVelocity, _setMaxVelocity)
	running = property(_isRunning)
	changedposition = property(_changedPosition)
	scenenodeid = property(_getNodeId, _setNodeId)
