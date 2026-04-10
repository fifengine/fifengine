# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Utilities for xml maploading process."""

import math
import os


def loadImportFile(loader, path, engine, debug=False):
    """Use XMLObjectLoader to load import files from path.

    Parameters
    ----------
    loader : object
        Loader instance (XMLObjectLoader) to use.
    path : str
        Path to import file.
    engine : object
        Engine instance (unused by this helper but provided for API consistency).
    debug : bool, optional
        Flag to activate/deactivate print statements.
    """
    loader.loadResource(path)
    if debug:
        print("imported object file " + path)


def loadImportDir(loader, path, engine, debug=False):
    """
    Call :func:`loadImportFile` for every XML file in a directory.

    Parameters
    ----------
    loader : object
        Loader instance.
    path : str
        Path to import directory.
    engine : object
        Engine instance used to list files.
    debug : bool, optional
        Flag to activate/deactivate print statements.
    """
    for _file in [
        f for f in engine.getVFS().listFiles(path) if f.split(".")[-1] == "xml"
    ]:
        loadImportFile(loader, "/".join([path, _file]), engine, debug)


def loadImportDirRec(loader, path, engine, debug=False):
    """Recursively call :func:`loadImportFile` for XML files in a directory tree.

    Parameters
    ----------
    loader : object
        Loader instance.
    path : str
        Path to import directory.
    engine : object
        Engine instance used to list files and directories.
    debug : bool, optional
        Flag to activate/deactivate print statements.
    """
    loadImportDir(loader, path, engine, debug)

    for _dir in [
        d for d in engine.getVFS().listDirectories(path) if not d.startswith(".")
    ]:
        loadImportDirRec(loader, "/".join([path, _dir]), engine, debug)


def root_subfile(masterfile, subfile):
    """
    Return new path for given subfile rooted against ``masterfile``.

    Example
    -------
    If ``masterfile`` is ``./../foo/bar.xml`` and ``subfile`` is
    ``./../foo2/subfoo.xml``, the returned path is ``../foo2/subfoo.xml``.

    Returns
    -------
    str
        Path to the subfile relative to the masterfile directory.

    Notes
    -----
    ``masterfile`` is expected to be a file, not a directory. ``subfile`` can
    be either.
    """
    s = "/"

    masterfile = norm_path(os.path.abspath(masterfile))
    subfile = norm_path(os.path.abspath(subfile))

    master_fragments = masterfile.split(s)
    sub_fragments = subfile.split(s)

    master_leftovers = []
    sub_leftovers = []

    for i in range(len(master_fragments)):
        try:
            if master_fragments[i] == sub_fragments[i]:
                master_leftovers = master_fragments[i + 1 :]
                sub_leftovers = sub_fragments[i + 1 :]
        except IndexError:
            break

    pathstr = ""
    for f in master_leftovers[:-1]:
        pathstr += ".." + s
    pathstr += s.join(sub_leftovers)
    return pathstr


def reverse_root_subfile(masterfile, subfile):
    """Do the inverse operation to :func:`root_subfile`.

    Example
    -------
    If ``masterfile`` is ``./../foo/bar.xml`` and ``subfile`` is
    ``../foo2/subfoo.xml``, the returned path is ``./../foo2/subfoo.xml``.

    Returns
    -------
    str
        Path to the reconstructed subfile rooted against the provided masterfile.

    Notes
    -----
    This is usually used to convert saved paths into engine-relative paths.
    ``masterfile`` is expected to be a file, not a directory. ``subfile`` can
    be either.
    """
    s = "/"

    masterfile = norm_path(os.path.abspath(masterfile)).split(s)[:-1]
    subfile = norm_path(os.path.abspath(s.join(masterfile) + s + subfile))
    masterfile = (
        norm_path(os.getcwd()) + s + "foo.bar"
    )  # cheat a little to satisfy root_subfile
    return root_subfile(masterfile, subfile)


def norm_path(path):
    """Make the path use '/' delimited separators.

    FIFE always uses these delimiters, but some OS-related routines will
    default to ``os.path.sep``.

    Returns
    -------
    str
        The normalized path using forward slashes as separators.
    """
    if os.path.sep == "/":
        return path

    return "/".join(path.split(os.path.sep))


def frange(limit1, limit2=None, increment=1.0):
    """Range function that accepts floats (and integers).

    If only one limit is specified, assumes 0 as lower limit.

    Parameters
    ----------
    limit1 : float
        lower range limit
    limit2 : float, optional
        upper range limit
    increment : float, optional
        length of each step

    Returns
    -------
    generator
        iterable over (limit2 - limit1) / increment steps

    Examples
    --------
    >>> frange(-2, 2, 0.1)
    >>> frange(10)
    >>> frange(10, increment=0.5)

    The returned value is an iterator. Use ``list(frange)`` for a list.

    Source: U{http://code.activestate.com/recipes/66472-frange-a-range-function-with-float-increments/}
    """
    if limit2 is None:
        limit2, limit1 = float(limit1), 0.0
    else:
        limit1 = float(limit1)

    count = int(math.ceil((limit2 - limit1) / increment))
    return (limit1 + n * increment for n in range(count))
