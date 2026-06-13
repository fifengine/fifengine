"""FIFE extensions package."""

import contextlib

__all__ = [
    "basicapplication",
    "fife_compat",
    "fife_timer",
    "fife_utils",
    "fife_settings",
    "fifelog",
    "pythonize",
    "savers",
    "loaders",
    "pychan",
]

# Import common extension modules so names in `__all__` are defined for
# tools that check exports (and to provide a stable package API).
with contextlib.suppress(Exception):
    from . import (
        basicapplication,
        fife_compat,
        fife_settings,
        fife_timer,
        fife_utils,
        fifelog,
        loaders,
        pychan,
        pythonize,
        savers,
    )
