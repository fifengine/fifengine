# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Utilities and helpers for serializers used by FIFE."""

import os

import fife

__all__ = (
    "ET",
    "SerializerError",
    "InvalidFormat",
    "WrongFileType",
    "NameClash",
    "NotFound",
    "warn",
    "root_subfile",
    "reverse_root_subfile",
)

try:
    import xml.etree.ElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET  # type: ignore[no-redef]


class SerializerError(Exception):
    """Base exception for serializer errors."""


class InvalidFormat(SerializerError):
    """Raised when a serializer encounters invalid format."""


class WrongFileType(SerializerError):
    """Raised when a file has an unexpected type for the serializer."""


class NameClash(SerializerError):
    """Raised when a name collision occurs while loading data."""


class NotFound(SerializerError):
    """Raised when a requested resource cannot be found."""


# Remove duplicate redefinitions that were accidentally introduced above.


def warn(self, msg):
    """Emit a serializer warning message.

    Parameters
    ----------
    msg : str
        Warning message to display.
    """
    print(f"Warning ({self.filename}): {msg}")


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
        Path with '/' separators.
    """
    if os.path.sep == "/":
        return path

    return "/".join(path.split(os.path.sep))


def loadImportFile(loader, path, engine, debug=False):
    """Use XMLObjectLoader to load an import file from a path.

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
    loader.loadResource(fife.ResourceLocation(path))
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
        Engine instance used to list directories and files.
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
    Return new path for given subfile rooted against masterfile.

    E.g. if masterfile is ./../foo/bar.xml and subfile is ./../foo2/subfoo.xml,
    returned path is ../foo2/subfoo.xml
    NOTE: masterfile is expected to be *file*, not directory. subfile can be either.

    Returns
    -------
    str
        Path to the subfile relative to the masterfile directory.
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
