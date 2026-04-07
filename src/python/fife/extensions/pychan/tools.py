# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
Functional utilities designed for pychan use cases.
"""

from . import exceptions

### Functools ###


def applyOnlySuitable(func, *args, **kwargs):
    """
    This function takes another function and applies it to a dictionary of
    keyword arguments. If the supplied function does not expect one or more of the
    keyword arguments, these are silently discarded. The result of the application is returned.
    This is useful to pass information to callbacks without enforcing a particular signature.
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
    Curries a function with extra arguments to
    create a suitable callback.

    If you don't know what this means, don't worry.
    It is designed for the case where you need
    different buttons to execute basically the same code
    with different arguments.

    Usage::
      # The target callback
      def printStuff(text):
          print text
      # Mapping the events
      gui.mapEvents({
          'buttonHello' : callbackWithArguments(printStuff,"Hello"),
          'buttonBye' : callbackWithArguments(printStuff,"Adieu")
      })
    """

    def real_callback():
        callback(*args, **kwargs)

    return real_callback


def attrSetCallback(**kwargs):
    """
    Generates an event callback that sets attributes on the widget
    it is called on. This is especially useful for mouseEntered/Exited
    events - to create hover effects.

    It takes a set of keyword arguments. The keys are treated as attribute names,
    which are then set to the corresponding value when the callback is called.
    Some key names are treated special - see below.

    Usage - Example adapted from demo application::
            eventMap = {
                    'creditsLink'  : showCreditsCallback,
                    'creditsLink/mouseEntered' : attrSetCallback(
                          text = "Show credits!",
                          background_color = (255,255,0,255),
                          do__adaptLayout=True),
                    'creditsLink/mouseExited'  : attrSetCallback(text = "Credits"),
            gui.mapEvents(eventMap)

    Now when the mouse enters the creditsLink (a Label in our case), the following code will be executed::
            #widget is the creditsLink - given to the event callback.
            widget.text = "Show credits!"
            widget.background_color = (255,255,0,255)
            widget.adaptLayout()

    The C{do__adaptLayout} argument causes the method C{adaptLayout} to be called.
    In fact any key starting with C{do__} results in such a method call. The order
    of execution of such calls is undefined.

    Keys starting with an underscore raise a L{exceptions.PrivateFunctionalityError}.
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

    Example Usage::
        def print_event(event=0):
          print event
        def print_widget(widget=0):
          print widget
        callback = tools.chainCallbacks(doSomethingUseful, print_event, print_widget)
        guiElement.capture(callback)
    """
    callbacks = args

    def chain_callback(event=0, widget=0):
        for callback in callbacks:
            applyOnlySuitable(callback, event=event, widget=widget)

    return chain_callback


def repeatALot(n=1000):
    """
    Internal decorator used to profile some pychan functions.
    Only use with functions without side-effect.

    Usage::
            @repeatALot(n=10000)
            def findChild(self,**kwargs):
                    ...
    """

    def wrap_f(f):
        def new_f(*args, **kwargs):
            for i in range(n):
                f(*args, **kwargs)
            return f(*args, **kwargs)

        return new_f

    return wrap_f


def this_is_deprecated(func, message=None):
    if message is None:
        message = repr(func)

    def wrapped_func(*args, **kwargs):
        print(f"PyChan: You are using the DEPRECATED functionality: {message}")
        return func(*args, **kwargs)

    return wrapped_func
