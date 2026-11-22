# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Tests for the synchronous dialog execution feature (`Widget.execute`).

These tests run against the real pychan stack: a real FIFE engine
(headless) and a real pychan ``Manager``. No mocks are used.

The ``mainLoop`` and ``breakFromMainLoop`` functions passed to
``pychan.setupModalExecution`` are real functions implementing the contract
``Widget.execute`` relies on: ``main_loop`` dispatches the real callbacks
that ``Widget.execute`` captured on the dialog's children (the ones stored
under the ``"__execute__"`` group, as an engine event would trigger them) and
returns the value that ``break_from_main_loop`` stored.
"""

import io
import os
from pathlib import Path

import pytest
from fife.extensions import pychan
from fife.extensions.pychan.internal import Manager
from fife.extensions.pychan.widgets.containers import VBox
from fife.extensions.pychan.widgets.label import Label

from fife import fife

FONT_PATH = Path(__file__).resolve().parents[3] / "data" / "FreeMono.ttf"

#: Font family id registered by the manifest in `_font_manifest` below.
#: `Manager.getFont("default")` resolves through the engine's default font
#: once this family exists, so widgets can be created headlessly.
DEFAULT_FONT_ID = "SystemDefault"


def _font_manifest():
    """Build a font manifest registering one family backed by a real font file.

    Returns:
        An XML font manifest string registering ``DEFAULT_FONT_ID``.
    """
    return f"""<?xml version="1.0"?>
<fonts version="1">
  <family id="{DEFAULT_FONT_ID}">
    <face type="truetype" weight="400" size="12">{FONT_PATH}</face>
  </family>
