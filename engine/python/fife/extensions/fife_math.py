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

"""
Math Library
==================================

This is a collection of useful 2D math functions/classes.
"""

from fife import fife
import math

def normalize(vector):
	"""
	Helper function to normalize a 2D vector
	
	@param vector: a L{fife.DoublePoint} to be normalized
	@type vector: L{fife.DoublePoint}
	
	@return: A normalized L{fife.DoublePoint}
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
	
def rotatePoint(origin, point, angle):
	"""
	Rotates a point around the specified origin.
	
	@param origin: A point specifying the origin.
	@type origin: L{fife.DoublePoint}
	@param point: The point to be rotated.
	@type point: L{fife.DoublePoint}
	@param angle: The angle in which to rotate the point.
	@type angle: C{int} or C{float}
	
	@return: The rotated point.
	@rtype: L{fife.DoublePoint}
	"""
	newp = fife.DoublePoint(0,0)
	
	theta = (angle * math.pi)/180
	
	costheta = math.cos(theta)
	sintheta = math.sin(theta)
	
	x = point.x - origin.x
	y = point.y - origin.y
	
	newp.x = costheta * x - sintheta * y
	newp.y = sintheta * x + costheta * y
	
	return newp

__all__ = ['normalize','rotatePoint']
