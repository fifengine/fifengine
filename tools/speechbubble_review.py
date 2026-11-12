#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Render FIFE speech bubbles and write PNGs for visual review.

Covers both bubble paths:

  FloatingTextRenderer  say() barks, via a per-instance SpeechStyle
  fcn::SpeechBubble     the interactive quest dialog, via pychan

Usage:
    ./build-tools/visual-check.py                 # every shot
    ./build-tools/visual-check.py barks detail    # named shots only

Output lands in $FIFE_VISUAL_OUT (default /tmp/fife-visual).

Requirements:
  - A live display and an OpenGL capable driver. This cannot run headless,
    and it cannot be run in CI: there is no GL context there.
  - The python bindings built and prepared, e.g. via run_tests.py -a or
    demos/run_demo_rpg.sh, which copy them into the package directory.
  - PYTHONPATH pointing at the prepared package, and LD_LIBRARY_PATH at the
    build directory and the fifechan install (both scripts set those up).

Why this exists: the unit tests assert the bubble geometry and that rendering
does not crash, and neither can see a wrong colour, an inverted triangle or a
back-face culled primitive. Several such defects only showed up once somebody
looked at the output, so run this after touching the drawing code.
"""

import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ORDER = ["barks", "detail", "round", "thought", "classic", "both"]

# Validate the shot names before importing the bindings, so a typo reports the
# usage rather than an ImportError.
_unknown = [arg for arg in sys.argv[1:] if arg not in ORDER]
if _unknown:
    print(f"unknown shot(s): {', '.join(_unknown)}")
    print(f"available: {', '.join(ORDER)}")
    sys.exit(2)

from fife.extensions import pychan  # noqa: E402

from fife import fife  # noqa: E402

REPO = os.environ.get("REPO", os.path.dirname(HERE))
DEMO = os.path.join(REPO, "demos", "rpg")
QUEST_XML = os.path.join(DEMO, "gui", "quest.xml")
OUT = os.environ.get("FIFE_VISUAL_OUT", "/tmp/fife-visual")

# The rpg demo ships its own font; tests/data is only used for the bark shots.
DEMO_FONT = "fonts/FreeSans.ttf"
TEST_FONT = "tests/data/FreeMono.ttf"

FONT_MANIFEST = """<?xml version="1.0"?>
<fonts version="1">
  <family id="{family}">
    <face type="truetype" weight="400" size="{size}" antialias="true" color="255,255,255">{path}</face>
  </family>
