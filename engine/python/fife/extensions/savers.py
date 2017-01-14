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

""" Savers plugin manager  """

import os.path

from fife import fife
from fife.extensions.serializers.xmlmapsaver import XMLMapSaver

mapFileMapping = { 'xml' : XMLMapSaver}
fileExtensions = ('xml',)

def saveMapFile(path, engine, map, importList=[], debug=True):
	""" save map file
	@type	path:		string
	@param	path:		The fully qualified path to the file to save
	@type	engine:		object
	@param	engine: 	FIFE engine instance
	@type	map:		object
	@param	map:		FIFE map object
	@type	importList:	list
	@param	importList:	A list of all imports
	@type 	debug:		boolean
	@param	debug:		Enables debugging information
	"""
	(filename, extension) = os.path.splitext(path)
	map.setFilename(path)
	map_saver = mapFileMapping[extension[1:]](path, engine, map, importList)
		
	map_saver.saveResource()
	if debug: print "--- Saved Map."
	return map

def addMapSaver(fileExtension, saverClass):
	"""Add a new saver for fileextension
	@type   fileExtension: string
	@param  fileExtension: The file extension the saver is registered for
	@type   saverClass:   object
	@param  saverClass:   A fife.ResourceLoader implementation that saves maps
	                       from files with the given fileExtension
	"""
	mapFileMapping[fileExtension] = saverClass
	_updateMapFileExtensions()

def _updateMapFileExtensions():
	global fileExtensions
	fileExtensions = set(mapFileMapping.keys())
