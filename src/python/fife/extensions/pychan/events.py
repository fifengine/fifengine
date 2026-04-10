# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
PyChan event handling (internal).

Provide event handling for PyChan widgets.

Users shouldn't need to use this module directly.
L{widgets.Widget.capture} and L{widgets.Widget.mapEvents} provide
a convenient API to capture events.

Nevertheless to understand how its supposed to work
take a look at L{EventMapper} and L{EventListenerBase}

Event callbacks
---------------

You can either write callbacks yourself or
use L{tools.callbackWithArguments} or L{tools.attrSetCallback}
to generate suitable callbacks.

Here's an example callback::
   def dumpEventInfo(event=0,widget=0):
      print widget, " received the event ", event

Note the signature - C{event} and C{widget} are keyword
arguments passed to the callback. If doesn't accept either
C{event} or C{widget} as argument, these are not passed.

This way a simple function which ignores C{event} or C{widget}
can be used, while they are available if needed.

Currently only one callback can be set per event. In case
you don't want to write your own callback that dispatches
to different callbacks you can use L{tools.chainCallbacks}.

Available Events
----------------

"""

import traceback
import weakref

from fife.extensions.fife_timer import Timer
from fife.extensions.pychan.tools import callbackWithArguments as cbwa

from . import exceptions, tools
from .compat import fifechan
from .internal import get_manager

EVENTS = [
    "mouseEntered",
    "mouseExited",
    "mousePressed",
    "mouseReleased",
    "mouseClicked",
    "mouseMoved",
    "mouseWheelMovedUp",
    "mouseWheelMovedDown",
    "mouseWheelMovedRight",
    "mouseWheelMovedLeft",
    "mouseDragged",
    "action",
    "keyPressed",
    "keyReleased",
    "widgetResized",
    "widgetMoved",
    "widgetHidden",
    "widgetShown",
    "ancestorMoved",
    "ancestorHidden",
    "ancestorShown",
]

# Add the EVENTS to the docs.
__doc__ += "".join(f" - {event}\n" for event in EVENTS)

MOUSE_EVENT, KEY_EVENT, ACTION_EVENT, WIDGET_EVENT = list(range(4))


def getEventType(name):
    """Get the event type for a given event name.

    Returns
    -------
    int
        One of MOUSE_EVENT, KEY_EVENT, WIDGET_EVENT or ACTION_EVENT.
    """
    if "mouse" in name:
        return MOUSE_EVENT
    if "key" in name:
        return KEY_EVENT
    if ("widget" in name) or ("ancestor" in name):
        return WIDGET_EVENT
    return ACTION_EVENT


CALLBACK_NONE_MESSAGE = """\
You passed None as parameter to %s.capture, which would normally remove a mapped event.
But there was no event mapped. Did you accidently call a function instead of passing it?
"""


class EventListenerBase:
    """
    Redirect events to callbacks.

    Use *only* from L{EventMapper}.

    This class uses the SWIG director feature - overriden
    virtual methods are called from C++ to - listen to
    Fifechan events.
    """

    def __init__(self):
        super().__init__()
        self.events = {}
        self.indent = 0
        self.debug = get_manager().debug
        self.is_attached = False

        # Enables event redirection to next pump cycle by default
        self._redirect = True
        self._timers = []
        self._deadtimers = []

    def attach(self, widget):
        """
        Start receiving events.

        No need to call this manually.
        """
        if self.is_attached:
            return
        if not self.events:
            return
        if self.debug:
            print("Attach:", self)
        self.doAttach(widget.real_widget)
        self.widget_ref = weakref.ref(widget)
        self.is_attached = True

    def detach(self):
        """
        Stop receiving events.

        No need to call this manually.
        """
        if not self.is_attached:
            return
        if self.debug:
            print("Detach:", self)
        self.is_attached = False

    def setRedirection(self, redirect):
        """
        Set event redirection mode.

        If enabled, the events are redirected to the next
        engine pump cycle. Otherwise the exectution is on
        the same engine cycle.
        """
        self._redirect = redirect

    def _redirectEvent(self, name, event):
        self.indent += 4
        try:
            event = self.translateEvent(getEventType(name), event)
            if name in self.events:
                if self.debug:
                    print("-" * self.indent, name)
                for f in self.events[name].values():
                    if not self._redirect:
                        f(event)
                        continue

                    def delayed_f(timer, f=f):  # bind f during loop
                        n_timer = timer()
                        f(event)

                        # FIXME: figure out a way to get rid of the dead timer list
                        del self._deadtimers[:]

                        if n_timer in self._timers:
                            self._deadtimers.append(n_timer)
                            self._timers.remove(n_timer)

                    timer = Timer(repeat=1)
                    timer._callback = cbwa(delayed_f, weakref.ref(timer))
                    timer.start()

                    self._timers.append(timer)

        except:
            print(name, repr(event))
            traceback.print_exc()
            raise

        finally:
            self.indent -= 4

    def translateEvent(self, event_type, event):
        """Translate event from fifechan to PyChan format.

        Returns
        -------
        object
            The translated event object suitable for PyChan callbacks.
        """
        if event_type == MOUSE_EVENT:
            return get_manager().hook.translate_mouse_event(event)
        if event_type == KEY_EVENT:
            return get_manager().hook.translate_key_event(event)
        return event


class _ActionEventListener(EventListenerBase, fifechan.ActionListener):
    def __init__(self):
        super().__init__()

    def doAttach(self, real_widget):
        real_widget.addActionListener(self)

    def doDetach(self, real_widget):
        real_widget.removeActionListener(self)

    def action(self, e):
        self._redirectEvent("action", e)


class _MouseEventListener(EventListenerBase, fifechan.MouseListener):
    def __init__(self):
        super().__init__()

    def doAttach(self, real_widget):
        real_widget.addMouseListener(self)

    def doDetach(self, real_widget):
        real_widget.removeMouseListener(self)

    def mouseEntered(self, e):
        self._redirectEvent("mouseEntered", e)

    def mouseExited(self, e):
        self._redirectEvent("mouseExited", e)

    def mousePressed(self, e):
        self._redirectEvent("mousePressed", e)

    def mouseReleased(self, e):
        self._redirectEvent("mouseReleased", e)

    def mouseClicked(self, e):
        self._redirectEvent("mouseClicked", e)

    def mouseMoved(self, e):
        self._redirectEvent("mouseMoved", e)

    def mouseWheelMovedUp(self, e):
        self._redirectEvent("mouseWheelMovedUp", e)

    def mouseWheelMovedDown(self, e):
        self._redirectEvent("mouseWheelMovedDown", e)

    def mouseWheelMovedRight(self, e):
        self._redirectEvent("mouseWheelMovedRight", e)

    def mouseWheelMovedLeft(self, e):
        self._redirectEvent("mouseWheelMovedLeft", e)

    def mouseDragged(self, e):
        self._redirectEvent("mouseDragged", e)


class _KeyEventListener(EventListenerBase, fifechan.KeyListener):
    def __init__(self):
        super().__init__()

    def doAttach(self, real_widget):
        real_widget.addKeyListener(self)

    def doDetach(self, real_widget):
        real_widget.removeKeyListener(self)

    def keyPressed(self, e):
        self._redirectEvent("keyPressed", e)

    def keyReleased(self, e):
        self._redirectEvent("keyReleased", e)


class _WidgetEventListener(EventListenerBase, fifechan.WidgetListener):
    def __init__(self):
        super().__init__()

    def doAttach(self, real_widget):
        real_widget.addWidgetListener(self)

    def doDetach(self, real_widget):
        real_widget.removeWidgetListener(self)

    def widgetResized(self, e):
        self._redirectEvent("widgetResized", e)

    def widgetMoved(self, e):
        self._redirectEvent("widgetMoved", e)

    def widgetHidden(self, e):
        self._redirectEvent("widgetHidden", e)

    def widgetShown(self, e):
        self._redirectEvent("widgetShown", e)

    def ancestorMoved(self, e):
        self._redirectEvent("ancestorMoved", e)

    def ancestorHidden(self, e):
        self._redirectEvent("ancestorHidden", e)

    def ancestorShown(self, e):
        self._redirectEvent("ancestorShown", e)


class EventMapper:
    """
    Handle events and callbacks for L{widgets.Widget}.

    Every PyChan widget has an L{EventMapper} instance
    as attribute B{event_mapper}.

    This instance handles all necessary house-keeping.
    Such an event mapper can be either C{attached} or
    C{detached}. In its attached state an L{EventListenerBase}
    is added to the Fifechan widget and will redirect
    the events to the callbacks.

    In its detached state no events are received from the
    real Fifechan widget.

    The event mapper starts in the detached state.
    When a new event is captured the mapper attaches itself
    automatically. The widget doesn't need to handle that.
    """

    def __init__(self, widget):
        super().__init__()
        self.widget_ref = weakref.ref(widget)
        self.callbacks = {}
        self.listener = {
            KEY_EVENT: _KeyEventListener(),
            ACTION_EVENT: _ActionEventListener(),
            MOUSE_EVENT: _MouseEventListener(),
            WIDGET_EVENT: _WidgetEventListener(),
        }
        self.is_attached = False
        self.debug = get_manager().debug

    def __repr__(self):
        """Return string representation.

        Returns
        -------
        str
            Human-readable representation of the EventMapper.
        """
        return f"EventMapper({repr(self.widget_ref())})"

    def attach(self):
        """Attach all listeners to their widgets."""
        for listener in list(self.listener.values()):
            listener.attach()

    def detach(self):
        """Detach all listeners from their widgets."""
        for listener in list(self.listener.values()):
            listener.detach()

    def capture(self, event_name, callback, group_name):
        """Capture an event with a callback.

        Parameters
        ----------
        event_name : str
            Name of the event to capture.
        callback : callable or None
            Callback to call when the event occurs, or `None` to remove.
        group_name : str
            Group name for the callback.

        Raises
        ------
        RuntimeError
            If `event_name` is unknown.
        """
        if event_name not in EVENTS:
            raise exceptions.RuntimeError("Unknown eventname: " + event_name)

        if callback is None:
            if self.isCaptured(event_name, group_name):
                self.removeEvent(event_name, group_name)
            elif self.debug:
                print(CALLBACK_NONE_MESSAGE % str(self.widget_ref()))
            return
        self.addEvent(event_name, callback, group_name)

    def isCaptured(self, event_name, group_name="default"):
        """Check if an event is captured.

        Returns
        -------
        bool
            True if the event/group is currently captured, False otherwise.
        """
        return (f"{event_name}/{group_name}") in self.getCapturedEvents()

    def getCapturedEvents(self):
        """Get a list of captured events.

        Returns
        -------
        list[str]
            A list of captured event descriptors in the form "name/group".
        """
        events = []
        for event_type, listener in list(self.listener.items()):
            for event_name, group in list(listener.events.items()):
                for group_name in list(group.keys()):
                    events.append(f"{event_name}/{group_name}")
        return events

    def getListener(self, event_name):
        """Get the listener for an event type.

        Returns
        -------
        EventListenerBase
            The listener instance handling the requested event type.
        """
        return self.listener[getEventType(event_name)]

    def removeEvent(self, event_name, group_name):
        """Remove an event capture."""
        listener = self.getListener(event_name)
        del listener.events[event_name][group_name]

        if not listener.events[event_name]:
            del listener.events[event_name]
        if not listener.events:
            listener.detach()

        del self.callbacks[group_name][event_name]
        if len(self.callbacks[group_name]) <= 0:
            del self.callbacks[group_name]

    def addEvent(self, event_name, callback, group_name):
        """Add an event with a callback.

        Raises
        ------
        RuntimeError
            If `callback` is not callable.
        """
        if not callable(callback):
            raise RuntimeError(
                f"An event callback must be either a callable or None - not {repr(callback)}"
            )
        # The closure self needs to keep a weak ref.
        # Otherwise the GC has problems.
        self_ref = weakref.ref(self)

        # Set up callback dictionary. This should fix some GC issues
        if group_name not in self.callbacks:
            self.callbacks[group_name] = {}

        if event_name not in self.callbacks[group_name]:
            self.callbacks[group_name][event_name] = {}

        self.callbacks[group_name][event_name] = callback

        def captured_f(event):
            if self_ref() is not None:
                tools.applyOnlySuitable(
                    self_ref().callbacks[group_name][event_name],
                    event=event,
                    widget=self_ref().widget_ref(),
                )

        listener = self.getListener(event_name)

        if event_name not in listener.events:
            listener.events[event_name] = {group_name: captured_f}
        else:
            listener.events[event_name][group_name] = captured_f
        listener.attach(self.widget_ref())


def splitEventDescriptor(name):
    """Split "widgetName/eventName" descriptions into tuples.

    Returns
    -------
    tuple
        A tuple (widgetName, eventName, groupName) parsed from the descriptor.

    Raises
    ------
    RuntimeError
        If the descriptor is malformed or contains an unknown event name.
    """
    L = name.split("/")
    if len(L) not in (1, 2, 3):
        raise exceptions.RuntimeError(
            "Invalid widgetname / eventname combination: " + name
        )
    if len(L) == 1:
        L = L[0], "action"
    elif L[1] not in EVENTS:
        raise exceptions.RuntimeError("Unknown event name: " + name)
    if len(L) == 2:
        L = L[0], L[1], "default"
    return L
