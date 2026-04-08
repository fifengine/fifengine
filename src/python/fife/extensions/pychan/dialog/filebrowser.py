# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""a filebrowser implementation for pychan."""

import os
import sys

import fife.extensions.pychan as pychan
import fife.extensions.pychan.widgets as widgets
from fife.fife import Engine


class FileBrowser:
    """The B{FileBrowser} displays directory and file listings.

    B{The fileSelected} parameter is a callback invoked when a file selection has been made; its
    signature must be fileSelected(path,filename).

    B{If selectdir} is set, fileSelected's filename parameter should be optional.

    B{The savefile} option provides a box for supplying a new filename that doesn't exist yet.

    B{The selectdir} option allows directories to be selected as well as files.
    """

    def __init__(self, *args, **kwargs):
        """Temporary constructor for deprecation."""
        # TODO: Remove the engine argument
        display_deprecation = False
        if isinstance(args[0], Engine):
            display_deprecation = True
            args = args[1:]
        if "engine" in kwargs:
            display_deprecation = True
            del kwargs["engine"]
        if display_deprecation:
            print("DEPRECATION WARNING:")
            print("The filebrowser no longer needs an engine instance")
            print("It will be completely removed in the future")

        self.__real_init__(*args, **kwargs)

    def __real_init__(
        self,
        fileSelected,
        savefile=False,
        selectdir=False,
        extensions=("xml",),
        guixmlpath="gui/filebrowser.xml",
    ):
        """
        Initialize the FileBrowser dialog.

        Parameters
        ----------
        fileSelected : callable
            Callback invoked on file selection. Signature must be fileSelected(path, filename).
        savefile : bool, optional
            Flag to provide a GUI for a user-given filename.
        selectdir : bool, optional
            Flag to allow selecting directories; fileSelected will be called without filename.
        extensions : tuple, optional
            List of extensions the filebrowser should show (defaults to ("xml",)).
        guixmlpath : str, optional
            Path to the XML GUI file (defaults to "gui/filebrowser.xml").
        """
        self.fileSelected = fileSelected

        self._widget = None
        self.savefile = savefile
        self.selectdir = selectdir

        self.guixmlpath = guixmlpath

        self.extensions = extensions
        self.path = "./.."
        self.dir_list = []
        self.file_list = []

    def showBrowser(self):
        """Create and / or show the gui."""
        if self._widget:
            self.setDirectory(self.path)
            self._widget.show()
            return

        self._widget = pychan.loadXML(self.guixmlpath)
        self._widget.mapEvents(
            {
                "dirList": self._selectDir,
                "selectButton": self._selectFile,
                "closeButton": self._widget.hide,
            }
        )
        if self.savefile:
            self._file_entry = widgets.TextField(name="saveField", text="")
            self._widget.findChild(name="fileColumn").addChild(self._file_entry)

        self.setDirectory(self.path)
        self._widget.show()

    def setDirectory(self, path):
        """Set the current directory according to path."""
        path_copy = self.path
        self.path = path
        if not self._widget:
            return

        def decodeList(list):
            fs_encoding = sys.getfilesystemencoding()
            if fs_encoding is None:
                fs_encoding = "ascii"

            newList = []
            for i in list:
                try:
                    newList.append(str(i, fs_encoding))
                except (TypeError, UnicodeDecodeError, ValueError):
                    newList.append(str(i, fs_encoding, "replace"))
                    print("WARNING: Could not decode item:", i)
            return newList

        dir_list_copy = list(self.dir_list)
        file_list_copy = list(self.file_list)

        self.dir_list = []
        self.file_list = []

        contents = os.listdir(path)
        for content in contents:
            if os.path.isdir(os.path.join(path, content)):
                self.dir_list.append(content)
            elif os.path.isfile(os.path.join(path, content)):
                extension = os.path.splitext(content)[1][1:]
                if extension in self.extensions:
                    self.file_list.append(content)

        try:
            self.dir_list = sorted(decodeList(self.dir_list), key=lambda s: s.lower())
            self.dir_list.insert(0, "..")
            self.file_list = sorted(decodeList(self.file_list), key=lambda s: s.lower())
        except (OSError, TypeError, UnicodeError, ValueError):
            self.path = path_copy
            self.dir_list = list(dir_list_copy)
            self.file_list = list(file_list_copy)
            print("WARNING: Tried to browse to directory that is not accessible!")

        self._widget.distributeInitialData(
            {"dirList": self.dir_list, "fileList": self.file_list}
        )

        self._widget.adaptLayout()

    def _selectDir(self):
        """Handle directory selection from the ListBox."""
        selection = self._widget.collectData("dirList")
        if selection >= 0 and selection < len(self.dir_list):
            new_dir = self.dir_list[selection]
            lst = self.path.split("/")
            if new_dir == ".." and lst[-1] != ".." and lst[-1] != ".":
                lst.pop()
            else:
                lst.append(new_dir)

            path = "/".join(lst)
            self.setDirectory(path)

    def _selectFile(self):
        """Handle file selection from the selectButton and hide the filebrowser."""
        self._widget.hide()
        selection = self._widget.collectData("fileList")

        if self.savefile:
            if self._widget.collectData("saveField"):
                self.fileSelected(self.path, self._widget.collectData("saveField"))
                return

        if selection >= 0 and selection < len(self.file_list):
            self.fileSelected(self.path, self.file_list[selection])
            return

        if self.selectdir:
            self.fileSelected(self.path)
            return

        print("FileBrowser: error, no selection.")
