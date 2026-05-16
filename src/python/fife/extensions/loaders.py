# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Loaders plugin manager."""


def loadMapFile(path, engine, callback=None, debug=True, extensions=None):
    """Load map file using the C++ MapLoader.

    Parameters
    ----------
    engine : object
        FIFE engine instance.
    callback : callable, optional
        Ignored. C++ MapLoader does not support callback progress.
    debug : bool, optional
        Flag to activate/deactivate print statements.

    Returns
    -------
    object
        FIFE map object.
    """
    from fife import fife

    model = engine.getModel()
    vfs = engine.getVFS()
    img_mgr = engine.getImageManager()
    render_backend = engine.getRenderBackend()

    loader = fife.MapLoader(model, vfs, img_mgr, render_backend)
    map_obj = loader.load(path)

    if debug:
        print("--- Loading map took using C++ MapLoader.")
    return map_obj
