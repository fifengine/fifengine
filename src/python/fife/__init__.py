
from typing import TYPE_CHECKING, Any, cast

from . import fife

if TYPE_CHECKING:
    import fifechan as _fifechan
else:
    try:
        import fifechan as _fifechan
    except ImportError:
        _fifechan = None

fifechan: Any = cast(Any, _fifechan)

__all__ = ["fife", "fifechan"]
