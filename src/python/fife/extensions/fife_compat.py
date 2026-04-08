# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""
FIFE Backwards Compatibility Layer.

This module can be imported if you want to
run code that wasn't adapted to API changes in FIFE.

2008.1
------

 - Animation.addFrame now expects a fife.ResourcePtr instead of an fife.Image
 - Pool.getIndex is just an alias for Pool.addResourceFromFile.
 - EventManager.setNonConsumableKeys is superseded by EventManager.setKeyFilter
"""

from fife import fife

# Utility functions


def deprecated(revision, message):
    print(f"fife_compat: Deprecation warning - See revision {revision} ")
    print(" - ", message)


def this_is_deprecated(func, revision=0, message=None):
    if message is None:
        message = repr(func) + " is deprecated."

    def wrapped_func(*args, **kwargs):
        deprecated(revision, message)
        return func(*args, **kwargs)

    return wrapped_func


def _compat_NonConsumableKeys():
    class CompatKeyFilter(fife.IKeyFilter):
        def __init__(self, keys):
            fife.IKeyFilter.__init__(self)
            self.keys = keys

        def isFiltered(self, event):
            return event.getKey().getValue() in self.keys

    def _setNonConsumableKeys(self, keys):
        deprecated(
            2636,
            "Write an IKeyFilter instead of using EventManager.setNonConsumableKeys.\n"
            + "You probably don't need it anyway",
        )
        self.compat_keyfilter = CompatKeyFilter(keys)
        self.compat_keyfilter.__disown__()
        self.setKeyFilter(self.compat_keyfilter)

    def _getNonConsumableKeys(self, keys):
        deprecated(
            2636,
            "Write an IKeyFilter instead of using EventManager.getNonConsumableKeys.",
        )
        return self.compat_keyfilter.keys

    fife.EventManager.setNonConsumableKeys = _setNonConsumableKeys
    fife.EventManager.getNonConsumableKeys = _getNonConsumableKeys


_compat_NonConsumableKeys()
