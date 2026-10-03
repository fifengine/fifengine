# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest


class TestSpeechBubble:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.speechbubble import SpeechBubble

        assert SpeechBubble is not None

    def test_attributes_defined(self):
        from fife.extensions.pychan.widgets.speechbubble import SpeechBubble

        names = [a.name for a in SpeechBubble.ATTRIBUTES]
        for attr in (
            "corner_radius",
            "tail_width",
            "tail_height",
            "tail_direction",
            "bubble_style",
            "tail_profile",
            "tail_color",
            "tail_offset",
        ):
            assert attr in names

    def test_registered_in_widgets_registry(self):
        from fife.extensions.pychan.widgets import WIDGETS
        from fife.extensions.pychan.widgets.speechbubble import SpeechBubble

        assert WIDGETS["SpeechBubble"] is SpeechBubble

    def test_is_a_container(self):
        from fife.extensions.pychan.widgets.containers import Container
        from fife.extensions.pychan.widgets.speechbubble import SpeechBubble

        assert issubclass(SpeechBubble, Container)


class TestSpeechBubbleMappings:
    def test_bubble_style_names_resolve(self):
        from fife.extensions.pychan.widgets.speechbubble import BUBBLE_STYLES

        from fife import fifechan

        for value, attr in BUBBLE_STYLES.items():
            assert hasattr(fifechan.SpeechBubble, attr), f"{value} -> {attr}"

    def test_tail_direction_names_resolve(self):
        from fife.extensions.pychan.widgets.speechbubble import TAIL_DIRECTIONS

        from fife import fifechan

        for value, attr in TAIL_DIRECTIONS.items():
            assert hasattr(fifechan.SpeechBubble, attr), f"{value} -> {attr}"

    def test_tail_profile_names_resolve(self):
        from fife.extensions.pychan.widgets.speechbubble import TAIL_PROFILES

        from fife import fifechan

        for value, attr in TAIL_PROFILES.items():
            assert hasattr(fifechan.TailProfile, attr), f"{value} -> {attr}"

    def test_every_direction_is_mapped(self):
        from fife.extensions.pychan.widgets.speechbubble import TAIL_DIRECTIONS

        assert set(TAIL_DIRECTIONS) == {
            "none",
            "up",
            "down",
            "left",
            "right",
            "bottomleft",
            "bottomright",
            "topleft",
            "topright",
            "auto",
        }

    def test_every_style_is_mapped(self):
        from fife.extensions.pychan.widgets.speechbubble import BUBBLE_STYLES

        assert set(BUBBLE_STYLES) == {"classic", "round", "thought", "shout", "whisper"}


def _bare_bubble():
    """Build a SpeechBubble wrapper without going through Widget.__init__.

    Returns
    -------
    SpeechBubble
        An instance holding a real fifechan widget, which needs no GUI manager.
    """
    from fife.extensions.pychan.widgets.speechbubble import SpeechBubble

    from fife import fifechan

    bubble = SpeechBubble.__new__(SpeechBubble)
    bubble.real_widget = fifechan.SpeechBubble()
    bubble._bubble_style = None
    bubble._tail_direction = None
    bubble._tail_profile = None
    return bubble