</fonts>"""


def manifest(family, size, path):
    """
    Build a font manifest for one family.

    Returns
    -------
    str
        The manifest XML.
    """
    return FONT_MANIFEST.format(family=family, size=size, path=path)


def engine(width=1024, height=768, font=None):
    """
    Create and initialise an engine on the default backend.

    Returns
    -------
    fife.Engine
        The initialised engine.
    """
    e = fife.Engine()
    e.thisown = False
    s = e.getSettings()
    s.setRenderBackend("OpenGL")
    s.setWindowTitle("fife visual check")
    s.setScreenWidth(width)
    s.setScreenHeight(height)
    s.setFullScreen(False)
    s.setDefaultFontSize(16)
    s.setDefaultFontGlyphs(
        " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        ".,!?-+/:();%`'*#=[]\""
    )
    if font is not None:
        s.setDefaultFontPath(font)
    e.init()
    return e


def world(e, rotation=45, tilt=60):
    """Build a map with a layer of tiles and an enabled camera.

    Returns
    -------
    tuple
        The map, the layer, the object to instantiate and the camera.
    """
    model = e.getModel()
    map_obj = model.createMap("visual_map")
    grid = model.getCellGrid("square")

    obj = model.createObject("tile", "visual")
    fife.ObjectVisual.create(obj)
    img = e.getImageManager().load(TEST_FONT.rsplit("/", 1)[0] + "/earth_1.png")
    obj.get2dGfxVisual().addStaticImage(0, img.getHandle())

    layer = map_obj.createLayer("layer001", grid)

    rb = e.getRenderBackend()
    vp = fife.Rect(0, 0, rb.getWidth(), rb.getHeight())
    cam = map_obj.addCamera("visual_cam", vp)
    cam.setCellImageDimensions(img.getWidth(), img.getHeight())
    loc = fife.Location(layer)
    c = loc.getExactLayerCoordinates()
    c.x += 3
    c.y += 1
    loc.setExactLayerCoordinates(c)
    cam.setLocation(loc)
    cam.setViewPort(vp)
    # Map::update() only renders enabled cameras, and RendererBase::render only
    # runs for layers the renderer is active on. Miss either and nothing draws.
    cam.setEnabled(True)
    cam.setRotation(rotation)
    cam.setTilt(tilt)
    return map_obj, layer, obj, cam


def pump(e, frames=4):
    """Draw a few frames so the renderers run."""
    e.initializePumping()
    for _ in range(frames):
        e.pump()
    e.finalizePumping()


def style(**kwargs):
    """
    Build a CLASSIC SpeechStyle.

    Returns
    -------
    fife.SpeechStyle
        The style, with the given fields overridden.
    """
    st = fife.SpeechStyle.classic()
    for key, value in kwargs.items():
        setattr(st, key, value)
    return st


def bubble(colour, alpha=235, border=None, **kwargs):
    """
    Build a SpeechStyle with a background and optionally a border.

    Returns
    -------
    fife.SpeechStyle
        The configured style.
    """
    st = style(**kwargs)
    st.hasBackgroundOverride = True
    st.backgroundColorOverride = fife.Color(colour[0], colour[1], colour[2], alpha)
    if border is not None:
        st.hasBorderOverride = True
        st.borderColorOverride = fife.Color(border[0], border[1], border[2], 255)
    return st


def say(layer, obj, renderer, col, row, text, st=None):
    """
    Place an instance that says text, optionally with its own style.

    Returns
    -------
    fife.Instance
        The new instance.
    """
    inst = layer.createInstance(obj, fife.ModelCoordinate(col, row, 0))
    fife.InstanceVisual.create(inst)
    inst.say(text, 600000)
    if st is not None:
        renderer.setSpeechStyle(inst, st)
    return inst


def text_renderer(e, layer, cam, family, size):
    """
    Enable a FloatingTextRenderer drawing with the given font.

    Returns
    -------
    fife.FloatingTextRenderer
        The enabled renderer.
    """
    renderer = fife.FloatingTextRenderer.getInstance(cam)
    renderer.addActiveLayer(layer)
    renderer.setFont(e.getFontManager().getFont(family, size))
    renderer.setEnabled(True)
    return renderer


def capture(e, name):
    """Write a PNG of the current frame."""
    e.getRenderBackend().captureScreen(os.path.join(OUT, name))
    print(name)


def shot_barks():
    """say() barks: default style, per-instance styles, no-bubble case."""
    os.chdir(REPO)
    e = engine()
    e.loadFontManifestFromString(manifest("Shot", 15, TEST_FONT))
    _, layer, obj, cam = world(e)
    r = text_renderer(e, layer, cam, "Shot", 15)
    r.setDefaultSpeechStyle(style())

    say(layer, obj, r, 0, 0, "no bubble at all")
    say(layer, obj, r, 2, 0, "default style", bubble((70, 110, 190)))
    say(
        layer,
        obj,
        r,
        4,
        0,
        "border + wrap",
        bubble((200, 90, 90), border=(255, 240, 240), maxTextWidth=150),
    )
    say(
        layer,
        obj,
        r,
        0,
        2,
        "align center",
        bubble((110, 180, 130), maxTextWidth=170, textAlign=fife.TextAlign_CENTER),
    )
    say(
        layer,
        obj,
        r,
        2,
        2,
        "align right",
        bubble((190, 160, 90), maxTextWidth=170, textAlign=fife.TextAlign_RIGHT),
    )
    say(layer, obj, r, 4, 2, "corner r=0", bubble((150, 150, 160), cornerRadius=0))
    say(layer, obj, r, 0, 4, "corner r=20", bubble((120, 110, 170), cornerRadius=20))
    say(layer, obj, r, 2, 4, "translucent", bubble((220, 120, 60), alpha=110))
    say(layer, obj, r, 4, 4, "multiline text\nsecond line", bubble((90, 160, 170)))

    pump(e)
    capture(e, "01-barks.png")


def shot_detail():
    """Tail directions and tail offsets, at a size where the geometry is clear."""
    os.chdir(REPO)
    e = engine(1320, 720)
    e.loadFontManifestFromString(manifest("Shot", 22, TEST_FONT))
    _, layer, obj, cam = world(e)
    r = text_renderer(e, layer, cam, "Shot", 22)
    r.setDefaultSpeechStyle(style())

    big = {"bubblePadding": 14, "tailSize": 26}
    say(
        layer,
        obj,
        r,
        0,
        0,
        "tail DOWN",
        bubble(
            (70, 110, 190), border=(255, 255, 255), tailDir=fife.TailDirection_DOWN, **big
        ),
    )
    say(
        layer,
        obj,
        r,
        5,
        0,
        "tail UP",
        bubble(
            (190, 110, 70), border=(255, 255, 255), tailDir=fife.TailDirection_UP, **big
        ),
    )
    say(
        layer,
        obj,
        r,
        0,
        4,
        "tail LEFT",
        bubble(
            (70, 170, 120), border=(255, 255, 255), tailDir=fife.TailDirection_LEFT, **big
        ),
    )
    say(
        layer,
        obj,
        r,
        5,
        4,
        "tail RIGHT",
        bubble(
            (170, 70, 150),
            border=(255, 255, 255),
            tailDir=fife.TailDirection_RIGHT,
            **big,
        ),
    )
    say(
        layer,
        obj,
        r,
        8,
        0,
        "tail AUTO",
        bubble(
            (95, 95, 105), border=(255, 255, 255), tailDir=fife.TailDirection_AUTO, **big
        ),
    )
    say(
        layer,
        obj,
        r,
        8,
        4,
        "tail offset",
        bubble((60, 130, 130), border=(255, 255, 255), tailOffset=22, **big),
    )

    pump(e)
    capture(e, "02-barks-detail.png")


def gui(e):
    """Start pychan and point the GUI at the demo font."""
    pychan.init(e)
    # The GUI needs an explicit default font family, as the demo sets.
    pychan.manager.hook.guimanager.setDefaultFont("FreeSans", 16)


def shot_widget(bubble_style, tail_direction, tail_profile, name):
    """Render the rpg quest dialog as a SpeechBubble in the given style."""
    os.chdir(DEMO)
    e = engine(900, 620, font=DEMO_FONT)
    e.loadFontManifestFromString(manifest("FreeSans", 16, DEMO_FONT))
    gui(e)

    root = pychan.loadXML(QUEST_XML)
    bubble_widget = root.findChild(name="questbubble")
    bubble_widget.bubble_style = bubble_style
    bubble_widget.tail_direction = tail_direction
    bubble_widget.tail_profile = tail_profile
    root.findChild(name="questtext").text = (
        "Please bring me ten copper coins and I will reward you kindly."
    )
    root.show()

    pump(e, 5)
    capture(e, name)


def shot_both():
    """Barks and the widget dialog in one frame."""
    os.chdir(DEMO)
    e = engine(1024, 768, font=DEMO_FONT)
    e.loadFontManifestFromString(manifest("FreeSans", 16, DEMO_FONT))
    e.loadFontManifestFromString(manifest("Shot", 15, DEMO_FONT))

    # world() loads from tests/data, which is repo-root relative.
    os.chdir(REPO)
    _, layer, obj, cam = world(e)
    os.chdir(DEMO)
    r = text_renderer(e, layer, cam, "Shot", 15)
    r.setDefaultSpeechStyle(style())

    say(
        layer, obj, r, 2, 0, "Hello there!", bubble((200, 90, 90), border=(255, 240, 240))
    )
    say(
        layer,
        obj,
        r,
        2,
        3,
        "Mind the gap.",
        bubble((90, 130, 200), border=(240, 245, 255)),
    )
    say(layer, obj, r, 6, 1, "no bubble")

    gui(e)
    root = pychan.loadXML(QUEST_XML)
    quest_bubble = root.findChild(name="questbubble")
    quest_bubble.position = (300, 400)
    quest_bubble.bubble_style = "round"
    quest_bubble.tail_direction = "TopLeft"
    root.findChild(name="questtext").text = (
        "Please bring me ten copper coins and I will reward you kindly."
    )
    root.show()

    pump(e, 6)
    capture(e, "06-both-together.png")


SHOTS = {
    "barks": shot_barks,
    "detail": shot_detail,
    "round": lambda: shot_widget("round", "BottomLeft", "rounded", "03-widget-round.png"),
    "thought": lambda: shot_widget(
        "thought", "BottomRight", "jagged", "04-widget-thought.png"
    ),
    "classic": lambda: shot_widget(
        "classic", "BottomRight", "sharp", "05-widget-classic.png"
    ),
    "both": shot_both,
}


def main(names):
    """
    Run each requested shot in its own process.

    FIFE keeps a TimeManager singleton per process, so two engines cannot
    coexist in one interpreter.

    Returns
    -------
    int
        0 on success, 1 if a shot failed.
    """
    os.makedirs(OUT, exist_ok=True)
    for name in ORDER:
        if name not in names:
            continue
        print(f"--- {name}")
        result = subprocess.run(
            [sys.executable, os.path.abspath(__file__), name],
            capture_output=True,
            text=True,
            timeout=300,
        )
        for line in result.stdout.splitlines():
            if line.strip():
                print("   ", line)
        if result.returncode != 0:
            print(f"FAILED {name} (exit {result.returncode})")
            print(result.stderr[-2000:])
            return 1
    print(f"\nPNGs in {OUT}")
    return 0


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] in SHOTS:
        SHOTS[sys.argv[1]]()
    else:
        sys.exit(main(sys.argv[1:] or ORDER))
