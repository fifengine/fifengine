# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Savers plugin manager."""


def saveMapFile(path, engine, map_obj, importList=None, debug=True):
    """Save a map file using the C++ MapSaver.

    Parameters
    ----------
    path : str
        Fully qualified path to the file to save.
    engine : object
        FIFE engine instance.
    map_obj : object
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
    from fife import fife

    model = engine.getModel()
    img_mgr = engine.getImageManager()

    saver = fife.createDefaultMapSaver(model, img_mgr)
    saver.save(map_obj, path, importList or [])

    if debug:
        print("--- Saved Map using C++ MapSaver.")
    return map_obj
