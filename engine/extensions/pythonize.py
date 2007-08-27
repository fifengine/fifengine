# coding: utf-8
import fife

__doc__ = """\
Pythonize FIFE

Import this extension to get a more
pythonic interface to FIFE.

Currently it implements the following
conveniences:

* FIFE Exceptions print their message.

"""

fife.Exception.__str__ = fife.Exception.getMessage
