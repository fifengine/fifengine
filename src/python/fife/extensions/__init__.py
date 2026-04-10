"""FIFE extensions package."""

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
try:
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
except Exception:
    # Avoid import-time failures in analysis tools or partial installs.
    pass
