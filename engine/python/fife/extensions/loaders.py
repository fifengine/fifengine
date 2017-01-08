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

""" Loaders plugin manager """

import os.path

from fife import fife
from fife.extensions.serializers.xmlmap import XMLMapLoader

mapFileMapping = { 'xml' : XMLMapLoader}
fileExtensions = set(['xml'])

def loadMapFile(path, engine, callback=None, debug=True, extensions={}):
	""" load map file and get (an optional) callback if major stuff is done:
	
		- map creation
		- parsed imports
		- parsed layers 
		- parsed cameras
		
		the callback will send both a string and a float (which shows
		the overall process), callback(string, float)
	
	@type	engine:		object
	@param	engine: 	FIFE engine instance
	@type	callback:	function
	@param	callback:	callback for maploading progress
	@type	debug:		bool
	@param	debug:		flag to activate / deactivate print statements
	@rtype	object
	@return	FIFE map object
	"""
	(filename, extension) = os.path.splitext(path)
	map_loader = mapFileMapping[extension[1:]](engine, callback, debug, extensions)
	map = map_loader.loadResource(path)
	if debug: print "--- Loading map took: ", map_loader.time_to_load, " seconds."
	return map

def addMapLoader(fileExtension, loaderClass):
	"""Add a new loader for fileextension
	@type   fileExtension: string
	@param  fileExtension: The file extension the loader is registered for
	@type   loaderClass:   object
	@param  loaderClass:   A fife.ResourceLoader implementation that loads maps
	                       from files with the given fileExtension
	"""
	mapFileMapping[fileExtension] = loaderClass
	_updateMapFileExtensions()

def _updateMapFileExtensions():
	global fileExtensions
	fileExtensions = set(mapFileMapping.keys())