</fonts>"""


def _make_dialog(child_names=("okButton",)):
    """Create a real root widget with real children of the given names.

    Returns:
        A ``VBox`` root widget with one labeled child per given name.
    """
    dialog = VBox(name="dialog")
    for name in child_names:
        dialog.addChild(Label(name=name, parent=dialog))
    return dialog


def _captured_callbacks(dialog):
    """The real callbacks `Widget.execute` captured on the dialog's children.

    These are the `_quitThisDialog` closures stored by the real
    `Widget.capture` calls under the "__execute__" group.

    Returns:
        A dict of the captured callbacks, keyed by child name.
    """
    callbacks = {}
    for widget in dialog.findChildren():
        captured = widget.event_mapper.callbacks.get("__execute__", {})
        for name in captured:
            callbacks[widget.name] = captured[name]
    return callbacks


def _invoke_execute_callback(dialog, child_name):
    """Invoke the real callback `Widget.execute` captured on the named child."""
    child = dialog.findChildByName(child_name)
    child.event_mapper.callbacks["__execute__"]["action"]()


def _install_main_loop(dialog, trigger_name=None, focused_child=None):
    """Install a real mainLoop/breakFromMainLoop pair via `setupModalExecution`.

    ``main_loop`` simulates the running game loop: it dispatches the real
    captured callbacks (like an engine event would) and returns the value
    ``break_from_main_loop`` stored. If ``trigger_name`` is given, only that
    child's callback is dispatched, simulating the user pressing that widget.

    ``state`` records values observed while the dialog is running. Note that
    ``Widget.is_focusable`` is derived from visibility in fifechan, so focus
    assertions read ``state`` during the run rather than after it.

    Returns:
        The mutable ``state`` dict recording values observed during the run.
    """
    state = {
        "break_value": None,
        "visible_during": None,
        "dialog_is_focusable_during": None,
    }

    def break_from_main_loop(value):
        state["break_value"] = value

    def main_loop():
        state["visible_during"] = dialog.isVisible()
        state["dialog_is_focusable_during"] = dialog.is_focusable
        if focused_child is not None:
            child = dialog.findChildByName(focused_child)
            state["focused_child_is_focusable_during"] = child.is_focusable
        if trigger_name is None:
            for callback in _captured_callbacks(dialog).values():
                callback()
        else:
            _invoke_execute_callback(dialog, trigger_name)
        return state["break_value"]

    pychan.setupModalExecution(main_loop, break_from_main_loop)
    return state


@pytest.fixture(scope="module")
def engine():
    """A real FIFE engine, running headless (dummy SDL drivers).

    Yields:
        An initialized ``fife.Engine``, destroyed when the module finishes.
    """
    os.environ.setdefault("SDL_VIDEODRIVER", "dummy")
    os.environ.setdefault("SDL_AUDIODRIVER", "dummy")
    os.environ.setdefault("ALSOFT_DRIVERS", "null")
    e = fife.Engine()
    e.init()
    yield e
    e.destroy()


@pytest.fixture(scope="module")
def pychan_manager(engine):
    """A real pychan manager on the engine, with a default font family registered.

    Yields:
        The initialized global pychan ``Manager``.
    """
    if not FONT_PATH.is_file():
        pytest.skip(f"{FONT_PATH} not found; cannot run Widget.execute tests")
    engine.loadFontManifestFromString(_font_manifest())
    pychan.init(engine)
    manager = pychan.manager
    yield manager
    # Leave the global state as found for other test modules.
    Manager.manager = None
    pychan.manager = None


@pytest.fixture
def dialog(pychan_manager):
    """A real root widget with a real child, named 'okButton'.

    Returns:
        A fresh ``VBox`` root widget for a single test.
    """
    return _make_dialog()


class TestExecuteHappyPath:
    def test_returns_bound_value(self, pychan_manager, dialog):
        state = _install_main_loop(dialog)
        result = dialog.execute({"okButton": "Yes!"})
        assert result == "Yes!"
        assert state["break_value"] == "Yes!"

    def test_returns_value_from_main_loop(self, pychan_manager, dialog):
        # `Widget.execute` returns `mainLoop()`'s result verbatim.
        state = _install_main_loop(dialog)
        assert dialog.execute({"okButton": "Yes!"}) == state["break_value"]

    def test_widget_is_visible_while_running(self, pychan_manager, dialog):
        state = _install_main_loop(dialog)
        dialog.execute({"okButton": "Yes!"})
        assert state["visible_during"] is True

    def test_widget_hidden_after_execution(self, pychan_manager, dialog):
        _install_main_loop(dialog)
        dialog.execute({"okButton": "Yes!"})
        assert dialog.isVisible() is False
        assert dialog.isSetVisible() is False

    def test_widget_removed_from_manager(self, pychan_manager, dialog):
        _install_main_loop(dialog)
        dialog.execute({"okButton": "Yes!"})
        assert dialog not in pychan_manager.allWidgets

    def test_child_action_event_captured(self, pychan_manager, dialog):
        _install_main_loop(dialog)
        dialog.execute({"okButton": "Yes!"})
        ok = dialog.findChildByName("okButton")
        assert ok.event_mapper.isCaptured("action", "__execute__")

    def test_multiple_binds_capture_each_child(self, pychan_manager):
        dialog = _make_dialog(("okButton", "cancelButton"))
        _install_main_loop(dialog, trigger_name="okButton")
        result = dialog.execute({"okButton": True, "cancelButton": False})
        ok = dialog.findChildByName("okButton")
        cancel = dialog.findChildByName("cancelButton")
        assert ok.event_mapper.isCaptured("action", "__execute__")
        assert cancel.event_mapper.isCaptured("action", "__execute__")
        assert result is True

    def test_sequential_executes(self, pychan_manager, dialog):
        # Each execution shows the dialog again and breaks with its own value.
        _install_main_loop(dialog)
        assert dialog.execute({"okButton": "first"}) == "first"
        assert dialog.isVisible() is False
        assert dialog.execute({"okButton": "second"}) == "second"
        assert dialog.isVisible() is False

    def test_execute_via_loadXML(self, pychan_manager):
        # The documented usage: pychan.loadXML(...).execute({...}).
        dialog = pychan.loadXML(io.StringIO("<VBox><Label name='okButton' /></VBox>"))
        state = _install_main_loop(dialog)
        assert dialog.execute({"okButton": "Yes!"}) == "Yes!"
        assert state["break_value"] == "Yes!"


class TestExecuteFocus:
    def test_focus_child_gets_requested(self, pychan_manager, dialog):
        ok = dialog.findChildByName("okButton")
        ok.is_focusable = False
        state = _install_main_loop(dialog, focused_child="okButton")
        dialog.execute({"okButton": "Yes!"}, focus="okButton")
        # `execute` set the child focusable; assert during the run because
        # fifechan's `isFocusable()` also requires visibility and the callback
        # hides the dialog afterwards.
        assert state["focused_child_is_focusable_during"] is True
        assert state["visible_during"] is True

    def test_missing_focus_name_falls_back_to_root(self, pychan_manager, dialog):
        state = _install_main_loop(dialog)
        dialog.execute({"okButton": "Yes!"}, focus="noSuchChild")
        # The root gets focus when the focus name is missing; again assert
        # during the run (the callback hides the dialog).
        assert state["dialog_is_focusable_during"] is True
        assert state["visible_during"] is True

    def test_empty_bind_focuses_root(self, pychan_manager, dialog):
        state = _install_main_loop(dialog)
        dialog.execute({})
        assert state["dialog_is_focusable_during"] is True
        assert state["visible_during"] is True


class TestExecuteEdgeCases:
    def test_raises_when_modal_execution_not_set_up(self, pychan_manager, dialog):
        manager = pychan_manager
        previous = manager.can_execute
        manager.can_execute = False
        try:
            with pytest.raises(RuntimeError, match="Synchronous execution is not set up"):
                dialog.execute({"okButton": "Yes!"})
        finally:
            manager.can_execute = previous

    def test_raises_for_non_root_widget(self, pychan_manager, dialog):
        ok = dialog.findChildByName("okButton")
        assert ok.parent is dialog
        with pytest.raises(RuntimeError, match="You can only 'execute' root widgets"):
            ok.execute({"okButton": "Yes!"})

    def test_missing_child_name_is_an_attribute_error(self, pychan_manager, dialog):
        # Current behavior of `Widget.execute`: findChild() returns None for
        # unknown names and `None.capture(...)` raises AttributeError. This
        # pins the behavior the note on Widget.execute refers to (API may
        # change).
        _install_main_loop(dialog)
        with pytest.raises(AttributeError):
            dialog.execute({"missingButton": "Yes!"})

    def test_empty_bind_returns_main_loop_result_and_stays_visible(
        self, pychan_manager, dialog
    ):
        _install_main_loop(dialog)
        result = dialog.execute({})
        # main_loop returns the unset break value: no event was dispatched.
        assert result is None
        # Nothing hides the dialog for an empty bind.
        assert dialog.isVisible() is True
        dialog.hide()
