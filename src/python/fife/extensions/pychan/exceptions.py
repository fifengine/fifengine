# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

class PyChanException(Exception):
    """
    Base exception class for PyChan.
    All exceptions raised by PyChan derive from this.
    """

    pass


class InitializationError(PyChanException):
    """
    Exception raised during the initialization.
    """

    pass


class RuntimeError(PyChanException):
    """
    Exception raised during the run time - for example caused by a missing name attribute in a XML file.
    """

    pass


class GuiXMLError(PyChanException):
    """
    An error that occured during parsing an XML file.
    """


class ParserError(PyChanException):
    """
    An error that occured during parsing an attribute.
    """


class PrivateFunctionalityError(RuntimeError):
    """
    Exception raised if private attributes/functions are used.
    """


class StopTreeWalking(StopIteration):
    """
    Internal exception used to abort iteration over the widget tree.
    """


class DeprecatedException(PyChanException):
    """
    Exception raised if a deprecated function or format is used.
    """