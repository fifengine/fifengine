# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Tests for the per-instance SpeechStyle registry of the FloatingTextRenderer."""

import pytest

from fife import fife


def _make_renderer(engine):
    """
    Create a map, a camera and a FloatingTextRenderer.

    Returns
    -------
    tuple
        The renderer, the layer and the object to instantiate.
    """
    model = engine.getModel()
    map_obj = model.createMap("speechstyle_map")

    grid = model.getCellGrid("square")
    img_mgr = engine.getImageManager()

    obj = model.createObject("0", "speechstyle_nspace")
    fife.ObjectVisual.create(obj)
    img = img_mgr.load("tests/data/earth_1.png")
    obj.get2dGfxVisual().addStaticImage(0, img.getHandle())

    layer = map_obj.createLayer("layer001", grid)

    rb = engine.getRenderBackend()
    viewport = fife.Rect(0, 0, rb.getWidth(), rb.getHeight())
    cam = map_obj.addCamera("speechstyle_cam", viewport)
    cam.setCellImageDimensions(img.getWidth(), img.getHeight())
    cam.setLocation(fife.Location(layer))
    cam.setViewPort(viewport)
    # Map::update() only renders enabled cameras.
    cam.setEnabled(True)

    renderer = fife.FloatingTextRenderer.getInstance(cam)
    assert renderer is not None
    # render() is only invoked for layers the renderer is active on.
    renderer.addActiveLayer(layer)
    return renderer, layer, obj


def _make_instance(layer, obj, x=0, y=0):
    instance = layer.createInstance(obj, fife.ModelCoordinate(x, y))
    fife.InstanceVisual.create(instance)
    return instance


def _style(**kwargs):
    style = fife.SpeechStyle.classic()
    for key, value in kwargs.items():
        setattr(style, key, value)
    return style


def test_speech_style_defaults(engine):
    style = fife.SpeechStyle()

    assert style.bubbleType == fife.BubbleType_NONE
    assert style.bubblePadding == 5
    assert style.cornerRadius == 8
    assert style.tailSize == 10
    assert style.tailOffset == 0
    assert style.tailDir == fife.TailDirection_DOWN
    assert style.maxTextWidth == 0
    assert style.textAlign == fife.TextAlign_LEFT
    assert style.fontOverride is None
    assert style.hasTextColorOverride is False
    assert style.hasBackgroundOverride is False
    assert style.hasBorderOverride is False


def test_speech_style_factories(engine):
    assert fife.SpeechStyle.plain().bubbleType == fife.BubbleType_NONE
    assert fife.SpeechStyle.classic().bubbleType == fife.BubbleType_CLASSIC


def test_bubble_type_names_mirror_the_fifechan_widget(engine):
    """FIFE::BubbleType and fcn::SpeechBubble::BubbleStyle must stay in step.

    The names and their order are shared. The integer values deliberately are
    not: FIFE has an extra NONE sentinel that the widget does not need, since
    the widget expresses "no tail" through TailDirection::None instead.
    """
    from fife import fifechan

    fife_names = ["CLASSIC", "ROUND", "THOUGHT", "SHOUT", "WHISPER"]
    widget_names = ["Classic", "Round", "Thought", "Shout", "Whisper"]
    assert [n.upper() for n in widget_names] == fife_names

    for name in fife_names:
        assert hasattr(fife, f"BubbleType_{name}"), name
        assert hasattr(fifechan.SpeechBubble, f"BubbleStyle_{name.title()}"), name

    # FIFE keeps NONE as the "draw no bubble" sentinel ahead of the styles.
    assert fife.BubbleType_NONE == 0
    for offset, name in enumerate(fife_names, start=1):
        assert getattr(fife, f"BubbleType_{name}") == offset


