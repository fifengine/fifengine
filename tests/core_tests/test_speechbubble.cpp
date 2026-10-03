// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes

// Platform specific includes
#include <catch2/catch_test_macros.hpp>

// 3rd party library includes

// FIFE includes
#include "util/structures/point.h"
#include "util/structures/rect.h"
#include "view/renderers/speechbubblegeometry.h"
#include "view/renderers/speechstyle.h"

using FIFE::BubbleType;
using FIFE::Color;
using FIFE::Point;
using FIFE::Rect;
using FIFE::SpeechStyle;
using FIFE::TailDirection;
using FIFE::TextAlign;
using FIFE::detail::clampCornerRadius;
using FIFE::detail::computeBubbleBounds;
using FIFE::detail::computeTailVertices;
using FIFE::detail::inferTailDirection;
using FIFE::detail::resolveTailDirection;

TEST_CASE("SpeechStyle defaults", "[core][speechbubble]")
{
    SpeechStyle s;

    CHECK(s.bubbleType == BubbleType::NONE);
    CHECK(s.bubblePadding == 5);
    CHECK(s.cornerRadius == 8);
    CHECK(s.tailSize == 10);
    CHECK(s.tailOffset == 0);
    CHECK(s.tailDir == TailDirection::DOWN);
    CHECK(s.maxTextWidth == 0);
    CHECK(s.textAlign == TextAlign::LEFT);
    CHECK(s.fontOverride == nullptr);
    CHECK(s.hasTextColorOverride == false);
    CHECK(s.hasBackgroundOverride == false);
}

TEST_CASE("SpeechStyle plain() factory", "[core][speechbubble]")
{
    SpeechStyle s = SpeechStyle::plain();
    CHECK(s.bubbleType == BubbleType::NONE);
    CHECK(s.bubblePadding == 5);
}

TEST_CASE("SpeechStyle classic() factory", "[core][speechbubble]")
{
    SpeechStyle s = SpeechStyle::classic();
    CHECK(s.bubbleType == BubbleType::CLASSIC);
    CHECK(s.bubblePadding == 5);
    CHECK(s.cornerRadius == 8);
}

TEST_CASE("SpeechStyle plain() leaves the bubble type unset", "[core][speechbubble]")
{
    CHECK(SpeechStyle::plain().bubbleType == BubbleType::NONE);
}

TEST_CASE("SpeechStyle color overrides", "[core][speechbubble]")
{
    SpeechStyle s;
    CHECK(s.hasTextColorOverride == false);
    CHECK(s.hasBackgroundOverride == false);

    s.hasTextColorOverride = true;
    s.textColorOverride    = Color(255, 200, 100);
    CHECK(s.textColorOverride.getR() == 255);
    CHECK(s.textColorOverride.getG() == 200);
    CHECK(s.textColorOverride.getB() == 100);

    s.hasBackgroundOverride   = true;
    s.backgroundColorOverride = Color(50, 50, 100, 220);
    CHECK(s.backgroundColorOverride.getR() == 50);
    CHECK(s.backgroundColorOverride.getG() == 50);
    CHECK(s.backgroundColorOverride.getB() == 100);
    CHECK(s.backgroundColorOverride.getAlpha() == 220);
}

TEST_CASE("SpeechStyle enum values", "[core][speechbubble]")
{
    CHECK(static_cast<int>(BubbleType::NONE) == 0);
    CHECK(static_cast<int>(BubbleType::CLASSIC) == 1);
    CHECK(static_cast<int>(BubbleType::ROUND) == 2);
    CHECK(static_cast<int>(BubbleType::THOUGHT) == 3);
    CHECK(static_cast<int>(BubbleType::SHOUT) == 4);
    CHECK(static_cast<int>(BubbleType::WHISPER) == 5);

    CHECK(static_cast<int>(TailDirection::DOWN) == 0);
    CHECK(static_cast<int>(TailDirection::UP) == 1);
    CHECK(static_cast<int>(TailDirection::LEFT) == 2);
    CHECK(static_cast<int>(TailDirection::RIGHT) == 3);
    CHECK(static_cast<int>(TailDirection::AUTO) == 4);

    CHECK(static_cast<int>(TextAlign::LEFT) == 0);
    CHECK(static_cast<int>(TextAlign::CENTER) == 1);
    CHECK(static_cast<int>(TextAlign::RIGHT) == 2);
}

TEST_CASE("computeBubbleBounds expands by padding", "[core][speechbubble]")
{
    SpeechStyle s;
    s.bubblePadding = 5;

    Rect const bubble = computeBubbleBounds(Rect(100, 100, 50, 20), s);

    CHECK(bubble.x == 95);
    CHECK(bubble.y == 95);
    CHECK(bubble.w == 60);
    CHECK(bubble.h == 30);
}

TEST_CASE("computeBubbleBounds with zero padding", "[core][speechbubble]")
{
    SpeechStyle s;
    s.bubblePadding = 0;

    Rect const bubble = computeBubbleBounds(Rect(100, 100, 50, 20), s);

    CHECK(bubble.x == 100);
    CHECK(bubble.y == 100);
    CHECK(bubble.w == 50);
    CHECK(bubble.h == 20);
}

TEST_CASE("computeBubbleBounds clamps negative padding", "[core][speechbubble]")
{
    SpeechStyle s;
    s.bubblePadding = -5;

    Rect const bubble = computeBubbleBounds(Rect(100, 100, 50, 20), s);

    CHECK(bubble.x == 100);
    CHECK(bubble.w == 50);
}

