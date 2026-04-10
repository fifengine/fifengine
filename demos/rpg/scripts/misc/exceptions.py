#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


"""Custom exception types used by the RPG demo scripts."""


class RPGDemoException(Exception):
    """Base exception for RPG demo-specific errors."""

    def __init__(self, msg=None):
        if msg:
            self._msg = msg
        else:
            self._msg = None


class InvalidCommandError(RPGDemoException):
    """Raised when a console command is invalid or not found."""

    def __init__(self, msg=None):
        super().__init__(msg)

    def __str__(self):
        """Return a human-readable representation of the error.

        Returns
        -------
        str
            Human-readable representation of the error message.
        """
        if self._msg:
            return repr(self._msg)
        else:
            return repr("Command not found!")


class ObjectNotFoundError(RPGDemoException):
    """Raised when a requested object cannot be located in the scene."""

    def __init__(self, msg=None):
        super().__init__(msg)

    def __str__(self):
        """Return the stored message or a default not-found string.

        Returns
        -------
        str
            The stored message or a default message indicating not found.
        """
        if self._msg:
            return repr(self._msg)
        else:
            return repr("Object was not found!")


class ObjectAlreadyInSceneError(RPGDemoException):
    """Raised when attempting to add an object that's already on the scene."""

    def __init__(self, msg=None):
        super().__init__(msg)

    def __str__(self):
        """Return the stored message or a default already-in-scene string.

        Returns
        -------
        str
            The stored message or a default message indicating already present.
        """
        if self._msg:
            return repr(self._msg)
        else:
            return repr("Object was already part of the scene!")


class InstanceNotFoundError(RPGDemoException):
    """Raised when a FIFE instance cannot be found on the specified layer."""

    def __init__(self, msg=None):
        super().__init__(msg)

    def __str__(self):
        """Return the stored message or a default instance-not-found string.

        Returns
        -------
        str
            The stored message or a default message indicating instance missing.
        """
        if self._msg:
            return repr(self._msg)
        else:
            return repr("Instance was not found on layer!")