@pytest.mark.parametrize(
    "bubble_type",
    [
        fife.BubbleType_ROUND,
        fife.BubbleType_THOUGHT,
        fife.BubbleType_SHOUT,
        fife.BubbleType_WHISPER,
    ],
)
def test_unimplemented_bubble_types_fall_back_to_classic(engine, bubble_type):
    """Styles the renderer cannot draw yet must still render as CLASSIC."""
    family = _ensure_font_family(engine)
    if family is None:
        pytest.skip("no font family could be registered")

    renderer, layer, obj = _make_renderer(engine)
    style = _style(hasBackgroundOverride=True)
    style.bubbleType = bubble_type
    style.backgroundColorOverride = fife.Color(120, 130, 140, 200)
    renderer.setDefaultSpeechStyle(style)
    renderer.setFont(engine.getFontManager().getFont(family, 14))
    renderer.setEnabled(True)

    # The style is stored verbatim; the fallback happens at draw time.
    assert renderer.getDefaultSpeechStyle().bubbleType == bubble_type

    instance = _make_instance(layer, obj)
    instance.say("Fallback", 2000)

    _render_once(engine)


def test_speech_style_color_overrides(engine):
    style = fife.SpeechStyle.classic()
    style.hasBackgroundOverride = True
    style.backgroundColorOverride = fife.Color(10, 20, 30, 40)
    style.hasBorderOverride = True
    style.borderColorOverride = fife.Color(50, 60, 70, 80)
    style.hasTextColorOverride = True
    style.textColorOverride = fife.Color(90, 100, 110, 120)

    bg = style.backgroundColorOverride
    assert (bg.r, bg.g, bg.b, bg.a) == (10, 20, 30, 40)
    border = style.borderColorOverride
    assert (border.r, border.g, border.b, border.a) == (50, 60, 70, 80)
    text = style.textColorOverride
    assert (text.r, text.g, text.b, text.a) == (90, 100, 110, 120)


def test_default_style_lookup(engine):
    renderer, layer, obj = _make_renderer(engine)
    default = _style(cornerRadius=12, tailDir=fife.TailDirection_LEFT)
    renderer.setDefaultSpeechStyle(default)

    assert renderer.getDefaultSpeechStyle().cornerRadius == 12

    instance = _make_instance(layer, obj)
    effective = renderer.getEffectiveStyle(instance)
    assert effective.cornerRadius == 12
    assert effective.tailDir == fife.TailDirection_LEFT


def test_per_instance_style_overrides_default(engine):
    renderer, layer, obj = _make_renderer(engine)
    renderer.setDefaultSpeechStyle(_style(cornerRadius=12))

    styled = _make_instance(layer, obj, 0, 0)
    plain = _make_instance(layer, obj, 1, 1)

    renderer.setSpeechStyle(styled, _style(cornerRadius=20, tailSize=30))

    assert renderer.getEffectiveStyle(styled).cornerRadius == 20
    assert renderer.getEffectiveStyle(styled).tailSize == 30
    # Unstyled instances keep falling back to the default.
    assert renderer.getEffectiveStyle(plain).cornerRadius == 12


def test_clear_speech_style_restores_default(engine):
    renderer, layer, obj = _make_renderer(engine)
    renderer.setDefaultSpeechStyle(_style(cornerRadius=12))

    instance = _make_instance(layer, obj)
    renderer.setSpeechStyle(instance, _style(cornerRadius=20))
    assert renderer.getEffectiveStyle(instance).cornerRadius == 20

    renderer.clearSpeechStyle(instance)
    assert renderer.getEffectiveStyle(instance).cornerRadius == 12


def test_clear_all_styles(engine):
    renderer, layer, obj = _make_renderer(engine)
    renderer.setDefaultSpeechStyle(_style(cornerRadius=12))

    first = _make_instance(layer, obj, 0, 0)
    second = _make_instance(layer, obj, 1, 1)
    renderer.setSpeechStyle(first, _style(cornerRadius=20))
    renderer.setSpeechStyle(second, _style(cornerRadius=30))

    assert renderer.getEffectiveStyle(first).cornerRadius == 20
    assert renderer.getEffectiveStyle(second).cornerRadius == 30

    renderer.clearAllStyles()

    assert renderer.getEffectiveStyle(first).cornerRadius == 12
    assert renderer.getEffectiveStyle(second).cornerRadius == 12


def test_multiple_instances_register_independently(engine):
    """Every styled instance must be tracked, not just the first one."""
    renderer, layer, obj = _make_renderer(engine)
    renderer.setDefaultSpeechStyle(_style(cornerRadius=12))

    instances = [_make_instance(layer, obj, x, 0) for x in range(4)]
    for index, instance in enumerate(instances):
        renderer.setSpeechStyle(instance, _style(cornerRadius=20 + index))

    for index, instance in enumerate(instances):
        assert renderer.getEffectiveStyle(instance).cornerRadius == 20 + index

    renderer.clearAllStyles()
    for instance in instances:
        assert renderer.getEffectiveStyle(instance).cornerRadius == 12


