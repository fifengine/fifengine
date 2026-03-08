# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Loaders plugin manager"""

from __future__ import print_function

import os.path

from fife.extensions.serializers.xmlmap import XMLMapLoader

mapFileMapping = {"xml": XMLMapLoader}
fileExtensions = set(["xml"])


def loadMapFile(path, engine, callback=None, debug=True, extensions={}):
    """load map file and get (an optional) callback if major stuff is done:

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
    filename, extension = os.path.splitext(path)
    map_loader = mapFileMapping[extension[1:]](engine, callback, debug, extensions)
    map = map_loader.loadResource(path)
    if debug:
        print("--- Loading map took: ", map_loader.time_to_load, " seconds.")
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