# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import ctypes
import os
import sys

import pytest

from fife import fife

# SDL3 event constants and struct layouts from SDL_events.h.
# SDL_Event is a 128-byte union; only the members used here are modelled.
_SDL_EVENT_MOUSE_WHEEL = 0x403
_SDL_MOUSEWHEEL_NORMAL = 0
_SDL_EVENT_FINGER_DOWN = 0x700
_SDL_EVENT_FINGER_UP = 0x701
_SDL_EVENT_FINGER_MOTION = 0x702
_SDL_EVENT_FINGER_CANCELED = 0x703
_SDL_EVENT_PINCH_BEGIN = 0x710
_SDL_EVENT_PINCH_UPDATE = 0x711
_SDL_EVENT_PINCH_END = 0x712


class _SDLMouseWheelEvent(ctypes.Structure):
    _fields_ = [
        ("type", ctypes.c_uint32),
        ("reserved", ctypes.c_uint32),
        ("timestamp", ctypes.c_uint64),
        ("windowID", ctypes.c_uint32),
        ("which", ctypes.c_uint32),
        ("x", ctypes.c_float),
        ("y", ctypes.c_float),
        ("direction", ctypes.c_uint32),
        ("mouse_x", ctypes.c_float),
        ("mouse_y", ctypes.c_float),
        ("integer_x", ctypes.c_int32),
        ("integer_y", ctypes.c_int32),
    ]


class _SDLTouchFingerEvent(ctypes.Structure):
    _fields_ = [
        ("type", ctypes.c_uint32),
        ("reserved", ctypes.c_uint32),
        ("timestamp", ctypes.c_uint64),
        ("touchID", ctypes.c_uint64),
        ("fingerID", ctypes.c_uint64),
        ("x", ctypes.c_float),
        ("y", ctypes.c_float),
        ("dx", ctypes.c_float),
        ("dy", ctypes.c_float),
        ("pressure", ctypes.c_float),
        ("windowID", ctypes.c_uint32),
    ]


class _SDLPinchFingerEvent(ctypes.Structure):
    _fields_ = [
        ("type", ctypes.c_uint32),
        ("reserved", ctypes.c_uint32),
        ("timestamp", ctypes.c_uint64),
        ("scale", ctypes.c_float),
        ("windowID", ctypes.c_uint32),
    ]


class _SDL_Event(ctypes.Union):
    _fields_ = [
        ("type", ctypes.c_uint32),
        ("wheel", _SDLMouseWheelEvent),
        ("tfinger", _SDLTouchFingerEvent),
        ("pinch", _SDLPinchFingerEvent),
        ("padding", ctypes.c_uint8 * 128),
    ]


def _find_loaded_library(fragment):
    """Return the path of an already-loaded shared library.

    SDL is linked into libfifengine, so its symbols are reachable through
    that library. Only Linux keeps the path in /proc/self/maps.

    Returns
    -------
    str | None
        The resolved library path, or None when it cannot be located.
    """
    if not sys.platform.startswith("linux"):
        return None

    try:
        with open("/proc/self/maps", encoding="ascii", errors="ignore") as maps:
            for line in maps:
                fields = line.split()
                if len(fields) < 6:
                    continue
                path = fields[-1]
                if fragment in os.path.basename(path) and os.path.exists(path):
                    return path
    except OSError:
        return None
    return None


def _push_event(event, description):
    """Push a prepared SDL event into the engine queue.

    Raises
    ------
    RuntimeError
        If SDL rejects the synthetic event.
    """
    library_path = _find_loaded_library("libfifengine")
    if library_path is None:
        pytest.skip("SDL event injection is only wired up on Linux")

    library = ctypes.CDLL(library_path)
    push_event = library.SDL_PushEvent
    push_event.argtypes = [ctypes.POINTER(_SDL_Event)]
    push_event.restype = ctypes.c_bool

    if not push_event(ctypes.byref(event)):
        raise RuntimeError(f"SDL_PushEvent rejected the synthetic {description} event")


