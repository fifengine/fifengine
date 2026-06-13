# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Test the key API: module-level constants, Key class aliases, and helpers.

These tests do NOT need an engine instance as the key constants are available
at module import time.
"""

import os
import sys

# Prefer installed wheel (venv), fall back to repo source tree
_venv = os.path.join(
    os.path.dirname(__file__),
    "..",
    "..",
    ".venv-uh",
    "lib",
    f"python{sys.version_info.major}.{sys.version_info.minor}",
    "site-packages",
)
if os.path.isdir(_venv) and _venv not in sys.path:
    sys.path.insert(0, _venv)
_src = os.path.join(os.path.dirname(__file__), "..", "..", "src", "python")
if os.path.isdir(_src) and _src not in sys.path:
    sys.path.append(_src)

from fife import fife  # noqa: E402

# -- Sanity: module-level keys exist ------------------------------------------

MODULE_KEYS = [
    "KEY_UNKNOWN",
    "RETURN",
    "ESCAPE",
    "BACKSPACE",
    "TAB",
    "SPACE",
    "F1",
    "F5",
    "F12",
    "F13",
    "F16",
    "F24",
    "PRINTSCREEN",
    "DELETE",
    "HOME",
    "END",
    "UP",
    "DOWN",
    "LEFT",
    "RIGHT",
    "INSERT",
    "PAGEUP",
    "PAGEDOWN",
    "CAPSLOCK",
    "NUMLOCKCLEAR",
    "SCROLLLOCK",
    "LCTRL",
    "LSHIFT",
    "LALT",
    "LGUI",
    "RCTRL",
    "RSHIFT",
    "RALT",
    "RGUI",
    "KEY_LESS",
    "KEY_GREATER",
    "KP_0",
    "KP_1",
    "KP_2",
    "KP_3",
    "KP_4",
    "KP_5",
    "KP_6",
    "KP_7",
    "KP_8",
    "KP_9",
    "KP_DIVIDE",
    "KP_MULTIPLY",
    "KP_MINUS",
    "KP_PLUS",
    "KP_ENTER",
    "KP_PERIOD",
    "VOLUMEUP",
    "VOLUMEDOWN",
    "MUTE",
    "MEDIA_PLAY",
    "MEDIA_PAUSE",
    "MEDIA_STOP",
    "MEDIA_NEXT_TRACK",
    "MEDIA_PREVIOUS_TRACK",
    "MEDIA_EJECT",
    "HELP",
    "MENU",
    "APPLICATION",
    "POWER",
    "SLEEP",
    "WAKE",
    "AC_SEARCH",
    "AC_HOME",
    "AC_BACK",
    "AC_FORWARD",
    "AC_STOP",
    "AC_REFRESH",
    "AC_BOOKMARKS",
    "MODE",
    "LEFT_TAB",
    "LEVEL5_SHIFT",
    "MULTI_KEY_COMPOSE",
    "LMETA",
    "RMETA",
    "LHYPER",
    "RHYPER",
    "SOFTLEFT",
    "SOFTRIGHT",
    "CALL",
    "ENDCALL",
]


def test_module_keys_exist():
    missing = [k for k in MODULE_KEYS if not hasattr(fife, k)]
    assert not missing, f"Missing module-level key constants: {missing}"


def test_module_key_values_unique():
    seen = {}
    for name in MODULE_KEYS:
        val = getattr(fife, name)
        assert (
            val not in seen
        ), f"Key collision: {seen[val]} and {name} both have value {val}"
        seen[val] = name


# -- Key construction and helpers ---------------------------------------------


def test_key_construct():
    k = fife.Key(fife.ESCAPE)
    assert k.getValue() == fife.ESCAPE
    assert k.getValue() == 27


def test_key_get_name():
    k = fife.Key(fife.ESCAPE)
    name = k.get_name()
    assert isinstance(name, str) and len(name) > 0


def test_key_getAsString():
    k = fife.Key(fife.RETURN)
    assert k.getAsString() == k.get_name()


def test_key_is_printable():
    a = fife.Key(fife.A)
    assert a.is_printable()
    assert a.isPrintable()


def test_key_is_function_key():
    assert fife.Key(fife.F1).isFunctionKey()
    assert fife.Key(fife.F24).isFunctionKey()
    assert not fife.Key(fife.A).isFunctionKey()


# -- Static values from SDL3 --------------------------------------------------


def test_known_values():
    assert fife.KEY_UNKNOWN == 0
    assert fife.RETURN == 0x0D
    assert fife.ESCAPE == 27
    assert fife.SPACE == 32
    assert fife.A == 0x61
    assert fife.Z == 0x7A
    assert fife.BACKSPACE == 8
    assert fife.TAB == 9
    assert fife.F1 == 0x4000003A
    assert fife.DELETE == 0x7F
