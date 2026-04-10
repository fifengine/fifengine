# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""
Property bindings for PyChan widgets.

This module contains a set of property bindings for the widgets,
factored out to de-clutter the Widget class.
"""

from fife import fife, fifechan

from .exceptions import RuntimeError

new_str = str


def get_manager():
    """
    Get the PyChan manager instance.

    Returns
    -------
    Manager
        The PyChan manager.
    """
    from fife.extensions import pychan

    return pychan.manager


class WrappedProperty:
    """
    Base class for property bindings.

    Parameters
    ----------
    name : str
        The name of the property.
    """

    def __init__(self, name):
        self.name = name

    def _getSetter(self, obj):
        setter_name = "set" + self.name
        return getattr(obj.real_widget, setter_name)

    def _getGetter(self, obj):
        getter_name = "get" + self.name
        return getattr(obj.real_widget, getter_name)


class ColorProperty(WrappedProperty):
    """
    A color property.

    Fakes a color attribute of a fifechan widget. This accepts either
    tuples of the colors (r,g,b) or L{fife.Color} objects.

    Color objects have value semantics in this case.
    """

    def __init__(self, name):
        super().__init__(name)

    def __set__(self, obj, color):
        """
        Set the color property.

        Parameters
        ----------
        obj : Widget
            The widget to set the color on.
        color : tuple or Color
            The color to set.
        """
        if isinstance(color, tuple):
            color = fifechan.Color(*color)
        else:
            color = fifechan.Color(color.r, color.g, color.b, color.a)
        self._getSetter(obj)(color)

    def __get__(self, obj, objtype=None):
        """
        Get the color property.

        Parameters
        ----------
        obj : Widget
            The widget to get the color from.
        objtype : type
            The type of the widget.

        Returns
        -------
        Color
            The current color.
        """
        color = self._getGetter(obj)()
        return fifechan.Color(color.r, color.g, color.b, color.a)


class ImageProperty(WrappedProperty):
    """Unifies the way Images and Image attributes are handled inside PyChan."""

    def __init__(self, name):
        super().__init__(name)
        self.prop_name = "_prop_" + self.name.lower()

    def __set__(self, obj, image):
        """
        Set the image property.

        Parameters
        ----------
        obj : Widget
            The widget to set the image on.
        image : str or GuiImage or None
            The image to set.

        Raises
        ------
        RuntimeError
            If `image` is not a string or `fife.GuiImage` instance.
        """
        image_info = getattr(obj, self.prop_name, {})
        if not image:
            image_info["source"] = ""
            image_info["image"] = fife.GuiImage()
            image_info["image"]._source = ""
            self._getSetter(obj)(None)

        elif isinstance(image, (str, new_str)):
            image_info["source"] = image
            image_info["image"] = get_manager().loadImage(image)
            image_info["image"].source = image
            self._getSetter(obj)(image_info["image"])

        elif isinstance(image, fife.GuiImage):
            image_info["source"] = getattr(image, "source", "")
            image_info["image"] = image
            if image_info["source"]:
                image_info["image"] = get_manager().loadImage(image_info["source"])
            self._getSetter(obj)(image_info["image"])
        else:
            attribute_name = f"{obj.__class__.__name__}.{self.name}"
            error_message = "%s only accepts GuiImage and python strings, not '%s'"
            raise RuntimeError(error_message % (attribute_name, repr(image)))

        setattr(obj, self.prop_name, image_info)

    def __get__(self, obj, objtype=None):
        """
        Get the image property.

        Parameters
        ----------
        obj : Widget
            The widget to get the image from.
        objtype : type
            The type of the widget.

        Returns
        -------
        GuiImage
            The current image.
        """
        d = getattr(obj, self.prop_name, {})
        image = d.get("image", None)
        if not image:
            image = fife.GuiImage()
            image.source = ""
        return image