def _push_mouse_wheel(x, y, mouse_x=10.0, mouse_y=20.0):
    """Inject a real SDL_EVENT_MOUSE_WHEEL into the engine event queue."""
    event = _SDL_Event()
    event.wheel.type = _SDL_EVENT_MOUSE_WHEEL
    event.wheel.x = x
    event.wheel.y = y
    event.wheel.direction = _SDL_MOUSEWHEEL_NORMAL
    event.wheel.mouse_x = mouse_x
    event.wheel.mouse_y = mouse_y
    _push_event(event, "mouse wheel")


def _push_finger(
    event_type,
    touch_id,
    finger_id,
    x=0.5,
    y=0.5,
    dx=0.0,
    dy=0.0,
    pressure=0.5,
):
    """Inject a real SDL finger event into the engine event queue."""
    event = _SDL_Event()
    event.tfinger.type = event_type
    event.tfinger.touchID = touch_id
    event.tfinger.fingerID = finger_id
    event.tfinger.x = x
    event.tfinger.y = y
    event.tfinger.dx = dx
    event.tfinger.dy = dy
    event.tfinger.pressure = pressure
    _push_event(event, "finger")


def _push_pinch(event_type, scale):
    """Inject a real SDL pinch event into the engine event queue."""
    event = _SDL_Event()
    event.pinch.type = event_type
    event.pinch.scale = scale
    _push_event(event, "pinch")


def test_events(engine_minimized):
    class MyEventListener(fife.ICommandListener):
        def __init__(self):
            fife.ICommandListener.__init__(self)

        def onCommand(self, command):
            pass

    eventmanager = engine_minimized.getEventManager()
    listener = MyEventListener()
    eventmanager.addCommandListener(listener)
    cmd = fife.Command()
    cmd.setCode(0)
    for i in range(20):
        eventmanager.dispatchCommand(cmd)
        cmd.setCode(i)
    eventmanager.removeCommandListener(listener)


def test_mouse_listener_with_missing_wheel_callbacks_does_not_crash(engine_minimized):
    """A partial mouse listener must survive a horizontal wheel event.

    A Python IMouseListener that implements only a subset of the callbacks
    must not abort when FIFE dispatches a horizontal wheel event. If the
    SWIG interface declares the callbacks pure, the inherited wrapper raises
    "SWIG director pure virtual method called" before the C++ no-op runs.
    """

    class PartialMouseListener(fife.IMouseListener):
        def __init__(self):
            fife.IMouseListener.__init__(self)
            self.pressed = 0

        def mousePressed(self, evt):
            self.pressed += 1

    eventmanager = engine_minimized.getEventManager()
    listener = PartialMouseListener()
    eventmanager.addMouseListener(listener)
    try:
        # Horizontal-dominant wheel: the old code raised here.
        _push_mouse_wheel(1.0, 0.01)
        engine_minimized.pump()

        # The listener is still alive and usable after the default no-op ran.
        assert listener.pressed == 0
    finally:
        eventmanager.removeMouseListener(listener)


def test_mouse_wheel_deltas_reach_python_listener(engine_minimized):
    """Implemented callbacks still run, with the new wheel delta API."""

    class WheelListener(fife.IMouseListener):
        def __init__(self):
            fife.IMouseListener.__init__(self)
            self.events = []

        def mouseWheelMovedRight(self, evt):
            self.events.append(
                (
                    "right",
                    evt.getWheelDeltaX(),
                    evt.getWheelDeltaY(),
                    evt.getX(),
                    evt.getY(),
                )
            )

        def mouseWheelMovedUp(self, evt):
            self.events.append(
                (
                    "up",
                    evt.getWheelDeltaX(),
                    evt.getWheelDeltaY(),
                    evt.getX(),
                    evt.getY(),
                )
            )

    eventmanager = engine_minimized.getEventManager()
    listener = WheelListener()
    eventmanager.addMouseListener(listener)
    try:
        _push_mouse_wheel(1.0, 0.01, mouse_x=11.0, mouse_y=22.0)
        engine_minimized.pump()

        assert len(listener.events) == 1
        kind, delta_x, delta_y, x, y = listener.events[0]
        assert kind == "right"
        assert delta_x == pytest.approx(1.0)
        assert delta_y == pytest.approx(0.01)
        assert x == 11
        assert y == 22

        listener.events.clear()
        _push_mouse_wheel(0.01, 1.0, mouse_x=33.0, mouse_y=44.0)
        engine_minimized.pump()

        assert len(listener.events) == 1
        kind, delta_x, delta_y, x, y = listener.events[0]
        assert kind == "up"
        assert delta_x == pytest.approx(0.01)
        assert delta_y == pytest.approx(1.0)
        assert x == 33
        assert y == 44
    finally:
        eventmanager.removeMouseListener(listener)


