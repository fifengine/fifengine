from typing import Any

# This is the typed fife module surface.
# It is a dynamic stub, so it will not be type-checked itself,
# but it provides type information for the fife module.
# It is generated from fife.i.templ and should be expanded as
# the binding surface grows.

class _Dynamic:
    def __init__(self, *args: Any, **kwargs: Any) -> None: ...
    def __getattr__(self, name: str) -> Any: ...

Exception: Any

class Engine(_Dynamic): ...

class LogManager(_Dynamic):
    LEVEL_DEBUG: int

class Key(_Dynamic):
    ESCAPE: int
    F10: int

class TimeEvent(_Dynamic):
    def __init__(self, delay: int = ...) -> None: ...
    def setPeriod(self, delay: int) -> None: ...
    def getPeriod(self) -> int: ...
    def setLastUpdateTime(self, time: int) -> None: ...

class IKeyListener(_Dynamic): ...
class ICommandListener(_Dynamic): ...
class CEGuiManager(_Dynamic): ...

TEXTURE_FILTER_NONE: int
TEXTURE_FILTER_BILINEAR: int
TEXTURE_FILTER_TRILINEAR: int
TEXTURE_FILTER_ANISOTROPIC: int
CMD_QUIT_GAME: int

def __getattr__(name: str) -> Any: ...
