#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class RPGDemoException(Exception):
    def __init__(self, msg=None):
        if msg:
            self._msg = msg
        else:
            self._msg = None


class InvalidCommandError(RPGDemoException):
    def __init__(self, msg=None):
        super().__init__(msg)

    def __str__(self):
        if self._msg:
            return repr(self._msg)
        else:
            return repr("Command not found!")


class ObjectNotFoundError(RPGDemoException):
    def __init__(self, msg=None):
        super().__init__(msg)

    def __str__(self):
        if self._msg:
            return repr(self._msg)
        else:
            return repr("Object was not found!")


class ObjectAlreadyInSceneError(RPGDemoException):
    def __init__(self, msg=None):
        super().__init__(msg)

    def __str__(self):
        if self._msg:
            return repr(self._msg)
        else:
            return repr("Object was already part of the scene!")


class InstanceNotFoundError(RPGDemoException):
    def __init__(self, msg=None):
        super().__init__(msg)

    def __str__(self):
        if self._msg:
            return repr(self._msg)
        else:
            return repr("Instance was not found on layer!")
