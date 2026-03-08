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

from __future__ import absolute_import

from builtins import map, object, str

from .exceptions import ParserError


class Attr(object):
    """
    A simple text attribute.
    """

    def __init__(self, name):
        self.name = name

    def set(self, obj, value):
        """
        Parses the given value with the L{parse} method
        and sets it on the given instance with C{setattr}.
        """
        value = self.parse(value)
        setattr(obj, self.name, value)

    def parse(self, value):
        """
        Parses a value and checks for errors.
        Override with specialiced behaviour.
        """
        return str(value)


class UnicodeAttr(Attr):
    def parse(self, value):
        """
        Parses a value and checks for errors.
        Override with specialiced behaviour.
        """
        return str(value)


class PointAttr(Attr):
    def parse(self, value):
        try:
            x, y = tuple(map(int, str(value).split(",")))
            return x, y
        except (TypeError, ValueError):
            raise ParserError(
                str(self.name) + " expected a comma separated list of two integers."
            )


class ColorAttr(Attr):
    def parse(self, value):
        a = 255
        try:
            try:
                r, g, b, a = tuple(map(int, str(value).split(",")))
                for c in (r, g, b, a):
                    if not 0 <= c < 256:
                        raise ParserError(
                            str(self.name)
                            + " expected a color (Failed: 0 <= %d <= 255)" % c
                        )
            except ValueError:
                r, g, b = tuple(map(int, str(value).split(",")))
                for c in (r, g, b):
                    if not 0 <= c < 256:
                        raise ParserError(
                            str(self.name)
                            + " expected a color (Failed: 0 <= %d <= 255)" % c
                        )
        except (TypeError, ValueError, ParserError):
            raise ParserError(str(self.name) + " expected a color.")

        return r, g, b, a


class IntAttr(Attr):
    def parse(self, value):
        try:
            return int(value)
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a single integer.")


class BoolAttr(Attr):
    def parse(self, value):
        # Allow clients to be compatible with old FIFE versions
        if value in ["0", "False", "false"]:
            value = False
        elif value in ["1", "True", "true"]:
            value = True
        if value not in (True, False):
            raise ParserError(str(self.name) + " expected False or True.")
        return value


class FloatAttr(Attr):
    def parse(self, value):
        try:
            return float(value)
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a float.")


class ListAttr(Attr):
    def parse(self, value):
        try:
            result = list(map(str, str(value).split(",")))
            return result
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a list with strings.")


class UnicodeListAttr(Attr):
    def parse(self, value):
        try:
            result = list(map(str, str(value).split(",")))
            return result
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a list with unicode strings.")


class IntListAttr(Attr):
    def parse(self, value):
        try:
            result = list(map(int, str(value).split(",")))
            return result
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a list with ints.")


class BoolListAttr(Attr):
    def parse(self, value):
        try:
            result = list(map(bool, str(value).split(",")))
            return result
        except (TypeError, ValueError):
            raise ParserError(str(self.name) + " expected a list with bools.")


class FloatListAttr(Attr):
    def parse(self, value):
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
    def parse(self, value):
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