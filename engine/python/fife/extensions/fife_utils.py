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

""" This file contains some functions that may be useful """

import fife, re, sys, os

__all__ = ['is_fife_exc', 'getUserDataDirectory']

_exc_re = re.compile(r'_\[(\w+)\]_')

def is_fife_exc(type, original_exc):
	""" Checks if an exception is of given type.
	Example::
		try:
			obj = self.model.createObject(str(id), str(nspace), parent)
		except RuntimeError, e:
			if is_fife_exc(fife.NameClash, e):
				raise NameClash('Tried to create already existing object, ignoring')
			raise
	"""
	ret = False
	m = _exc_re.search(str(original_exc))
	if m:
		if m.group(1) == type('').getTypeStr():
			ret = True
	return ret

def getUserDataDirectory(vendor, appname):
	""" Gets the proper location to save configuration and data files, depending on depending on OS.
	
	Windows: %APPDATA%\vendor\appname
	Mac: ~/Library/Application Support/vendor/appname
	Linux/Unix/Other: ~/.vendor/appname
	
	See:
	Brian Vanderburg II @ http://mail.python.org/pipermail/python-list/2008-May/660779.html
	"""
	dir = None 

	# WINDOWS
	if os.name == "nt":

		# Try env APPDATA or USERPROFILE or HOMEDRIVE/HOMEPATH
		if "APPDATA" in os.environ:
			dir = os.environ["APPDATA"]

		if ((dir is None) or (not os.path.isdir(dir))) and ("USERPROFILE" in os.environ):
			dir = os.environ["USERPROFILE"]
			if os.path.isdir(os.path.join(dir, "Application Data")):
				dir = os.path.join(dir, "Application Data")

		if ((dir is None) or (not os.path.isdir(dir))) and ("HOMEDRIVE" in os.environ) and ("HOMEPATH" in os.environ):
			dir = os.environ["HOMEDRIVE"] + os.environ["HOMEPATH"]
			if os.path.isdir(os.path.join(dir, "Application Data")):
				dir = os.path.join(dir, "Application Data")

		if (dir is None) or (not os.path.isdir(dir)):
			dir = os.path.expanduser("~")
			
		# On windows, add vendor and app name
		dir = os.path.join(dir, vendor, appname)

	# Mac
	elif os.name == "mac": # ?? may not be entirely correct
		dir = os.path.expanduser("~")
		dir = os.path.join(dir, "Library", "Application Support")
		dir = os.path.join(dir, vendor, appname)

	# Unix/Linux/all others
	if dir is None:
		dir = os.path.expanduser("~")
		dir = os.path.join(dir, "."+vendor, appname)
		
	# Create vendor/appname folder if it doesn't exist
	if not os.path.isdir(dir):
		os.makedirs(dir)
		
	return dir
