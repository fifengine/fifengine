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

"""
FIFE Backwards Combatibility Layer
==================================

This module can be imported if you want to
run code that wasn't adapted to API changes in FIFE.

2008.1
------

 - Animation.addFrame now expects a fife.ResourcePtr instead of an fife.Image
 - Pool.getIndex is just an alias for Pool.addResourceFromFile.
 - EventManager.setNonConsumableKeys is superseeded by EventManager.setKeyFilter

"""

from fife import fife

# Utility functions

def deprecated(revision,message):
	print "fife_compat: Deprecation warning - See revision %d " % revision
	print " - ",message

def this_is_deprecated(func,revision=0,message=None):
	if message is None:
		message = repr(func) + " is deprecated."
	def wrapped_func(*args,**kwargs):
		deprecated(revision,message)
		return func(*args,**kwargs)
	return wrapped_func

def _compat_NonConsumableKeys():
	class CompatKeyFilter(fife.IKeyFilter):
		def __init__(self, keys):
			fife.IKeyFilter.__init__(self)
			self.keys = keys

		def isFiltered(self, event):
			return event.getKey().getValue() in self.keys

	def _setNonConsumableKeys(self,keys):
		deprecated(2636, "Write an IKeyFilter instead of using EventManager.setNonConsumableKeys.\n" +
				 "You probably don't need it anyway")
		self.compat_keyfilter = CompatKeyFilter(keys)
		self.compat_keyfilter.__disown__()
		self.setKeyFilter(self.compat_keyfilter)

	def _getNonConsumableKeys(self,keys):
		deprecated(2636, "Write an IKeyFilter instead of using EventManager.getNonConsumableKeys.")
		return self.compat_keyfilter.keys

	fife.EventManager.setNonConsumableKeys = _setNonConsumableKeys
	fife.EventManager.getNonConsumableKeys = _getNonConsumableKeys

_compat_NonConsumableKeys()
