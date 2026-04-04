"""Fallback fifechan module for test runs without generated GUI bindings."""


class Widget:
    pass


class Icon(Widget):
    pass


class Window(Widget):
    pass


__all__ = ["Widget", "Icon", "Window"]
