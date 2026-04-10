# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
Simple error checking attributes.

This module defines a set of Attribute classes
which you can use to define possible values
an attribute of an object accepts.

Usage::
  class SomeObject:
      nameAttr, posAttr = [ Attr("name"), PointAttr("pos") ]

  obj =  SomeObject()
  obj.nameAttr.set(obj,"newName")
  obj.posAttr.set(obj,"89,89")

This is most useful for error checking parsing and defining
accepted attributes in classes and is used by pychan internally.

"""

from .exceptions import ParserError


class Attr:
    """A simple text attribute."""

    def __init__(self, name):
        self.name = name

    def set(self, obj, value):
        """
        Set the attribute value on an object.

        Parses the given value with the L{parse} method
        and sets it on the given instance with C{setattr}.
        """
        value = self.parse(value)
        setattr(obj, self.name, value)

    def parse(self, value):
        """Parse a value and check for errors.

        Override with specialized behavior.

        Returns
        -------
        str
            The parsed string value.
        """
        return str(value)


class UnicodeAttr(Attr):
    """Unicode text attribute."""

    def parse(self, value):
        """Parse a value as unicode string.

        Returns
        -------
        str
            The parsed unicode string.
        """
        return str(value)


class PointAttr(Attr):
    """Point attribute (x, y)."""

    def parse(self, value):
        """Parse a value as an x,y tuple.

        Returns
        -------
        tuple[int, int]
            The parsed (x, y) coordinates.

        Raises
        ------
        ParserError
            If the value cannot be parsed as two integers separated by a comma.
        """
        try:
            x, y = tuple(map(int, str(value).split(",")))
            return x, y
        except (TypeError, ValueError):
            raise ParserError(
                str(self.name) + " expected a comma separated list of two integers."
            )


class ColorAttr(Attr):
    """Color attribute (r, g, b, a)."""

    def parse(self, value):
        """Parse a value as an r,g,b,a color tuple.

        Returns
        -------
        tuple[int, int, int, int]
            The parsed (r, g, b, a) color values.

        Raises
        ------
        ParserError
            If the value cannot be parsed as a color tuple.
        """
        a = 255
        try:
            try:
                r, g, b, a = tuple(map(int, str(value).split(",")))
                for c in (r, g, b, a):
                    if not 0 <= c < 256:
                        raise ParserError(
                            str(self.name)
                            + f" expected a color (Failed: 0 <= {c} <= 255)"
                        )
            except ValueError:
                r, g, b = tuple(map(int, str(value).split(",")))
                for c in (r, g, b):
                    if not 0 <= c < 256:
                        raise ParserError(
                            str(self.name)
                            + f" expected a color (Failed: 0 <= {c} <= 255)"
                        )
        except (TypeError, ValueError, ParserError):
            raise ParserError(str(self.name) + " expected a color.")

        return r, g, b, a


class IntAttr(Attr):
    """Integer attribute."""

    def parse(self, value):
        """Parse a value as an integer.

        Returns
        -------
        int
            The parsed integer value.

        Raises
        ------
        ParserError
            If the value cannot be parsed as an integer.
        """
        try:
            return int(value)
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a single integer.")


class BoolAttr(Attr):
    """Boolean attribute."""

    def parse(self, value):
        """Parse a value as a boolean.

        Returns
        -------
        bool
            The parsed boolean value.

        Raises
        ------
        ParserError
            If the value cannot be parsed as a boolean.
        """
        # Allow clients to be compatible with old FIFE versions
        if value in ["0", "False", "false"]:
            value = False
        elif value in ["1", "True", "true"]:
            value = True
        if value not in (True, False):
            raise ParserError(str(self.name) + " expected False or True.")
        return value


class FloatAttr(Attr):
    """Float attribute."""

    def parse(self, value):
        """Parse a value as a float.

        Returns
        -------
        float
            The parsed float value.

        Raises
        ------
        ParserError
            If the value cannot be parsed as a float.
        """
        try:
            return float(value)
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a float.")


class ListAttr(Attr):
    """List attribute (string list)."""

    def parse(self, value):
        """Parse a value as a list.

        Returns
        -------
        list[str]
            The parsed list of strings.

        Raises
        ------
        ParserError
            If the value cannot be parsed as a list of strings.
        """
        try:
            result = list(map(str, str(value).split(",")))
            return result
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a list with strings.")


class UnicodeListAttr(Attr):
    """Unicode list attribute."""

    def parse(self, value):
        """Parse a value as a unicode list.

        Returns
        -------
        list[str]
            The parsed list of unicode strings.

        Raises
        ------
        ParserError
            If the value cannot be parsed as a list of unicode strings.
        """
        try:
            result = list(map(str, str(value).split(",")))
            return result
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a list with unicode strings.")


class IntListAttr(Attr):
    """Integer list attribute."""

    def parse(self, value):
        """Parse a value as an integer list.

        Returns
        -------
        list[int]
            The parsed list of integers.

        Raises
        ------
        ParserError
            If the value cannot be parsed as a list of integers.
        """
        try:
            result = list(map(int, str(value).split(",")))
            return result
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a list with ints.")


class BoolListAttr(Attr):
    """Boolean list attribute."""

    def parse(self, value):
        """Parse a value as a boolean list.

        Returns
        -------
        list[bool]
            The parsed list of booleans.

        Raises
        ------
        ParserError
            If the value cannot be parsed as a list of booleans.
        """
        try:
            result = list(map(bool, str(value).split(",")))
            return result
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a list with bools.")


class FloatListAttr(Attr):
    """Float list attribute."""

    def parse(self, value):
        """Parse a value as a float list.

        Returns
        -------
        list[float]
            The parsed list of floats.

        Raises
        ------
        ParserError
            If the value cannot be parsed as a list of floats.
        """
        try:
            result = list(map(float, str(value).split(",")))
            return result
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a list with floats.")


ATTRIBUTE_TYPE = {
    "Str": Attr,
    "Unicode": UnicodeAttr,
    "Point": PointAttr,
    "Color": ColorAttr,
    "Int": IntAttr,
    "Bool": BoolAttr,
    "Float": FloatAttr,
}


class MixedListAttr(Attr):
    """Mixed list attribute (multiple types)."""

    def parse(self, value):
        """Parse a value as a mixed type list.

        Returns
        -------
        list
            The parsed list containing values of mixed types.

        Raises
        ------
        ParserError
            If the value cannot be parsed as a mixed list.
        """
        try:
            result = []
            # we use ';' to split, because of point and color attributes
            # example: "Int:5; Color:255,0,255"
            tmp_result = map(str, str(value).split(";"))
            for r in tmp_result:
                type = str(r).split(":")[0].strip()
                val = str(r).split(":")[1].strip()

                m = ATTRIBUTE_TYPE.get(type)
                if m is None:
                    raise ParserError("Unknown attribute type " + type + ".")
                method = getattr(m(self), "parse")
                result.append(method(val))
            return result
        except (AttributeError, IndexError, TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a mixed list.")
