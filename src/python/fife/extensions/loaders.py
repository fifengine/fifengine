# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Loaders plugin manager."""

import os.path

from fife.extensions.serializers.xmlmap import XMLMapLoader

mapFileMapping = {"xml": XMLMapLoader}
fileExtensions = {"xml"}


def loadMapFile(path, engine, callback=None, debug=True, extensions={}):
    """Load map file and get (an optional) callback if major stuff is done:

            - map creation
            - parsed imports
            - parsed layers
            - parsed cameras

            the callback will send both a string and a float (which shows
            the overall process), callback(string, float)

    Parameters
    ----------
    engine : object
        FIFE engine instance.
    callback : callable, optional
        Callback for map loading progress; called as callback(string, float).
    debug : bool, optional
        Flag to activate/deactivate print statements.

    Returns
    -------
    object
        FIFE map object.
    """
    filename, extension = os.path.splitext(path)
    map_loader = mapFileMapping[extension[1:]](engine, callback, debug, extensions)
    map = map_loader.loadResource(path)
    if debug:
        print("--- Loading map took: ", map_loader.time_to_load, " seconds.")
    return map


def addMapLoader(fileExtension, loaderClass):
    """Add a new loader for fileextension.

    Parameters
    ----------
    fileExtension : str
        The file extension the loader is registered for.
    loaderClass : type
        A ``fife.ResourceLoader`` implementation that loads maps from files
        with the given file extension.
    """
    mapFileMapping[fileExtension] = loaderClass
    _updateMapFileExtensions()


def _updateMapFileExtensions():
    global fileExtensions
    fileExtensions = set(mapFileMapping.keys())
