# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Common utility exceptions used by the rio_de_hola demo.

This module defines a small set of exception types used by demo
components when encountering programming or initialization errors.
"""


class ProgrammingError(Exception):
    """Exception raised for programming errors in demo code."""


class InitializationError(Exception):
    """Exception raised when demo initialization fails."""