def test_style_survives_instance_destruction(engine):
    """Destroying a styled instance must drop its style, not dangle."""
    renderer, layer, obj = _make_renderer(engine)
    renderer.setDefaultSpeechStyle(_style(cornerRadius=12))

    instance = _make_instance(layer, obj)
    renderer.setSpeechStyle(instance, _style(cornerRadius=20))
    assert renderer.getEffectiveStyle(instance).cornerRadius == 20

    layer.removeInstance(instance)

    survivor = _make_instance(layer, obj, 1, 1)
    renderer.setSpeechStyle(survivor, _style(cornerRadius=30))
    assert renderer.getEffectiveStyle(survivor).cornerRadius == 30


def test_set_speech_style_ignores_none(engine):
    renderer, layer, obj = _make_renderer(engine)
    renderer.setSpeechStyle(None, _style(cornerRadius=20))
    # Must not raise; the default is untouched.
    assert renderer.getDefaultSpeechStyle().cornerRadius == 8


FONT_MANIFEST = """<?xml version="1.0"?>
<fonts version="1">
  <family id="SpeechTest">
    <face weight="400">tests/data/FreeMono.ttf</face>
  </family>
</fonts>"""

FONT_FAMILY = "SpeechTest"


def _ensure_font_family(engine):
    """
    Register a font family so the renderer has something to draw with.

    Returns
    -------
    str | None
        The family name, or None when no font could be loaded.
    """
    font_manager = engine.getFontManager()
    if font_manager.hasFamily(FONT_FAMILY):
        return FONT_FAMILY
    try:
        engine.loadFontManifestFromString(FONT_MANIFEST)
    except Exception:
        return None
    return FONT_FAMILY if font_manager.hasFamily(FONT_FAMILY) else None


def _render_once(engine):
    """Pump a single frame so enabled renderers run."""
    engine.initializePumping()
    try:
        engine.pump()
    finally:
        engine.finalizePumping()


def test_classic_bubble_renders_without_error(engine):
    """Exercise the CLASSIC draw path end to end against the real backend."""
    family = _ensure_font_family(engine)
    if family is None:
        pytest.skip("no font family could be registered")

    renderer, layer, obj = _make_renderer(engine)
    style = _style(
        hasBackgroundOverride=True,
        hasBorderOverride=True,
        cornerRadius=10,
        tailSize=12,
        maxTextWidth=200,
    )
    style.backgroundColorOverride = fife.Color(255, 100, 100, 165)
    style.borderColorOverride = fife.Color(255, 50, 50)
    renderer.setDefaultSpeechStyle(style)
    renderer.setFont(engine.getFontManager().getFont(family, 14))
    renderer.setEnabled(True)

    instance = _make_instance(layer, obj)
    instance.say("Hello bubble", 2000)

    # Must not raise, assert or abort inside the renderer.
    _render_once(engine)


def test_classic_bubble_renders_per_instance_styles(engine):
    """Two instances with different styles render in the same frame."""
    family = _ensure_font_family(engine)
    if family is None:
        pytest.skip("no font family could be registered")

    renderer, layer, obj = _make_renderer(engine)
    renderer.setFont(engine.getFontManager().getFont(family, 14))
    renderer.setEnabled(True)

    base = _style(hasBackgroundOverride=True)
    base.backgroundColorOverride = fife.Color(0, 0, 255, 200)

    left = _make_instance(layer, obj, 0, 0)
    right = _make_instance(layer, obj, 3, 0)
    left.say("Left", 2000)
    right.say("Right", 2000)

    renderer.setSpeechStyle(left, base)
    # right keeps the default (no bubble at all)
    _render_once(engine)

    assert renderer.getEffectiveStyle(left).hasBackgroundOverride
    assert not renderer.getEffectiveStyle(right).hasBackgroundOverride