TEST_CASE("computeTailVertices DOWN", "[core][speechbubble]")
{
    Rect const bubble(100, 90, 60, 30);

    auto const v = computeTailVertices(bubble, Point(130, 150), TailDirection::DOWN, 10);

    CHECK(v[0] == Point(130, 150));
    CHECK(v[1] == Point(125, 120));
    CHECK(v[2] == Point(135, 120));
}

TEST_CASE("computeTailVertices UP", "[core][speechbubble]")
{
    Rect const bubble(100, 150, 60, 30);

    auto const v = computeTailVertices(bubble, Point(130, 90), TailDirection::UP, 10);

    CHECK(v[0] == Point(130, 90));
    CHECK(v[1] == Point(125, 150));
    CHECK(v[2] == Point(135, 150));
}

TEST_CASE("computeTailVertices LEFT", "[core][speechbubble]")
{
    Rect const bubble(200, 150, 60, 30);

    auto const v = computeTailVertices(bubble, Point(120, 165), TailDirection::LEFT, 10);

    CHECK(v[0] == Point(120, 165));
    CHECK(v[1] == Point(200, 160));
    CHECK(v[2] == Point(200, 170));
}

TEST_CASE("computeTailVertices RIGHT", "[core][speechbubble]")
{
    Rect const bubble(100, 150, 60, 30);

    auto const v = computeTailVertices(bubble, Point(220, 165), TailDirection::RIGHT, 10);

    CHECK(v[0] == Point(220, 165));
    CHECK(v[1] == Point(160, 160));
    CHECK(v[2] == Point(160, 170));
}

TEST_CASE("computeTailVertices odd tailSize rounds down", "[core][speechbubble]")
{
    Rect const bubble(100, 90, 60, 30);

    auto const v = computeTailVertices(bubble, Point(130, 150), TailDirection::DOWN, 11);

    CHECK(v[1] == Point(125, 120));
    CHECK(v[2] == Point(135, 120));
}

TEST_CASE("computeTailVertices AUTO falls back to DOWN", "[core][speechbubble]")
{
    Rect const bubble(100, 90, 60, 30);

    auto const v = computeTailVertices(bubble, Point(130, 150), TailDirection::AUTO, 10);

    CHECK(v[1] == Point(125, 120));
    CHECK(v[2] == Point(135, 120));
}

TEST_CASE("computeTailVertices produces non-degenerate triangles", "[core][speechbubble]")
{
    Rect const bubble(100, 100, 60, 30);
    Point const tip(130, 40);

    CHECK(
        computeTailVertices(bubble, tip, TailDirection::DOWN, 10)[0].y !=
        computeTailVertices(bubble, tip, TailDirection::DOWN, 10)[1].y);
    CHECK(
        computeTailVertices(bubble, tip, TailDirection::UP, 10)[0].y !=
        computeTailVertices(bubble, tip, TailDirection::UP, 10)[1].y);
    CHECK(
        computeTailVertices(bubble, tip, TailDirection::LEFT, 10)[0].x !=
        computeTailVertices(bubble, tip, TailDirection::LEFT, 10)[1].x);
    CHECK(
        computeTailVertices(bubble, tip, TailDirection::RIGHT, 10)[0].x !=
        computeTailVertices(bubble, tip, TailDirection::RIGHT, 10)[1].x);
}

TEST_CASE("inferTailDirection anchor below bubble", "[core][speechbubble]")
{
    CHECK(inferTailDirection(Point(130, 180), Rect(100, 100, 60, 30)) == TailDirection::DOWN);
}

TEST_CASE("inferTailDirection anchor above bubble", "[core][speechbubble]")
{
    CHECK(inferTailDirection(Point(130, 50), Rect(100, 100, 60, 30)) == TailDirection::UP);
}

TEST_CASE("inferTailDirection dominant axis wins", "[core][speechbubble]")
{
    // Horizontal offset is larger, so the tail stays on the vertical axis.
    CHECK(inferTailDirection(Point(50, 112), Rect(100, 100, 60, 30)) == TailDirection::UP);
    CHECK(inferTailDirection(Point(210, 118), Rect(100, 100, 60, 30)) == TailDirection::DOWN);
}

TEST_CASE("inferTailDirection tie favours DOWN", "[core][speechbubble]")
{
    Rect const bubble(100, 100, 60, 30);
    Point const center(130, 115);

    CHECK(inferTailDirection(center, bubble) == TailDirection::DOWN);
}

TEST_CASE("resolveTailDirection passes explicit directions through", "[core][speechbubble]")
{
    Rect const bubble(100, 100, 60, 30);
    Point const anchor(130, 40);

    CHECK(resolveTailDirection(TailDirection::RIGHT, anchor, bubble) == TailDirection::RIGHT);
    CHECK(resolveTailDirection(TailDirection::AUTO, anchor, bubble) == TailDirection::UP);
}

TEST_CASE("clampCornerRadius limits radius to half extents", "[core][speechbubble]")
{
    Rect const bubble(100, 100, 60, 30);

    CHECK(clampCornerRadius(bubble, 8) == 8);
    CHECK(clampCornerRadius(bubble, 40) == 15);
    CHECK(clampCornerRadius(bubble, -3) == 0);
}
