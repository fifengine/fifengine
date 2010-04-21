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
import math


def normalize(vector):
	"""
	Helper function to normalize a vector
	"""
	norm = fife.DoublePoint(0,0) 
		
	invLength = 1.0/vector.length()
	if invLength > 1e-06:
		norm.x = vector.x * invLength;
		norm.y = vector.y * invLength;
	else:
		norm.x = 0
		norm.y = 0
	
	return norm
	
class Rect(object):
	def __init__(self, x = 0, y = 0, w = 0, h = 0):
		self._x = x
		self._y = y
		self._w = w
		self._h = h
		
	def intersects(self, rect):
		_x = self._x - rect.x;
		_y = self._y - rect.y;
		_w = self._w;
		_h = self._h;

		if _x < 0:
			_w += _x
			_x = 0

		if _y < 0:
			_h += _y
			_y = 0

		if _x + _w > rect.w:
			_w = rect.w - _x

		if _y + _h > rect.h:
			_h = rect.h - _y
			
		if _w <= 0 or _h <= 0:
			return False

		return True

	def _setX(self, x):
		self._x = x
		
	def _getX(self):
		return self._x

	def _setY(self, y):
		self._y = y
		
	def _getY(self):
		return self._y
		
	def _setW(self, w):
		self._w = w
		
	def _getW(self):
		return self._w
		
	def _setH(self, h):
		self._h = h
		
	def _getH(self):
		return self._h
		
	x = property(_getX, _setX)
	y = property(_getY, _setY)
	w = property(_getW, _setW)
	h = property(_getH, _setH)
	
def rotatePoint(origin, point, angle):
	newp = fife.DoublePoint(0,0)
	
	theta = (angle * math.pi)/180
	
	costheta = math.cos(theta)
	sintheta = math.sin(theta)
	
	x = point.x - origin.x
	y = point.y - origin.y
	
	newp.x = costheta * x - sintheta * y
	newp.y = sintheta * x + costheta * y
	
	return newp
	