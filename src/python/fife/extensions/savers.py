# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Savers plugin manager"""

from __future__ import print_function

import os.path

from fife.extensions.serializers.xmlmapsaver import XMLMapSaver

mapFileMapping = {"xml": XMLMapSaver}
fileExtensions = ("xml",)


def saveMapFile(path, engine, map, importList=[], debug=True):
    """save map file
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
    filename, extension = os.path.splitext(path)
    map.setFilename(path)
    map_saver = mapFileMapping[extension[1:]](path, engine, map, importList)

    map_saver.saveResource()
    if debug:
        print("--- Saved Map.")
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
