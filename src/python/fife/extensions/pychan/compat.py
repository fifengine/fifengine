# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import inspect
from builtins import object, str
from typing import Any

in_fife = None
fife: Any = None
fifechan: Any = None


def _import_fifechan():
    global in_fife

    err_fife = ""
    try:
        from fife import fife, fifechan

        in_fife = True

        # move fife custom widgets to fifechan module
        for member in inspect.getmembers(fife, inspect.isclass):
            if issubclass(member[1], fifechan.Widget):
                setattr(fifechan, member[0], member[1])

        return fife, fifechan
    except ImportError as e:
        err_fife = str(e)

    try:
        import fifechan

        in_fife = False
        return None, fifechan
    except ImportError as e:
        import traceback

        traceback.print_exc()
        raise ImportError(
            "Couldn't import neither fife nor fifechan: fife:'%s' fifechan:'%s'"
            % (err_fife, str(e))
        )


fife, fifechan = _import_fifechan()


def _munge_engine_hook(engine):
    engine.translate_mouse_event = getattr(engine, "translate_mouse_event", lambda x: x)
    engine.translate_key_event = getattr(engine, "translate_key_event", lambda x: x)

    if not in_fife:
        return engine
    if not isinstance(engine, fife.Engine):
        return engine

    guimanager = fife.FifechanManager()
    guimanager.thisown = 0

    engine.setGuiManager(guimanager)

    guimanager.setDefaultFont(
        engine.getSettings().getDefaultFontPath(),
        engine.getSettings().getDefaultFontSize(),
        engine.getSettings().getDefaultFontGlyphs(),
    )

    guimanager.init(
        engine.getRenderBackend().getName(),
        engine.getRenderBackend().getScreenWidth(),
        engine.getRenderBackend().getScreenHeight(),
    )

    guimanager.setCursor(engine.getCursor())

    engine.getEventManager().addSdlEventListener(guimanager)

    def _fife_load_image(filename, gui=True):
        img = engine.getImageManager().load(filename)
        if gui:
            return fife.GuiImage(img)
        return img
        # use below line instead of above ones to let fifechan
        # use its image loader that supports creating/using atlases
        # return fifechan.GuiImage().load(filename)

    class hook(object):
        pass

    hook = hook()

    hook.add_widget = guimanager.add
    hook.remove_widget = guimanager.remove
    hook.default_font = guimanager.getDefaultFont()
    hook.load_image = _fife_load_image
    hook.translate_mouse_event = guimanager.translateMouseEvent
    hook.translate_key_event = guimanager.translateKeyEvent
    hook.guimanager = guimanager
    hook.console = guimanager.getConsole()
    hook.create_font = guimanager.createFont
    hook.release_font = guimanager.releaseFont

    hook.screen_width = engine.getRenderBackend().getScreenWidth()
    hook.screen_height = engine.getRenderBackend().getScreenHeight()

    hook.engine = engine
    return hook


class _multilistener(
    fifechan.ActionListener, fifechan.MouseListener, fifechan.KeyListener
):
    def __init__(self):
        fifechan.ActionListener.__init__(self)
        fifechan.MouseListener.__init__(self)
        fifechan.KeyListener.__init__(self)


class _point(object):
    def __init__(self, x=0, y=0):
        self.x = 0
        self.y = 0


if in_fife:
    # fife = fifechan
    fifechan.ActionListener._ActionListener_init__ = lambda x: x
    # fifechan.MouseListener.__init__ = lambda x : x
    # fifechan.KeyListener.__init__ = lambda x : x
else:
    fifechan.Point = _point
    fifechan.ScrollArea.SHOW_AUTO = fifechan.ScrollArea.ShowAuto
    fifechan.ScrollArea.SHOW_NEVER = fifechan.ScrollArea.ShowNever
    fifechan.ScrollArea.SHOW_ALWAYS = fifechan.ScrollArea.ShowAlways

assert isinstance(_multilistener(), fifechan.ActionListener)
assert isinstance(_multilistener(), fifechan.MouseListener)
assert isinstance(_multilistener(), fifechan.KeyListener)