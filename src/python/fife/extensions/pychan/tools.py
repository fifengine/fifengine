# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""Functional utilities designed for pychan use cases."""

from . import exceptions

# Functools ###


def applyOnlySuitable(func, *args, **kwargs):
    """
    Apply a function to arguments, filtering out unsuitable keyword arguments.

    This function takes another function and applies it to a dictionary of
    keyword arguments. If the supplied function does not expect one or more of the
    keyword arguments, these are silently discarded. The result of the application is returned.
    This is useful to pass information to callbacks without enforcing a particular signature.

    Returns
    -------
    Any
        The return value of `func(*args, **filtered_kwargs)`.
    """
    func_name = "__func__"
    code_name = "__code__"
    if hasattr(func, func_name):
        code = func.__func__.__code__
        varnames = code.co_varnames[1 : code.co_argcount]  # ditch bound instance
    elif hasattr(func, code_name):
        code = func.__code__
        varnames = code.co_varnames[0 : code.co_argcount]
    elif hasattr(func, "__call__"):
        func = func.__call__
        if hasattr(func, func_name):
            code = func.__func__.__code__
            varnames = code.co_varnames[1 : code.co_argcount]  # ditch bound instance
        elif hasattr(func, code_name):
            code = func.__code__
            varnames = code.co_varnames[0 : code.co_argcount]

    # http://docs.python.org/lib/inspect-types.html
    if code.co_flags & 8:
        return func(*args, **kwargs)
    for name, value in list(kwargs.items()):
        if name not in varnames:
            del kwargs[name]
    return func(*args, **kwargs)


def callbackWithArguments(callback, *args, **kwargs):
    """
    Curry a function with extra arguments to create a suitable callback.

    If you don't know what this means, don't worry.
    It is designed for the case where you need
    different buttons to execute basically the same code
    with different arguments.

    Returns
    -------
    callable
        A zero-argument callable that, when invoked, calls `callback(*args, **kwargs)`.
    """

    def real_callback():
        callback(*args, **kwargs)

    return real_callback


def attrSetCallback(**kwargs):
    """
    Generate an event callback that sets attributes on the widget.

    This is especially useful for mouseEntered/Exited
    events - to create hover effects.

    Returns
    -------
    callable
        A single-argument callable `callback(widget)` that sets the provided
        attributes on `widget` and calls any `do__*` methods.

    Raises
    ------
    exceptions.PrivateFunctionalityError
        If any provided kwarg starts with an underscore.
    """
    do_calls = []

    for name in list(kwargs.keys()):
        if name.startswith("_"):
            raise exceptions.PrivateFunctionalityError(name)
        if name.startswith("do__"):
            do_calls.append(name[4:])
            del kwargs[name]

    def attrSet_callback(widget=None):
        for name, value in list(kwargs.items()):
            setattr(widget, name, value)
        for method_name in do_calls:
            method = getattr(widget, method_name)
            method()

    return attrSet_callback


def chainCallbacks(*args):
    """
    Chains callbacks to be called one after the other.

    Returns
    -------
    callable
        A callable `callback(event=0, widget=0)` that executes each provided
        callback in order with suitable arguments.
    """
    callbacks = args

    def chain_callback(event=0, widget=0):
        for callback in callbacks:
            applyOnlySuitable(callback, event=event, widget=widget)

    return chain_callback


def repeatALot(n=1000):
    """
    Profile functions by running them many times.

    Returns
    -------
    callable
        A decorator that replaces the target function with a wrapper that
        executes it `n` times for profiling and returns the function result.
    """

    def wrap_f(f):
        def new_f(*args, **kwargs):
            for i in range(n):
                f(*args, **kwargs)
            return f(*args, **kwargs)

        return new_f

    return wrap_f


def this_is_deprecated(func, message=None):
    """
    Mark a function as deprecated.

    Wraps a function to print a deprecation warning when called.

    Parameters
    ----------
    func : callable
        The function to mark as deprecated.
    message : str, optional
        Custom deprecation message.

    Returns
    -------
    callable
        The wrapped function.
    """
    if message is None:
        message = repr(func)

    def wrapped_func(*args, **kwargs):
        print(f"PyChan: You are using the DEPRECATED functionality: {message}")
        return func(*args, **kwargs)

    return wrapped_func
