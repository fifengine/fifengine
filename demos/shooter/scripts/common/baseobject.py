# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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
from scripts.common.helpers import normalize
from scripts.common.helpers import Rect

class SpaceObject(object):
	def __init__(self, model, name, layer, findInstance=True):
		self._model = model
		self._layer = layer
		self._name = name
		self._xscale = self._layer.getCellGrid().getXScale()
		self._yscale = self._layer.getCellGrid().getYScale()		
		self._velocity = fife.DoublePoint(0,0)
		self._maxvelocity = 1
		self._timedelta = 0
		self._boundingBox = Rect(0,0,0,0)
		self._running = False
			
		if findInstance:
			self._instance = self._layer.getInstance(self._name)
		else:
			self._instnace = None
		
	def start(self):
		if self._instance:
			self._running = True

	def update(self, timedelta):
		if self._running:
			self._timedelta = timedelta
		
			shiploc = self.location
			exactloc = shiploc.getExactLayerCoordinates()
		
			exactloc.x += self._velocity.x
			exactloc.y += self._velocity.y
		
			self._boundingBox.x = exactloc.x - self._boundingBox.w/2
			self._boundingBox.y = exactloc.y - self._boundingBox.h/2
				
			shiploc.setExactLayerCoordinates(exactloc)
			self.location = shiploc
	
	def stop(self):
		self._running = False
		
	def destroy(self):
		self._running = False
		self._layer.deleteInstance(self._instance)
		
	def applyThrust(self, vector, timedelta):
		self._velocity.x += (vector.x * (timedelta/1000.0))/self._xscale
		self._velocity.y += (vector.y * (timedelta/1000.0))/self._yscale
		
		if self._velocity.length() > self._maxvelocity:
			norm = normalize(self._velocity)
			self._velocity.x = norm.x * self._maxvelocity
			self._velocity.y = norm.y * self._maxvelocity
		
	
	def applyBrake(self, brakingForce, timedelta):

		if self._velocity.length() <= .001:
			self._velocity.x = 0
			self._velocity.y = 0			
			return
		
		#first normalize to get a unit vector of the direction we are traveling
		norm = normalize(self._velocity)
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
		
		self._velocity.x += (norm.x * (timedelta/1000.0))/self._xscale
		self._velocity.y += (norm.y * (timedelta/1000.0))/self._yscale

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

	width = property(_getW, _setW)
	height = property(_getH, _setH)
	boundingbox = property(_getBoundingBox)	
	location = property(_getLocation,_setLocation)
	instance = property(_getInstance, _setInstance)
	velocity = property(_getVelocity, _setVelocity)
	maxvelocity = property(_getMaxVelocity, _setMaxVelocity)
	running = property(_isRunning)