class TestSpeechBubbleMappingValidation:
    """Unknown config values must be rejected, not silently defaulted."""

    def test_unknown_bubble_style_is_rejected(self):
        import pytest
        from fife.extensions.pychan.exceptions import ParserError
        from fife.extensions.pychan.widgets.speechbubble import SpeechBubble

        bubble = _bare_bubble()
        with pytest.raises(ParserError, match="bubble_style"):
            SpeechBubble._setBubbleStyle(bubble, "geometric")

    def test_unknown_tail_direction_is_rejected(self):
        import pytest
        from fife.extensions.pychan.exceptions import ParserError
        from fife.extensions.pychan.widgets.speechbubble import SpeechBubble

        bubble = _bare_bubble()
        with pytest.raises(ParserError, match="tail_direction"):
            SpeechBubble._setTailDirection(bubble, "sideways")

    def test_unknown_tail_profile_is_rejected(self):
        import pytest
        from fife.extensions.pychan.exceptions import ParserError
        from fife.extensions.pychan.widgets.speechbubble import SpeechBubble

        bubble = _bare_bubble()
        with pytest.raises(ParserError, match="tail_profile"):
            SpeechBubble._setTailProfile(bubble, "squiggly")

    def test_every_style_name_is_accepted(self):
        from fife.extensions.pychan.widgets.speechbubble import (
            BUBBLE_STYLES,
            SpeechBubble,
        )

        for name in BUBBLE_STYLES:
            bubble = _bare_bubble()
            bubble._bubble_style = None
            SpeechBubble._setBubbleStyle(bubble, name)
            assert bubble._bubble_style == BUBBLE_STYLES[name]

    def test_style_names_are_case_insensitive(self):
        from fife.extensions.pychan.widgets.speechbubble import (
            BUBBLE_STYLES,
            SpeechBubble,
        )

        bubble = _bare_bubble()
        SpeechBubble._setBubbleStyle(bubble, "THOUGHT")
        assert bubble._bubble_style == BUBBLE_STYLES["thought"]


class TestSpeechBubbleRealWidget:
    """
    Exercises the wrapped fifechan widget.

    The real widget needs no pychan manager, so these run standalone.
    """

    def test_defaults(self):
        from fife import fifechan

        bubble = fifechan.SpeechBubble()
        assert bubble.getCornerRadius() == 10
        assert bubble.getTailWidth() == 16
        assert bubble.getTailHeight() == 12
        assert bubble.getTailOffset() == pytest.approx(0.5)

    def test_setters_roundtrip(self):
        from fife import fifechan

        bubble = fifechan.SpeechBubble()
        bubble.setCornerRadius(14)
        bubble.setTailWidth(22)
        bubble.setTailHeight(18)
        bubble.setTailOffset(0.25)

        assert bubble.getCornerRadius() == 14
        assert bubble.getTailWidth() == 22
        assert bubble.getTailHeight() == 18
        assert bubble.getTailOffset() == pytest.approx(0.25)

    def test_bubble_style_roundtrip(self):
        from fife import fifechan

        bubble = fifechan.SpeechBubble()
        for style in (
            fifechan.SpeechBubble.BubbleStyle_Classic,
            fifechan.SpeechBubble.BubbleStyle_Round,
            fifechan.SpeechBubble.BubbleStyle_Thought,
            fifechan.SpeechBubble.BubbleStyle_Shout,
            fifechan.SpeechBubble.BubbleStyle_Whisper,
        ):
            bubble.setBubbleStyle(style)
            assert bubble.getBubbleStyle() == style

    def test_tail_direction_roundtrip(self):
        from fife import fifechan

        bubble = fifechan.SpeechBubble()
        bubble.setTailDirection(fifechan.SpeechBubble.TailDirection_BottomLeft)
        assert bubble.getTailDirection() == fifechan.SpeechBubble.TailDirection_BottomLeft

    def test_tail_profile_presets(self):
        from fife import fifechan

        bubble = fifechan.SpeechBubble()
        jagged = fifechan.TailProfile.jagged()
        assert jagged.jaggedness == pytest.approx(0.5)

        bubble.setTailProfile(jagged)
        assert bubble.getTailProfile().jaggedness == pytest.approx(0.5)

    def test_tail_color_roundtrip(self):
        from fife import fifechan

        bubble = fifechan.SpeechBubble()
        bubble.setTailColor(fifechan.Color(10, 20, 30, 40))
        color = bubble.getTailColor()
        assert (color.r, color.g, color.b, color.a) == (10, 20, 30, 40)

    def test_adjust_size_reserves_tail_space(self):
        from fife import fifechan

        bubble = fifechan.SpeechBubble()
        bubble.setTailHeight(20)
        bubble.setSize(300, 100)
        bubble.adjustSize()
        # Minimum height accounts for the tail plus readable content.
        assert bubble.getHeight() >= 2 * 10 + 20 + 20
