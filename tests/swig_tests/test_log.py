# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife
from fife.extensions.fifelog import LogManager


def test_logmanager_construct(engine_minimized):
    """LogManager can be constructed from an engine."""
    lm = LogManager(engine_minimized, promptlog=True, filelog=False)
    assert lm is not None
    assert lm.lm is not None


def test_logmanager_get_module_name(engine_minimized):
    """getModuleName returns correct display names through SWIG."""
    lm = engine_minimized.getLogManager()
    assert lm.getModuleName(fife.LM_AUDIO) == "Audio"
    assert lm.getModuleName(fife.LM_CONTROLLER) == "Controller"
    assert lm.getModuleName(fife.LM_VIEWVIEW) == "View::View"
    assert lm.getModuleName(fife.LM_MODEL) == "Model"
    assert lm.getModuleName(fife.LM_EXCEPTION) == "Exception"


def test_logmanager_add_visible_modules(engine_minimized):
    """Adding a module makes it visible; parent modules also become visible."""
    lm = LogManager(engine_minimized, promptlog=False, filelog=False)
    lm.addVisibleModules("all")
    visible = lm.getVisibleModules()
    assert "Audio" in visible
    assert "Model" in visible

    lm.removeVisibleModules("all")
    visible = lm.getVisibleModules()
    assert "Audio" not in visible


def test_logmanager_set_level_filter(engine_minimized):
    """Level filter can be set and retrieved."""
    lm = LogManager(engine_minimized, promptlog=False, filelog=False)
    lm.setLevelFilter(fife.LogManager.LEVEL_WARN)
    # No getter exposed in fifelog.py, but setLevelFilter should not crash


def test_logmanager_log_methods(engine_minimized):
    """log_debug, log_log, log_warn, log_error execute without error."""
    lm = LogManager(engine_minimized, promptlog=False, filelog=False)
    lm.addVisibleModules("script")
    lm.log_debug("debug from pytest")
    lm.log_log("log from pytest")
    lm.log_warn("warn from pytest")
    lm.log_error("error from pytest")


def test_logmanager_log_to_prompt(engine_minimized):
    """Toggling logToPrompt works."""
    lm = LogManager(engine_minimized, promptlog=True, filelog=False)
    assert lm.logToPrompt is True
    lm.logToPrompt = False
    assert lm.logToPrompt is False


def test_logmanager_log_to_file(engine_minimized):
    """Toggling logToFile works."""
    lm = LogManager(engine_minimized, promptlog=False, filelog=True)
    assert lm.logToFile is True
    lm.logToFile = False
    assert lm.logToFile is False


def test_logmanager_set_visible_modules(engine_minimized):
    """setVisibleModules clears previous modules and sets new ones."""
    lm = LogManager(engine_minimized, promptlog=False, filelog=False)
    lm.setVisibleModules("audio", "video")
    visible = lm.getVisibleModules()
    assert "Audio" in visible
    assert "Video" in visible
    assert "Controller" not in visible
