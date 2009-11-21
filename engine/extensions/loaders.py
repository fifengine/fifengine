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

# Loader interface for FIFE's native xml format.

import fife

from serializers.xmlmap import XMLMapLoader
from serializers import WrongFileType, NameClash, logger

from serializers.xmlobject import XMLObjectLoader

fileExtensions = ('xml',)

def loadMapFile(path, engine, callback=None):
	""" load map file and get (an optional) callback if major stuff is done:
	- map creation
	- parsed imports
	- parsed layers 
	- parsed cameras
	the callback will send both a string and a float (which shows
	the overall process), callback(string, float)
		
	@return	map	: map object
	"""
	map_loader = XMLMapLoader(engine, callback)
	map = map_loader.loadResource(fife.ResourceLocation(path))
	logger.log(fife.LogManager.LEVEL_LOG, "--- Loading map took: " + str(map_loader.time_to_load) + " seconds.")
	return map


def loadImportFile(path, engine):
	object_loader = XMLObjectLoader(engine.getImagePool(), engine.getAnimationPool(), engine.getModel(), engine.getVFS())
	res = None
	try:
		res = object_loader.loadResource(fife.ResourceLocation(path))
		logger.log(fife.LogManager.LEVEL_LOG, 'imported object file ' + path)
	except WrongFileType:
		logger.log(fife.LogManager.LEVEL_LOG, 'ignored non-object file ' + path)
	except NameClash:
		logger.log(fife.LogManager.LEVEL_LOG, 'ignored already loaded file ' + path)
	
	return res

def loadImportDir(path, engine):
	for file in filter(lambda f: f.split('.')[-1] == 'xml', engine.getVFS().listFiles(path)):
		loadImportFile('/'.join([path, file]), engine)

def loadImportDirRec(path, engine):
	loadImportDir(path, engine)

	for dir in filter(lambda d: not d.startswith('.'), engine.getVFS().listDirectories(path)):
		loadImportDirRec('/'.join([path, dir]), engine)
