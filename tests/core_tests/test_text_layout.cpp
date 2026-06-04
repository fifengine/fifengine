// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <memory>
#include <utility>

#include "fixture.h"
#include "video/fonts/fontface.h"
#include "video/fonts/fonttypes.h"
#include "video/fonts/textlayout.h"
#include <catch2/catch_test_macros.hpp>

using FIFE::AssetHandle;
using FIFE::FontFace;
using FIFE::GlyphRun;
using FIFE::TextLayout;

// Mock FontFace for testing
class MockFontFace : public FontFace
{
    public:
        MockFontFace() : FontFace(AssetHandle{1})
        {
        }
        bool supports(uint32_t) const override
        {
            return true;
        }
};

TEST_CASE("GlyphRun can be constructed")
{
    GlyphRun run;
    REQUIRE(run.glyphIndices.empty());
    REQUIRE(run.positions.empty());
    REQUIRE(run.face == nullptr);
}

TEST_CASE("TextLayout is initially empty")
{
    TextLayout layout;
    REQUIRE(layout.isEmpty());
    REQUIRE(layout.width == 0);
    REQUIRE(layout.height == 0);
}

TEST_CASE("TextLayout addRun updates dimensions")
{
    TextLayout layout;

    auto face = std::make_shared<MockFontFace>();

    GlyphRun run;
    run.face         = face;
    run.glyphIndices = {1, 2, 3};
    run.positions    = {Point(0, 0), Point(10, 0), Point(20, 0)};

    layout.addRun(std::move(run));

    REQUIRE_FALSE(layout.isEmpty());
    REQUIRE(layout.runs.size() == 1);
    REQUIRE(layout.width >= 20);
}

TEST_CASE("TextLayout clear resets state")
{
    TextLayout layout;
    auto face = std::make_shared<MockFontFace>();

    GlyphRun run;
    run.face         = face;
    run.glyphIndices = {1, 2};
    run.positions    = {Point(0, 0), Point(10, 0)};
    layout.addRun(std::move(run));

    layout.clear();
    REQUIRE(layout.isEmpty());
    REQUIRE(layout.width == 0);
    REQUIRE(layout.height == 0);
}

TEST_CASE("TextLayout multiple runs")
{
    TextLayout layout;
    auto face = std::make_shared<MockFontFace>();

    GlyphRun run1;
    run1.face         = face;
    run1.glyphIndices = {1, 2};
    run1.positions    = {Point(0, 0), Point(10, 0)};
    layout.addRun(std::move(run1));

    GlyphRun run2;
    run2.face         = face;
    run2.glyphIndices = {3, 4};
    run2.positions    = {Point(0, 20), Point(10, 20)};
    layout.addRun(std::move(run2));

    REQUIRE(layout.runs.size() == 2);
}

TEST_CASE("addRun with null face throws")
{
    TextLayout layout;
    GlyphRun run;
    run.glyphIndices = {1};
    run.positions    = {Point(0, 0)};
    // face is null — should assert
    REQUIRE_THROWS_AS(layout.addRun(std::move(run)), std::exception);
}

TEST_CASE("TextLayout run with no glyphs keeps zero dimensions")
{
    TextLayout layout;
    auto face = std::make_shared<MockFontFace>();

    GlyphRun run;
    run.face = face;
    layout.addRun(std::move(run));

    REQUIRE(layout.width == 0);
    REQUIRE(layout.height == 0);
    REQUIRE(layout.runs.size() == 1);
}

TEST_CASE("TextLayout single glyph run")
{
    TextLayout layout;
    auto face = std::make_shared<MockFontFace>();

    GlyphRun run;
    run.face         = face;
    run.glyphIndices = {1};
    run.positions    = {Point(5, 3)};
    layout.addRun(std::move(run));

    REQUIRE(layout.width >= 5);
    REQUIRE(layout.height >= 3);
}

TEST_CASE("TextLayout handles negative positions")
{
    TextLayout layout;
    auto face = std::make_shared<MockFontFace>();

    GlyphRun run;
    run.face         = face;
    run.glyphIndices = {1, 2};
    run.positions    = {Point(-10, -20), Point(30, 0)};
    layout.addRun(std::move(run));

    REQUIRE(layout.width >= 30);
}
