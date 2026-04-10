#!/usr/bin/env python

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

# ------------------------------------------------------------------------------
# This is the pychan demo of Fifengine.
# ------------------------------------------------------------------------------

"""Pychan demo application and example utilities.

This module contains the Pychan demo application and helper classes used
by the Pychan examples in the demos/pychan folder.
"""

import os
import sys

from fife import fife
from fife.extensions import pychan
from fife.extensions.pychan import pychanbasicapplication
from fife.extensions.pychan.dialogs import trace


class PyChanExample:
    """Simple example container that loads and runs a pychan XML widget."""

    def __init__(self, xmlFile, application=None):
        self.application = application
        self.xmlFile = xmlFile
        self.widget = None

    def start(self):
        """Start the example by loading and showing its widget."""
        # For simplicity the most basic examples should define
        # a okButton and/or a closeButton. Those are mapped
        # to the stop handler.
        self.widget = pychan.loadXML(self.xmlFile)
        eventMap = {"closeButton": self.stop, "okButton": self.stop}
        # Since the basic example are not required to
        # supply close and ok button, we 'ignoreMissing'
        self.widget.mapEvents(eventMap, ignoreMissing=True)
        self.widget.show()

        # from pprint import pprint
        # pprint(self.widget.getNamedChildren())

    def stop(self):
        """Stop the example and hide its widget."""
        if self.widget:
            self.widget.hide()
        self.widget = None
        if self.application:
            self.application.demoList.requestFocus()


class TextSetter:
    """Callable that sets the `text` attribute on a widget."""

    def __init__(self, text):
        self.text = text

    def __call__(self, widget):
        """Set the widget text to the configured value."""
        widget.text = self.text


class KeyFilter(fife.IKeyFilter):
    """Implement a FIFE key filter that blocks a configured set of keys."""

    def __init__(self, keys):
        """Initialize the filter with an iterable of FIFE key values."""
        fife.IKeyFilter.__init__(self)
        self._keys = keys

    def isFiltered(self, event):
        """Return True if the event key value is in the filter set.

        Returns
        -------
        bool
            True when the event's key value is in the filter set, otherwise False.
        """
        return event.getKey().getValue() in self._keys


class ApplicationListener(fife.IKeyListener, fife.ICommandListener):
    """Listen for application-level key and command events."""

    def __init__(self, engine):
        """Create and register the listener with the engine event manager."""
        self._engine = engine
        self._eventmanager = self._engine.getEventManager()

        fife.IKeyListener.__init__(self)
        self._eventmanager.addKeyListener(self)

        fife.ICommandListener.__init__(self)
        self._eventmanager.addCommandListener(self)

        keyfilter = KeyFilter([fife.Key.ESCAPE])
        keyfilter.__disown__()

        self._eventmanager.setKeyFilter(keyfilter)

        self.quit = False

    def keyPressed(self, event):
        """Process non-game keyboard input (shortcuts)."""
        if event.isConsumed():
            return

        keyval = event.getKey().getValue()

        if keyval == fife.Key.ESCAPE:
            self.quit = True
            event.consume()

    def keyReleased(self, event):
        """Handle key release events (no-op)."""
        pass

    def onCommand(self, command):
        """Handle incoming commands such as quit requests."""
        if command.getCommandType() == fife.CMD_QUIT_GAME:
            self.quit = True
            command.consume()


