# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest
from fife.extensions.pychan.exceptions import (
    DeprecatedException,
    GuiXMLError,
    InitializationError,
    ParserError,
    PrivateFunctionalityError,
    PyChanException,
    RuntimeError,
    StopTreeWalking,
)


class TestPychanExceptions:
    def test_pychan_exception_base(self):
        with pytest.raises(PyChanException):
            raise PyChanException("test")

    def test_initialization_error(self):
        with pytest.raises(InitializationError):
            raise InitializationError("test")

    def test_runtime_error(self):
        with pytest.raises(RuntimeError):
            raise RuntimeError("test")

    def test_xml_error(self):
        with pytest.raises(GuiXMLError):
            raise GuiXMLError("test")

    def test_parser_error(self):
        with pytest.raises(ParserError):
            raise ParserError("test")

    def test_private_functionality_error(self):
        with pytest.raises(PrivateFunctionalityError):
            raise PrivateFunctionalityError("test")

    def test_stop_tree_walking(self):
        with pytest.raises(StopTreeWalking):
            raise StopTreeWalking()

    def test_deprecated_exception(self):
        with pytest.raises(DeprecatedException):
            raise DeprecatedException("test")

    def test_exception_hierarchy(self):
        assert issubclass(InitializationError, PyChanException)
        assert issubclass(RuntimeError, PyChanException)
        assert issubclass(GuiXMLError, PyChanException)
        assert issubclass(ParserError, PyChanException)
        assert issubclass(PrivateFunctionalityError, RuntimeError)
        assert issubclass(DeprecatedException, PyChanException)
        assert issubclass(StopTreeWalking, StopIteration)
