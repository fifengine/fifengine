# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import os
import sys

import pytest

fife_path = os.path.join("..", "..", "src", "python")
if os.path.isdir(fife_path) and fife_path not in sys.path:
    sys.path.insert(0, fife_path)

from fife import fife as fife_internal

fife = fife_internal

if not hasattr(fife, "ModelCoordinate") and hasattr(fife, "Point3D"):
    fife.ModelCoordinate = fife.Point3D

if not hasattr(fife, "ExactModelCoordinate") and hasattr(fife, "DoublePoint3D"):
    fife.ExactModelCoordinate = fife.DoublePoint3D

from fife.extensions import fifelog


def _env_truthy(value):
    if value is None:
        return False
    return str(value).strip().lower() in ("1", "true", "yes", "on")


def _env_int(value, default=0):
    try:
        return int(value)
    except (TypeError, ValueError):
        return default


def _create_engine(minimized=False, test_name=None):
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
    if hasattr(s, "setFullScreen"):
        s.setFullScreen(_env_truthy(os.environ.get("FIFE_TEST_FULLSCREEN")))
    if hasattr(s, "setDisplay"):
        s.setDisplay(_env_int(os.environ.get("FIFE_TEST_DISPLAY"), 0))
    if hasattr(s, "setDefaultFontPath"):
        s.setDefaultFontPath("../data/FreeMono.ttf")
    if hasattr(s, "setDefaultFontGlyphs"):
        s.setDefaultFontGlyphs(
            " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
            + ".,!?-+/:();%`'*#=[]"
        )
    version = fife.get_version()
    title = (
        f"FIFE {version} - SWIG Test: {test_name}"
        if test_name
        else f"FIFE {version} - SWIG Test"
    )
    if hasattr(s, "setWindowTitle"):
        s.setWindowTitle(title)
    if minimized:
        if hasattr(s, "setScreenWidth"):
            s.setScreenWidth(1)
        if hasattr(s, "setScreenHeight"):
            s.setScreenHeight(1)
    else:
        if hasattr(s, "setScreenWidth"):
            s.setScreenWidth(1024)
        if hasattr(s, "setScreenHeight"):
            s.setScreenHeight(768)
    if hasattr(s, "setDefaultFontSize"):
        s.setDefaultFontSize(12)
    e.init()
    return e


@pytest.fixture
def engine(request):
    e = _create_engine(minimized=False, test_name=request.node.name)
    return e


@pytest.fixture
def engine_minimized(request):
    e = _create_engine(minimized=True, test_name=request.node.name)
    return e
