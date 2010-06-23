#!/usr/bin/env python

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

class RPGDemoException(Exception):
	def __init__(self, msg=None):
		if msg:
			self._msg = msg
		else:
			self._msg = None

class InvalidCommandError(RPGDemoException):
	def __init__(self, msg=None):
		super(InvalidCommandError, self).__init__(msg)
	
	def __str__(self):
		if self._msg:
			return repr(self._msg)
		else:
			return repr("Command not found!")
		
class ObjectNotFoundError(RPGDemoException):
	def __init__(self, msg=None):
		super(ObjectNotFoundError, self).__init__(msg)
		
	def __str__(self):
		if self._msg:
			return repr(self._msg)
		else:
			return repr("Object was not found!")
		
class ObjectAlreadyInSceneError(RPGDemoException):
	def __init__(self, msg=None):
		super(ObjectAlreadyInSceneError, self).__init__(msg)
		
	def __str__(self):
		if self._msg:
			return repr(self._msg)
		else:
			return repr("Object was already part of the scene!")

class InstanceNotFoundError(RPGDemoException):
	def __init__(self, msg=None):
		super(InstanceNotFoundError, self).__init__(msg)
		
	def __str__(self):
		if self._msg:
			return repr(self._msg)
		else:
			return repr("Instance was not found on layer!")
