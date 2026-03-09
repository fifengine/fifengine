#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import print_function

import os
import sys
import unittest

if not hasattr(unittest.TestCase, "assert_"):
    unittest.TestCase.assert_ = unittest.TestCase.assertTrue

fife_path = os.path.join("..", "..", "engine", "python")
if os.path.isdir(fife_path) and fife_path not in sys.path:
    sys.path.insert(0, fife_path)

from fife import fife  # noqa: E402

print("Using the FIFE python module found here: ", os.path.dirname(fife.__file__))

if not hasattr(fife, "ModelCoordinate") and hasattr(fife, "Point3D"):
    fife.ModelCoordinate = fife.Point3D

if not hasattr(fife, "ExactModelCoordinate") and hasattr(fife, "DoublePoint3D"):
    fife.ExactModelCoordinate = fife.DoublePoint3D

from fife.extensions import fifelog  # noqa: E402


def getEngine(minimized=False):
    e = fife.Engine()
    try:
        log = fifelog.LogManager(e, promptlog=False, filelog=True)
        if hasattr(log, "setVisibleModules"):
            log.setVisibleModules("all")
    except Exception:
        pass
    s = e.getSettings()
    if hasattr(s, "setRenderBackend"):
        s.setRenderBackend("SDL")
    if hasattr(s, "setDefaultFontPath"):
        s.setDefaultFontPath("../data/FreeMono.ttf")
    if hasattr(s, "setDefaultFontGlyphs"):
        s.setDefaultFontGlyphs(
            " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
            + ".,!?-+/:();%`'*#=[]"
        )
    if minimized:
        if hasattr(s, "setScreenWidth"):
            s.setScreenWidth(1)
        if hasattr(s, "setScreenHeight"):
            s.setScreenHeight(1)
    if hasattr(s, "setDefaultFontSize"):
        s.setDefaultFontSize(12)
    e.init()
    return e


__all__ = []
__all__.append("unittest")
__all__.append("fife")
__all__.append("fifelog")
__all__.append("getEngine")
