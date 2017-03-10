# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
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

class PyChanException(Exception):
	"""
	Base exception class for PyChan.
	All exceptions raised by PyChan derive from this.
	"""
	pass

class InitializationError(PyChanException):
	"""
	Exception raised during the initialization.
	"""
	pass

class RuntimeError(PyChanException):
	"""
	Exception raised during the run time - for example caused by a missing name attribute in a XML file.
	"""
	pass

class GuiXMLError(PyChanException):
	"""
	An error that occured during parsing an XML file.
	"""

class ParserError(PyChanException):
	"""
	An error that occured during parsing an attribute.
	"""

class PrivateFunctionalityError(RuntimeError):
	"""
	Exception raised if private attributes/functions are used.
	"""

class StopTreeWalking(StopIteration):
	"""
	Internal exception used to abort iteration over the widget tree.
	"""
