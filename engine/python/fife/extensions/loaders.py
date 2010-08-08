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

from fife import fife

from fife.extensions.serializers.xmlmap import XMLMapLoader
from fife.extensions.serializers import WrongFileType, NameClash

from fife.extensions.serializers.xmlobject import XMLObjectLoader

fileExtensions = ('xml',)

def loadMapFile(path, engine, callback=None, debug=True):
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
	
	@type	map:	object
	@return	map:	FIFE map object
	"""
	map_loader = XMLMapLoader(engine, callback, debug)
	map = map_loader.loadResource(fife.ResourceLocation(path))
	if debug: print "--- Loading map took: ", map_loader.time_to_load, " seconds."
	return map


def loadImportFile(path, engine, debug=False):
	""" uses XMLObjectLoader to load import files from path
	
	@type	path:	string
	@param	path:	path to import file
	@type	engine:	object
	@param	engine:	FIFE engine instance
	@type	debug:	bool
	@param	debug:	flag to activate / deactivate print statements
	"""
	object_loader = XMLObjectLoader(engine.getImagePool(), engine.getAnimationPool(), engine.getModel(), engine.getVFS())
	res = None
	try:
		res = object_loader.loadResource(fife.ResourceLocation(path))
		if debug: print 'imported object file ' + path
	except WrongFileType:
		pass
#		print 'ignored non-object file ' + path
	except NameClash:
		pass
#		print 'ignored already loaded file ' + path
	return res

def loadImportDir(path, engine, debug=False):
	""" helper function to call loadImportFile on a directory
	
	@type	path:	string
	@param	path:	path to import directory
	@type	engine:	object
	@param	engine:	FIFE engine instance
	@type	debug:	bool
	@param	debug:	flag to activate / deactivate print statements
	"""
	for file in filter(lambda f: f.split('.')[-1] == 'xml', engine.getVFS().listFiles(path)):
		loadImportFile('/'.join([path, file]), engine, debug)

def loadImportDirRec(path, engine, debug=False):
	""" helper function to call loadImportFile recursive on a directory

	@type	path:	string
	@param	path:	path to import directory
	@type	engine:	object
	@param	engine:	FIFE engine instance
	@type	debug:	bool
	@param	debug:	flag to activate / deactivate print statements	
	"""
	loadImportDir(path, engine, debug)

	for dir in filter(lambda d: not d.startswith('.'), engine.getVFS().listDirectories(path)):
		loadImportDirRec('/'.join([path, dir]), engine, debug)