class DemoApplication(pychanbasicapplication.PychanApplicationBase):
    """Main demo application that exposes a selection of Pychan examples."""

    def __init__(self):
        # Let the ApplicationBase initialise FIFE
        super().__init__()

        # Init Pychan
        pychan.loadFonts("fonts/freefont.xml")
        pychan.manager.setDefaultFont("FreeSans")
        pychan.setupModalExecution(self.mainLoop, self.breakFromMainLoop)

        # Build the main GUI
        self.gui = pychan.loadXML("gui/demoapp.xml")
        self.gui.min_size = (
            self.engine.getRenderBackend().getScreenWidth(),
            self.engine.getRenderBackend().getScreenHeight(),
        )

        eventMap = {
            "creditsLink": self.showCredits,
            "closeButton": self.quit,
            "demoList": self.selectExample,
            "xmlButton": self.loadRuntimeXML,
        }
        self.gui.mapEvents(eventMap)

        # A simple hover-effect for the credits label
        credits = self.gui.findChild(name="creditsLink")
        # Note that we can't simply write:
        # credits.capture(credits._setText(u"Credits"), event_name="mouseExited")
        # that's because that would call credits._setText _NOW_ and we want to call
        # it later.
        credits.capture(lambda: credits._setText("CREDITS"), event_name="mouseEntered")
        credits.capture(lambda: credits._setText("Credits"), event_name="mouseExited")

        # import example modules
        from colortester import ColorExample
        from dynamic import DynamicExample
        from dynamicgraph import DynamicGraphExample
        from iconprogressbar import IconProgressBarExample
        from imageprogressbar import ImageProgressBarExample
        from modalfocus import ModalFocusExample
        from poc_gui_animation import PocAnimations
        from showhide import ShowHideExample
        from sliders import SliderExample
        from stretching import StretchingExample
        from styling import StylingExample
        from tabbedarea import TabbedAreaExample

        # Our list of examples
        # We keep a dictionary of these and fill
        # the ListBox on the left with its names.
        self.examples = {
            "Absolute Positioning": PyChanExample("gui/absolute.xml", self),
            "Adjusting Container": PyChanExample("gui/adjustingcontainer.xml"),
            "All Widgets": PyChanExample("gui/all_widgets.xml"),
            "Basic Styling": StylingExample(),
            "Circular Box": PyChanExample("gui/circularcontainer.xml"),
            "Dynamic Widgets": DynamicExample(),
            "Sliders": SliderExample(),
            "ScrollArea": PyChanExample("gui/scrollarea.xml"),
            "Colortester": ColorExample(),
            "GuiAnimations": PocAnimations(),
            "Tabbed Area": TabbedAreaExample(),
            "Image Stretching": StretchingExample(),
            "Resizable Window": PyChanExample("gui/resizable.xml"),
            "Dock Area": PyChanExample("gui/dockarea.xml"),
            "Graph Widgets": PyChanExample("gui/graphwidgets.xml"),
            "Dynamic Graph": DynamicGraphExample(),
            "Icon Progress Bar": IconProgressBarExample(),
            "Image Progress Bar": ImageProgressBarExample(),
            "Flow Container": PyChanExample("gui/flowcontainer.xml"),
            "Animation Icon": PyChanExample("gui/animationicon.xml"),
            "Modal Focus": ModalFocusExample(),
            "Show and Hide": ShowHideExample(),
        }
        self.demoList = self.gui.findChild(name="demoList")
        self.demoList.items = sorted(self.examples.keys())

        # Finally show the main GUI
        self.gui.show()

        self.currentExample = None
        self.creditsWidget = None

    # We use the trace decorator which can help debugging the examples.
    # mostly it's for show though :-)
    @trace
    def selectExample(self):
        """Handle selection of an example from the example list."""
        if self.demoList.selected_item is None:
            return
        # print "selected",self.demoList.selected_item
        if self.currentExample:
            self.currentExample.stop()
        self.currentExample = self.examples[self.demoList.selected_item]
        self.gui.findChild(name="xmlSource").text = open(
            self.currentExample.xmlFile
        ).read()
        self.currentExample.start()

    def loadRuntimeXML(self):
        """Load the XML from the editor and run it as an example."""
        if self.demoList.selected_item is None:
            return

        if self.currentExample:
            self.currentExample.stop()

        # save source to file
        tmp = open("gui/tmp.xml", "w")
        tmp.write(self.gui.findChild(name="xmlSource").text.encode("utf-8"))
        tmp.close()
        # change the xml path, load it and reset the path
        xml_orig = self.currentExample.xmlFile
        self.currentExample.xmlFile = "gui/tmp.xml"
        self.currentExample.start()
        self.currentExample.xmlFile = xml_orig

    def showCredits(self):
        """Show the credits popup for the demo application."""
        # We use PyChan's synchronous execution feature here.
        if self.creditsWidget is None:
            self.creditsWidget = pychan.loadXML("gui/credits.xml")
        self.creditsWidget.execute({"okButton": "Yay!"})

    def createListener(self):
        """Create and return the application listener used by the demo.

        Note
        ----
        This function is overloaded so the demo can provide a custom
        :class:`ApplicationListener` instance.

        Returns
        -------
        ApplicationListener
            The created ApplicationListener instance.
        """
        self._listener = ApplicationListener(self.engine)
        return self._listener

    def _pump(self):
        """Pump the demo and quit when the listener requests termination."""
        if self._listener.quit:
            self.quit()


class TestXMLApplication(pychanbasicapplication.PychanApplicationBase):
    """Test application that loads a single XML file for interactive tests."""

    def __init__(self, xmlfile):
        super().__init__()
        pychan.init(self.engine, debug=True)
        self.start()

    @trace
    def start(self):
        """Start the test application by loading and showing the widget."""
        self.widget = pychan.loadXML(self.xmlFile)
        self.widget.show()


if __name__ == "__main__":
    import sys

    print("Using the FIFE python module found here: ", os.path.dirname(fife.__file__))

    if len(sys.argv) == 2:
        app = TestXMLApplication(sys.argv[1])
    else:
        app = DemoApplication()
    app.run()
