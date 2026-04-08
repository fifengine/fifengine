#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import os
import sys


def genpath(somepath):
    return os.path.sep.join(somepath.split("/"))


_paths = ("src/swigwrappers/python", "src/extensions")
for p in _paths:
    if p not in sys.path:
        sys.path.append(os.path.sep.join(p.split("/")))

from fife import fife  # noqa: E402
from fife.extensions import fifelog  # noqa: E402


def getEngine(minimized=False):
    e = fife.Engine()
    log = fifelog.LogManager(e, promptlog=False, filelog=True)
    log.setVisibleModules("all")
    s = e.getSettings()
    s.setDefaultFontPath("tests/data/FreeMono.ttf")
    s.setDefaultFontGlyphs(
        " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        + ".,!?-+/:();%`'*#=[]"
    )
    if minimized:
        s.setScreenWidth(1)
        s.setScreenHeight(1)
    s.setDefaultFontSize(12)
    e.init()
    return e


__all__ = []
__all__.append("unittest")
__all__.append("fife")
__all__.append("fifelog")
__all__.append("genpath")
__all__.append("getEngine")
