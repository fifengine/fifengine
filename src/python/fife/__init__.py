
import ctypes
import os
from glob import glob
from typing import TYPE_CHECKING, Any, cast


def _candidate_library_dirs(package_dir):
    candidates = [package_dir, os.path.dirname(package_dir)]
    current = package_dir
    while True:
        parent = os.path.dirname(current)
        if parent == current:
            break
        candidates.append(os.path.join(parent, "out", "fife-dependencies", "install", "lib"))
        current = parent
    return candidates


def _preload_optional_gui_backend():
    if os.name != "posix":
        return

    package_dir = os.path.dirname(__file__)
    for library_dir in _candidate_library_dirs(package_dir):
        for library_path in sorted(glob(os.path.join(library_dir, "libfifechan_sdl2.so*"))):
            try:
                ctypes.CDLL(library_path, mode=ctypes.RTLD_GLOBAL)
                return
            except OSError:
                continue


_preload_optional_gui_backend()

from . import fife

if TYPE_CHECKING:
    from . import fifechan as _fifechan
else:
    try:
        from . import fifechan as _fifechan
    except ImportError:
        _fifechan = None

fifechan: Any = cast(Any, _fifechan)

__all__ = ["fife", "fifechan"]
