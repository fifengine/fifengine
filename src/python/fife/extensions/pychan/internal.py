# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""PyChan internal module for widget management."""

from fife.extensions import fife_timer as timer

from .compat import fifechan, in_fife
from .exceptions import InitializationError


def get_manager():
    """
    Get the manager from inside pychan.

    To avoid cyclic imports write::
       from internal import get_manager

    Returns
    -------
    Manager
        The global `Manager` instance used by pychan.
    """
    return Manager.manager


def screen_width():
    """Get the screen width.

    Returns
    -------
    int
        The current screen width in pixels.
    """
    return get_manager().hook.screen_width


def screen_height():
    """Get the screen height.

    Returns
    -------
    int
        The current screen height in pixels.
    """
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
        """Get a reference to the console.

        Returns
        -------
        Console
            The console instance managed by the GUI hook.
        """
        return self.hook.console

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

        resolved = {}
        for k, v in style.items():
            if isinstance(k, tuple):
                for item in k:
                    resolved[item] = v
            else:
                resolved[k] = v
        style = resolved

        default_entries = self.styles.get("default", {})
        default_section = style.get("default", {})
        for k, v in list(default_entries.items()):
            if k == "default":
                continue
            if k in style:
                continue
            inherited = dict(v)
            for prop in default_section:
                inherited.pop(prop, None)
            style[k] = inherited
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
        for applicable, specstyle in sorted(
            style.items(),
            key=lambda x: (
                isinstance(x[0], type),
                x[0].__name__ if isinstance(x[0], type) else str(x[0]),
            ),
        ):
            if applicable == "default":
                continue
            if not isinstance(applicable, tuple):
                applicable = (applicable,)
            if cls in applicable:
                for k, v in list(specstyle.items()):
                    v = kwargs.get(k, v)
                    setattr(widget, k, v)

    def _remapStyleKeys(self, style):
        """Translate style selectors to tuples of widget classes. (internal).

        Returns
        -------
        dict
            A copy of `style` with string selectors remapped to widget classes.
        """

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
            new_k = tuple(map(_toClass, k)) if isinstance(k, tuple) else _toClass(k)
            style_copy[new_k] = v
        return style_copy

    def getFont(self, font):
        """Get a font by name from the FifechanManager.

        Parameters
        ----------
        font : str
            The font family name or identifier.

        Returns
        -------
        IFont
            The font instance from the FifechanManager.
        """
        if font == "default":
            return self.hook.guimanager.getDefaultFont()
        return self.hook.guimanager.createFont(font, 0)

    def setDefaultFont(self, font, size=12):
        """Set the default font by family name.

        Parameters
        ----------
        font : str
            The font family name (e.g. "FreeSans") registered in the FontManager.
        size : int, optional
            The font size (default 12).
        """
        self.hook.guimanager.setDefaultFont(font, size)

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

        Raises
        ------
        InitializationError
            If `filename` is empty or invalid.
        """
        if not filename:
            raise InitializationError("Empty Image file.")
        return self.hook.load_image(filename, gui)


# Default Widget style.

DEFAULT_STYLE = {
    "default": {
        "border_size": 0,
        "border_style": 1,
        "margins": (0, 0),
        "base_color": fifechan.Color(35, 35, 45),
        "foreground_color": fifechan.Color(210, 210, 220),
        "background_color": fifechan.Color(45, 45, 58),
        "selection_color": fifechan.Color(70, 90, 140),
        "border_color": fifechan.Color(60, 60, 80),
        "highlight_offset": fifechan.Color(25, 25, 35),
        "shadow_offset": fifechan.Color(15, 15, 20),
        "font": "default",
    },
    "Button": {
        "border_size": 2,
        "margins": (5, 2),
        "min_size": (15, 10),
        "base_color": fifechan.Color(50, 55, 72),
    },
    "CheckBox": {
        "border_size": 0,
    },
    "RadioButton": {
        "border_size": 0,
        "background_color": fifechan.Color(0, 0, 0),
    },
    "Label": {"border_size": 0, "background_color": fifechan.Color(45, 45, 58, 0)},
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
        "base_color": fifechan.Color(38, 38, 50),
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