def test_partial_touch_listener_does_not_crash(engine_minimized):
    """A touch listener may implement only the gestures it cares about."""

    class PartialTouchListener(fife.ITouchListener):
        def __init__(self):
            fife.ITouchListener.__init__(self)
            self.downs = 0

        def touchDown(self, evt):
            self.downs += 1

    eventmanager = engine_minimized.getEventManager()
    listener = PartialTouchListener()
    eventmanager.addTouchListener(listener)
    try:
        _push_finger(_SDL_EVENT_FINGER_DOWN, 101, 1)
        engine_minimized.pump()
        assert listener.downs == 1

        # None of these callbacks are implemented by the subclass. The C++
        # no-op defaults must run instead of a pure-virtual director upcall.
        _push_finger(_SDL_EVENT_FINGER_UP, 101, 1)
        _push_pinch(_SDL_EVENT_PINCH_BEGIN, 1.0)
        _push_pinch(_SDL_EVENT_PINCH_UPDATE, 2.0)
        _push_pinch(_SDL_EVENT_PINCH_END, 1.0)
        engine_minimized.pump()

        assert listener.downs == 1
    finally:
        eventmanager.removeTouchListener(listener)


def test_touch_and_pinch_events_reach_python_listener(engine_minimized):
    """Finger and pinch payloads are available through the new touch API."""

    class TouchListener(fife.ITouchListener):
        def __init__(self):
            fife.ITouchListener.__init__(self)
            self.events = []

        def touchDown(self, evt):
            self.events.append(
                (
                    "down",
                    evt.getTouchId(),
                    evt.getFingerId(),
                    evt.getNormalizedX(),
                    evt.getNormalizedY(),
                    evt.getFingerCount(),
                )
            )

        def touchMotion(self, evt):
            self.events.append(("motion", evt.getDeltaX(), evt.getDeltaY()))

        def touchUp(self, evt):
            self.events.append(("up", evt.getFingerCount()))

        def pinchBegin(self, evt):
            self.events.append(("pinch_begin", evt.getScale()))

        def pinchUpdate(self, evt):
            self.events.append(("pinch_update", evt.getScale()))

        def pinchEnd(self, evt):
            self.events.append(("pinch_end", evt.getScale()))

    eventmanager = engine_minimized.getEventManager()
    listener = TouchListener()
    eventmanager.addTouchListener(listener)
    try:
        _push_finger(_SDL_EVENT_FINGER_DOWN, 202, 7, x=0.25, y=0.75)
        engine_minimized.pump()
        assert listener.events == [
            ("down", 202, 7, pytest.approx(0.25), pytest.approx(0.75), 1)
        ]

        listener.events.clear()
        _push_finger(_SDL_EVENT_FINGER_MOTION, 202, 7, x=0.5, y=0.5, dx=0.1, dy=-0.2)
        engine_minimized.pump()
        assert listener.events == [("motion", pytest.approx(0.1), pytest.approx(-0.2))]

        listener.events.clear()
        _push_pinch(_SDL_EVENT_PINCH_BEGIN, 1.0)
        _push_pinch(_SDL_EVENT_PINCH_UPDATE, 2.0)
        _push_pinch(_SDL_EVENT_PINCH_UPDATE, 0.5)
        _push_pinch(_SDL_EVENT_PINCH_END, 1.0)
        engine_minimized.pump()
        assert listener.events == [
            ("pinch_begin", pytest.approx(1.0)),
            ("pinch_update", pytest.approx(2.0)),
            ("pinch_update", pytest.approx(1.0)),
            ("pinch_end", pytest.approx(1.0)),
        ]

        listener.events.clear()
        _push_finger(_SDL_EVENT_FINGER_UP, 202, 7)
        engine_minimized.pump()
        assert listener.events == [("up", 0)]
    finally:
        eventmanager.removeTouchListener(listener)
