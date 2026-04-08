# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
Pythonic Fifechan Wrapper - PyChan.
=================================

Pythonic GUI API.


Features
--------
 - Simpler Interface
 - Very Basic XML Format support
 - Basic Layout Engine
 - Pseudo-Synchronous Dialogs.
 - Automagic background tiling (WIP)
 - Basic Styling support.
 - Simple Font Handling

TODO
----
 - Make setting parent attribute imply containment relation.
 - Finalize Widget.execute

 - Documentation ( Allways not enough :-( )
 - Handle Image Fonts
 - Move Font config files to XML, too ...

 - Implement real Menus
 - Implement StackWidget
 - Then implement TabWidget

 - GridLayout
 - Table

BUGS
----
 - Focus problems with Widget.execute.
 - Font.glyph_spacing is rendered incorrectly.
 - Is this a bug? At least inconvenient. MouseEntered events are not distributed for freshly shown widget.
 - It just looks bad.

Problems
--------
 - Reference counting problems again -sigh-
 ... and thus possible leaks.
 - High amount of code reuse -> Complex code
 - Needs at least new style classes and other goodies.
 - Missing documentation on:
     - Styling
     - ScrollArea
     - Fonts

How to use
==========

At its core you only need a few functions.
After setting up FIFE you need to initialize
pychan. After that you can load a GUI from an
XML file. Please see the documentation of L{loadXML}
for the details of the XML format
::
   import pychan
   pychan.init(fifeEngine)
   guiElement = pychan.loadXML("contents/gui/myform.xml")

The resulting guiElement can be shown and hidden with the
obvious L{widgets.Widget.show} and L{widgets.Widget.hide} methods.

To get a specific widget you have to give it a name in the XML
definition and use that to extract the widget from the returned
GUI element.
::
   okButton = guiElement.findChild(name="okButton")
   myInput = guiElement.findChild(name="myInput")

The data is extracted and set via direct attribute access.
These are using the python property technique to hide
behind the scenes manipulations. Please keep in mind that
the Layout engine and the exact way the widgets are displayed
is somewhat limited.
::
   myInput.text = "Blahblah"
   myList.items = ["1","2"]
   guiElement.position = (80,90)

A dialog without an OK button would be futile - so here's how
you hook widget events to function calls. Every widget
has a L{widgets.Widget.capture} method, which will directly call the passed
function when a widget event occurs. As a convenience a
L{widgets.Widget.mapEvents} function will batch the L{widgets.Widget.findChild} and
L{widgets.Widget.capture} calls in an obvious way.
::
   myButton.capture( application.quit )
   guiElement.mapEvents({
      'okButton' : self.applyAndClose,
      'closeButton':  guiElement.hide
   })

Other important places to look for information:
  - L{widgets.Widget} - Attributes explained.
  - L{loadXML} - Explain the XML format.
  - L{widgets.layout.LayoutBase} - Working of the layout engine.

Initialization, data distribution and collection
================================================

Very often a dialogs text fields, labels and listboxes have to be filled with data
after the creation of the dialog. This can be a tiresome process.
After a dialog has executed, B{other} attributes have to be read out again,
this to can be tiresome. PyChan simplifies both processes. But it treats them as three
processes. One is setting the data that will never be read out again - called B{initial data} -
the text of a checkbox or the list of a listBox are good examples. The second is setting the
data that is mutable by the user and may be read out again - for example the state of a checkbox
or the selected index in a list. The third and final process is collection of the user-mutable data::
  guiElement.distributeInitialData({
    'myListBox' : choices,
    'myLabel' : map.name,
  })
  guiElement.distributeData({
    'myTextField' : map.description
  })
  # ... process dialog.
  map.description, choice = guiElement.collectData('myListBox','myTextField')
  print "You selected:",choice,", good choice!"

See L{widgets.Widget.distributeData},L{widgets.Widget.distributeInitialData},
L{widgets.Widget.collectData} and L{widgets.Widget.collectDataAsDict}.

Styling and font handling
=========================

Note: These features are B{work in progress} and likely to change.

A style is a set of rules for matching widgets and a set of attributes
applied to them after creation. The attributes can be any of the given
attributes. Matching happens currently only by the widget class name
itself.

As an example the following style - written as a python data structure -
will set the border size of all labels to 10::
  style = {
      'ListBox' : { # Matches all listboxes
           'border_size : 10 # After creation call  lisbox.border_size = 10
      }
  }

As a convenience you can use the string B{default} to match all widget
classes and thus - for example apply a common font::
  style = {
     'default' : {
          'font' : 'console_small'
     }
  }

A new style is added to pychan with L{internal.Manager.addStyle}.
You can set a new default style by adding a style with the name 'default'.

The font is set via a string identifier pulled from a font definition
in a PyChan configuration file. You have to load these by calling
L{loadFonts} in your startup code::
   import pychan
   pychan.init( fifeEngine )
   pychan.loadFonts( "content/fonts/console.xml" )

The font definition files are in XML format.
Unicode and internationalisation
================================

All text that is visible and editable by the player has to be a unicode object.
All text that is used internally, e.g. widget names, have to be normal strings.

While PyChan will not raise an exception, if you do not follow this guideline,
you are encouraged to so.

You can change the way unicode encoding errors are handled by using the
function L{setUnicodePolicy}.


Widget hierachy
===============

Every widget can be contained in another container widget like L{Window}, L{VBox},
L{HBox}, L{Container} or L{ScrollArea}. Container widgets can contain any number
of widgets. Thus we have a tree like structure of the widgets - which finally makes
up the window or frame that is placed on the screen.

In PyChan widgets are supposed to be manipulated via the root of this hierachy,
so that the actual layout can be changed in the XML files without hassle.
It can be compared to how HTML works.

These bits and pieces connect things up::
 -  name - A (hopefully) unique name in the widget hierachy
 -  findChildren - The accessor method to find widgets by name or any other attribute.
 -  _parent - The parent widget in the widget hierachy
 -  deepApply - The method used to walk over the widget hierachy. You have to reimplement
   this in case you want to provide custom widgets.

Wrapping machinery
==================

The wrapping mechanism works be redirecting attribute access to the Widget
derived classes to a C{real_widget} member variable which in turn is an instance
of the SWIG wrapped Fifechan widget.

To ensure the real widget has already been constructed, when the wrapping machinery
is already in use, this has to be the first attribute to set in the constructors.
This leads to a reversed construction sequence as the super classes constructor
has to be invoked I{after} the subclass specific construction has taken place.

"""

from traceback import print_exc
from xml.sax import handler

__all__ = ["loadXML", "loadFonts", "init", "manager"]

# This *import should really be removed!
from .exceptions import GuiXMLError, InitializationError, PyChanException
from .fonts import loadFonts
from .widgets import WIDGETS, Widget
from .widgets.tabbedarea import Tab

### Initialisation ###

manager = None


def init(engine, debug=False, compat_layout=False):
    """
    Initialize PyChan before using any other pychan methods.

    This sets up a manager object which is available under pychan.manager.

    Parameters
    ----------
    engine : object
        The FIFE engine object.
    debug : bool, optional
        Enables and disables debugging output. Default is False.
    compat_layout : bool, optional
        Enables and disables compat layout. Default is False.
    """
    from .compat import _munge_engine_hook
    from .internal import Manager

    global manager

    manager = Manager(_munge_engine_hook(engine), debug, compat_layout)


# XML Loader


def traced(f):
    """
    Print tracebacks for any exceptions occurring in a function.

    Useful to avoid the infamous 'finally pops bad exception' that shadows
    the real cause of the error.
    """

    def traced_f(*args, **kwargs):
        try:
            return f(*args, **kwargs)
        except:
            print_exc()
            raise

    return traced_f


class __GuiLoaderBase(handler.ContentHandler):
    pass


class _GuiLoader(__GuiLoaderBase):
    def __init__(self):
        super().__init__()
        self.root = None
        self.indent = ""
        self.stack = []

    def _printTag(self, name, attrs):
        if not manager.debug:
            return
        attrstrings = [f'{str(k)}="{str(v)}"' for k, v in attrs.items()]
        tag = f"<{name} " + " ".join(attrstrings) + ">"
        try:
            print(self.indent + tag)
        except UnicodeEncodeError:
            print(self.indent + tag.encode("ascii", "backslashreplace"))

    def _resolveTag(self, name):
        """Resolve a XML Tag to a PyChan GUI class."""
        cls = WIDGETS.get(name, None)
        if cls is None:
            raise GuiXMLError(f"Unknown GUI Element: {name}")
        return cls

    def _setAttr(self, obj, name, value):
        if not hasattr(obj.__class__, "ATTRIBUTES"):
            raise PyChanException(
                f"The registered widget/spacer class {repr(obj)} does not supply an 'ATTRIBUTES'."
            )
        try:
            for attr in obj.ATTRIBUTES:
                if attr.name == name:
                    attr.set(obj, value)
                    return
        except GuiXMLError as e:
            raise GuiXMLError(f"Error parsing attr '{name}'='{value}' for '{obj}': '{e}'")
        raise GuiXMLError(f"Unknown GUI Attribute '{name}' on '{repr(obj)}'")

    def startElement(self, name, attrs):
        self._printTag(name, attrs)
        cls = self._resolveTag(name)
        if issubclass(cls, Widget):
            self.stack.append("gui_element")
            self._createInstance(cls, name, attrs)
        else:
            self.stack.append("unknown")
        self.indent += " " * 4

    def _createInstance(self, cls, name, attrs):
        obj = cls(parent=self.root)
        for k, v in list(attrs.items()):
            self._setAttr(obj, k, v)

        if self.root:
            if isinstance(obj, Tab):
                if hasattr(self.root, "addTabDefinition"):
                    self.root.addTabDefinition(obj)
                else:
                    raise GuiXMLError("A Tab needs to be added to a TabbedArea widget!")
            else:
                self.root.addChild(obj)
        self.root = obj

    def endElement(self, name):
        self.indent = self.indent[:-4]
        if manager.debug:
            print(f"{self.indent}</{name}>")
        if self.stack.pop() in ("gui_element"):
            self.root = self.root.parent or self.root


def loadXML(filename_or_stream):
    """
    Load a PyChan XML file and generate a widget from it.

    Parameters
    ----------
    filename_or_stream : str or file-like
        A filename or a file-like object (for example using StringIO).

    The XML format is very dynamic, in the sense, that the actual allowed tags and attributes
    depend on the PyChan code.

    So when a tag C{Button} is encountered, an instance of class Button will be generated,
    and added to the parent object.
    All attributes will then be parsed and then set in the following way:

      - position,size,min_size,max_size,margins - These are assumed to be comma separated tuples
        of integers.
      - foreground_color,base_color,background_color - These are assumed to be triples or quadruples of comma
        separated integers. (triples: r,g,b; quadruples: r,g,b,a)
      - border_size,padding - These are assumed to be simple integers.

    All other attributes are set verbatim as strings on the generated instance.
    In case a Widget does not accept an attribute to be set or the attribute can not be parsed
    correctly, the function will raise a GuiXMLError.

    In short::
            <VBox>
                    <Button text="X" min_size="20,20" base_color="255,0,0" border_size="2" />
            </VBox>

    This result in the following code executed::

            vbox = VBox(parent=None)
            button = Button(parent=vbox)
            button.text = "X"
            button.min_size = (20,20)
            button.base_color = (255,0,0)
            button.border_size = 2
            vbox.add( button )
    """
    from xml.sax import parse

    loader = _GuiLoader()
    parse(filename_or_stream, loader)
    return loader.root


def setupModalExecution(mainLoop, breakFromMainLoop):
    """
    Set up the synchronous dialog execution feature.

    You can enable synchronous dialog execution by
    passing two functions to this function.

    Parameters
    ----------
    mainLoop : callable
        The application's main loop. It should be callable (may be called
        recursively) and return the value passed to `breakFromMainLoop`.
    breakFromMainLoop : callable
        Function that causes `mainLoop` to finish and return the passed argument.

    Notes
    -----
    With these two functions dialogs can be executed synchronously. See
    L{widgets.Widget.execute}.
    """
    if not manager:
        raise InitializationError("PyChan is not initialized yet.")
    manager.setupModalExecution(mainLoop, breakFromMainLoop)


def setUnicodePolicy(*policy):
    """
    Set the unicode error handling policy.

    Possible options are:
     - 'strict' meaning that encoding errors raise a UnicodeEncodeError.
     - 'ignore' all encoding errors will be silently ignored.
     - 'replace' all errors are replaced by the next argument.

    For further information look at the python documentation,
    especially C{codecs.register_error}.

    Example::
            pychan.setUnicodePolicy('replace','?')
    """
    if not manager:
        raise InitializationError("PyChan is not initialized yet.")
    manager.unicodePolicy = policy
