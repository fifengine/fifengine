# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""PyChan internal module for widget management."""

from fife.extensions import fife_timer as timer

from . import fonts
from .compat import fife, fifechan, in_fife
from .exceptions import InitializationError


def get_manager():
    """
    Get the manager from inside pychan.

    To avoid cyclic imports write::
       from internal import get_manager
    """
    return Manager.manager


def screen_width():
    """Get the screen width."""
    return get_manager().hook.screen_width


def screen_height():
    """Get the screen height."""
    return get_manager().hook.screen_height


class Manager:
    """Manage PyChan widgets and resources."""

    manager = None

    def __init__(self, hook, debug=False, compat_layout=False):
        super().__init__()
        self.hook = hook
        self.debug = debug
        self.compat_layout = compat_layout
        self.unicodePolicy = ("ignore",)

        if in_fife:
            if not hook.engine.getEventManager():
                raise InitializationError("No event manager installed.")
            if not hook.guimanager:
                raise InitializationError("No GUI manager installed.")
        timer.init(hook.engine.getTimeManager())

        self.fonts = {}
        # glyphs = ' abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/:();%`\'*#=[]"'
        self.fonts["default"] = hook.default_font

        self.styles = {}
        self.addStyle("default", DEFAULT_STYLE)

        Manager.manager = self

        # Setup synchronous dialogs
        self.mainLoop = None
        self.breakFromMainLoop = None
        self.can_execute = False

        import weakref

        self.allTopHierachyWidgets = weakref.WeakKeyDictionary()

        self.allWidgets = set()

        # Autopos
        from .autoposition import placeWidget

        self.placeWidget = placeWidget

    def addWidget(self, widget):
        """
        Add a widget to the manager.

        Adds Widget to the manager. So the manager "owns" the Widget.
        Note: As long as the wiget is in self.allWidgets the Python
        GC can not free it.
        """
        if not widget._added:
            widget._added = True
            self.allWidgets.add(widget)

    def removeWidget(self, widget):
        """
        Remove a widget from the manager.

        Removes Widget from the manager.
        Note: As long as the wiget is in self.allWidgets the Python
        GC can not free it.
        """
        if widget._added:
            widget._added = False
            self.allWidgets.remove(widget)

    def setupModalExecution(self, mainLoop, breakFromMainLoop):
        """
        Set up synchronous execution of dialogs.

        Setup synchronous execution of dialogs.
        """
        self.mainLoop = mainLoop
        self.breakFromMainLoop = breakFromMainLoop
        self.can_execute = True

    def addTopWidget(self, widget):
        """
        Add a top hierarchy widget to Fifechan.

        Adds a top hierachy widget to Fifechan and place it on the screen.
        Used by L{Widget.show} - do not use directly.
        """
        if not widget._top_added:
            assert widget not in self.allTopHierachyWidgets
            widget._top_added = True
            self.allTopHierachyWidgets[widget] = 1
            self.hook.add_widget(widget.real_widget)

    def removeTopWidget(self, widget):
        """
        Remove a top hierarchy widget from Fifechan.

        Removes a top hierachy widget from Fifechan.
        Used by L{Widget.hide} - do not use directly.
        """
        if widget._top_added:
            assert widget in self.allTopHierachyWidgets
            widget._top_added = False
            self.hook.remove_widget(widget.real_widget)
            del self.allTopHierachyWidgets[widget]

    def getConsole(self):
        """Get a reference to the console."""
        return self.hook.console

    def getDefaultFont(self):
        """Get the default font."""
        return self.fonts["default"]

    def setDefaultFont(self, name):
        """Set the default font."""
        self.fonts["default"] = self.getFont(name)

    def getFont(self, name):
        """
        B{pending deprecation}.

        Returns a GuiFont identified by its name.

        Parameters
        ----------
        name : str
            A string identifier from the font definitions in pychans config files.
        """
        if in_fife:
            font = self.fonts.get(name)
            if isinstance(font, fife.GuiFont):
                return font
            if hasattr(font, "font") and isinstance(getattr(font, "font"), fife.GuiFont):
                return font.font
            raise InitializationError(
                f"Couldn't find the font '{name}'. Please load the xml file."
            )
        else:
            return self.hook.get_font(name)

    def createFont(self, path="", size=0, glyphs=""):
        """Create and return a GuiFont from the GUI Manager."""
        return self.hook.create_font(path, size, glyphs)

    def releaseFont(self, font):
        """
        Releases a font from memory.  Expects a fifechan.GuiFont.

        Notes
        -----
        This needs to be tested. Also should add a way to release a font by name (fonts.Font).
        """
        if not isinstance(font, fifechan.GuiFont):
            raise InitializationError(
                f"PyChan Manager expected a fifechan.GuiFont instance, not {repr(font)}."
            )
        self.hook.release_font(font)

    def addFont(self, font):
        """
        B{deprecated}.

        Add a font to the font registry. It's not necessary to call this directly.
        But it expects a L{fonts.Font} instance and throws an L{InitializationError}
        otherwise.

        Parameters
        ----------
        font : fonts.Font
            A fonts.Font instance.
        """
        if not isinstance(font, fonts.Font):
            raise InitializationError(
                f"PyChan Manager expected a fonts.Font instance, not {repr(font)}."
            )
        self.fonts[font.name] = font

    def addStyle(self, name, style):
        """
        Add a style to the style registry.

        Parameters
        ----------
        name : str
            The name of the style.
        style : dict
            The style dictionary.
        """
        style = self._remapStyleKeys(style)

        for k, v in list(self.styles.get("default", {}).items()):
            style[k] = style.get(k, v)
        self.styles[name] = style

    def stylize(self, widget, style, **kwargs):
        """
        Apply a style to a widget.

        Parameters
        ----------
        widget : Widget
            The widget to stylize.
        style : str
            The name of the style to apply.
        **kwargs : dict
            Additional keyword arguments to override style values.
        """
        style = self.styles[style]
        for k, v in list(style.get("default", {}).items()):
            v = kwargs.get(k, v)
            setattr(widget, k, v)

        cls = widget.__class__
        for applicable, specstyle in list(style.items()):
            if not isinstance(applicable, tuple):
                applicable = (applicable,)
            if cls in applicable:
                for k, v in list(specstyle.items()):
                    v = kwargs.get(k, v)
                    setattr(widget, k, v)

    def _remapStyleKeys(self, style):
        """Translate style selectors to tuples of widget classes. (internal)."""

        # Remap class names, create copy:
        def _toClass(class_):
            from . import widgets

            if class_ == "default":
                return class_

            if type(class_) is type(widgets.Widget) and issubclass(
                class_, widgets.Widget
            ):
                return class_
            if str(class_) not in widgets.WIDGETS:
                raise InitializationError(
                    f"Can't resolve {repr(class_)} to a widget class."
                )
            return widgets.WIDGETS[str(class_)]

        style_copy = {}
        for k, v in list(style.items()):
            if isinstance(k, tuple):
                new_k = tuple(map(_toClass, k))
            else:
                new_k = _toClass(k)
            style_copy[new_k] = v
        return style_copy

    def loadImage(self, filename, gui=True):
        """
        Load an image from a file.

        Parameters
        ----------
        filename : str
            The path to the image file.
        gui : bool
            Whether to load as a GUI image (default True).

        Returns
        -------
        Image
            The loaded image.
        """
        if not filename:
            raise InitializationError("Empty Image file.")
        return self.hook.load_image(filename, gui)


# Default Widget style.

DEFAULT_STYLE = {
    "default": {
        "border_size": 0,
        "margins": (0, 0),
        "base_color": fifechan.Color(28, 28, 28),
        "foreground_color": fifechan.Color(255, 255, 255),
        "background_color": fifechan.Color(50, 50, 50),
        "selection_color": fifechan.Color(80, 80, 80),
        "font": "default",
    },
    "Button": {
        "border_size": 2,
        "margins": (5, 2),
        "min_size": (15, 10),
    },
    "CheckBox": {
        "border_size": 0,
    },
    "RadioButton": {
        "border_size": 0,
        "background_color": fifechan.Color(0, 0, 0),
    },
    "Label": {"border_size": 0, "background_color": fifechan.Color(50, 50, 50, 0)},
    "ListBox": {
        "border_size": 0,
    },
    "Window": {
        "border_size": 0,
        "margins": (5, 5),
        "opaque": True,
        "padding": 2,
        "titlebar_height": 25,
        "background_image": None,
    },
    "TextBox": {},
    ("Container", "HBox", "VBox"): {
        "border_size": 0,
        "margins": (0, 0),
        "padding": 2,
        "opaque": True,
        "background_image": None,
    },
}
