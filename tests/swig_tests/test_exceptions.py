# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest

from fife import fife


def _assert_printable_exception(exc):
    try:
        text = str(exc)
    except Exception as render_error:
        pytest.fail(f"str({type(exc).__name__}) raised unexpectedly: {render_error!r}")
    assert isinstance(text, str)
    assert text != ""
    return text


def test_constructed_sdl_exception_str_is_printable():
    exc = fife.SDLException("synthetic sdl error")
    text = _assert_printable_exception(exc)
    assert "synthetic sdl error" in text


@pytest.mark.parametrize(
    "exception_cls",
    [
        fife.SDLException,
        fife.NotFound,
        fife.NotSet,
        fife.IndexOverflow,
        fife.InvalidFormat,
        fife.CannotOpenFile,
        fife.InvalidConversion,
        fife.NotSupported,
        fife.NameClash,
        fife.Duplicate,
        fife.ScriptException,
        fife.EventException,
        fife.GuiException,
        fife.InconsistencyDetected,
        fife.OutOfMemory,
    ],
)
def test_all_wrapped_exceptions_have_safe_printable_str(exception_cls):
    payload = f"synthetic message for {exception_cls.__name__}"
    exc = exception_cls(payload)

    text = _assert_printable_exception(exc)
    message = exc.getMessage()

    assert isinstance(message, str)
    assert message != ""
    assert payload in text
