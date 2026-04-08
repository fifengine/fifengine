# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Savers plugin manager."""

import os.path

from fife.extensions.serializers.xmlmapsaver import XMLMapSaver

mapFileMapping = {"xml": XMLMapSaver}
fileExtensions = ("xml",)


def saveMapFile(path, engine, map, importList=[], debug=True):
    """Save a map file.

    Parameters
    ----------
    path : str
        Fully qualified path to the file to save.
    engine : object
        FIFE engine instance.
    map : object
        FIFE map object.
    importList : list, optional
        A list of all imports.
    debug : bool, optional
        Enables debugging information.

    Returns
    -------
    object
        The saved map object.
    """
    filename, extension = os.path.splitext(path)
    map.setFilename(path)
    map_saver = mapFileMapping[extension[1:]](path, engine, map, importList)

    map_saver.saveResource()
    if debug:
        print("--- Saved Map.")
    return map


def addMapSaver(fileExtension, saverClass):
    """Register a new map saver for a file extension.

    Parameters
    ----------
    fileExtension : str
        The file extension the saver is registered for.
    saverClass : type
        A ``fife.ResourceLoader`` implementation that saves maps from files
        with the given extension.
    """
    mapFileMapping[fileExtension] = saverClass
    _updateMapFileExtensions()


def _updateMapFileExtensions():
    global fileExtensions
    fileExtensions = set(mapFileMapping.keys())