@pytest.mark.parametrize(
    "tail_direction",
    [
        fife.TailDirection_DOWN,
        fife.TailDirection_UP,
        fife.TailDirection_LEFT,
        fife.TailDirection_RIGHT,
        fife.TailDirection_AUTO,
    ],
)
def test_classic_bubble_renders_every_tail_direction(engine, tail_direction):
    """Every tail direction must produce a drawable triangle."""
    family = _ensure_font_family(engine)
    if family is None:
        pytest.skip("no font family could be registered")

    renderer, layer, obj = _make_renderer(engine)
    style = _style(hasBackgroundOverride=True, hasBorderOverride=True)
    style.backgroundColorOverride = fife.Color(200, 200, 200, 255)
    style.borderColorOverride = fife.Color(0, 0, 0, 255)
    style.tailDir = tail_direction
    renderer.setDefaultSpeechStyle(style)
    renderer.setFont(engine.getFontManager().getFont(family, 14))
    renderer.setEnabled(True)

    instance = _make_instance(layer, obj)
    instance.say("Tail", 2000)

    _render_once(engine)


def test_bubble_with_zero_corner_radius_renders(engine):
    """cornerRadius 0 must take the plain rectangle path."""
    family = _ensure_font_family(engine)
    if family is None:
        pytest.skip("no font family could be registered")

    renderer, layer, obj = _make_renderer(engine)
    style = _style(hasBackgroundOverride=True, cornerRadius=0)
    style.backgroundColorOverride = fife.Color(10, 20, 30, 255)
    renderer.setDefaultSpeechStyle(style)
    renderer.setFont(engine.getFontManager().getFont(family, 14))
    renderer.setEnabled(True)

    instance = _make_instance(layer, obj)
    instance.say("Square", 2000)

    _render_once(engine)


def test_multiline_and_wrapped_text_renders(engine):
    """Both the newline and the maxTextWidth paths must render."""
    family = _ensure_font_family(engine)
    if family is None:
        pytest.skip("no font family could be registered")

    renderer, layer, obj = _make_renderer(engine)
    style = _style(hasBackgroundOverride=True, maxTextWidth=80)
    style.backgroundColorOverride = fife.Color(255, 255, 0, 200)
    renderer.setDefaultSpeechStyle(style)
    renderer.setFont(engine.getFontManager().getFont(family, 14))
    renderer.setEnabled(True)

    first = _make_instance(layer, obj, 0, 0)
    second = _make_instance(layer, obj, 0, 3)
    first.say("A very long line of speech that has to wrap somewhere", 2000)
    second.say("two\nlines", 2000)

    _render_once(engine)


def test_offscreen_instance_is_culled_without_error(engine):
    """A bubble far outside the viewport must be skipped cleanly."""
    family = _ensure_font_family(engine)
    if family is None:
        pytest.skip("no font family could be registered")

    renderer, layer, obj = _make_renderer(engine)
    style = _style(hasBackgroundOverride=True)
    style.backgroundColorOverride = fife.Color(255, 0, 255, 255)
    renderer.setDefaultSpeechStyle(style)
    renderer.setFont(engine.getFontManager().getFont(family, 14))
    renderer.setEnabled(True)

    instance = _make_instance(layer, obj, 500, 500)
    instance.say("Far away", 2000)

    _render_once(engine)


def test_destroying_styled_instance_while_rendering(engine):
    """Dropping a styled instance before a frame must not leave a dangling style."""
    family = _ensure_font_family(engine)
    if family is None:
        pytest.skip("no font family could be registered")

    renderer, layer, obj = _make_renderer(engine)
    style = _style(hasBackgroundOverride=True)
    style.backgroundColorOverride = fife.Color(1, 2, 3, 4)
    renderer.setDefaultSpeechStyle(style)
    renderer.setFont(engine.getFontManager().getFont(family, 14))
    renderer.setEnabled(True)

    doomed = _make_instance(layer, obj, 0, 0)
    doomed.say("Doomed", 2000)
    renderer.setSpeechStyle(doomed, style)

    kept = _make_instance(layer, obj, 3, 0)
    kept.say("Kept", 2000)
    renderer.setSpeechStyle(kept, style)

    layer.removeInstance(doomed)

    _render_once(engine)

    assert renderer.getEffectiveStyle(kept).hasBackgroundOverride
