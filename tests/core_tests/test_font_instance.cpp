// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// 3rd party library includes
#include <catch2/catch_test_macros.hpp>

// Standard C++ library includes
#include <memory>
#include <stdexcept>

// FIFE includes
#include "fixture.h"
#include "video/fonts/fontfamily.h"
#include "video/fonts/fontinstance.h"
#include "video/fonts/fonttypes.h"
#include "video/fonts/imagefontface.h"
#include "video/fonts/truetypefontface.h"

using FIFE::AssetHandle;
using FIFE::FontFace;
using FIFE::FontFamily;
using FIFE::FontWeight;
using FIFE::TrueTypeFontFace;
using FIFE::TrueTypeFontInstance;

TEST_CASE("TrueTypeFontInstance basic properties")
{
    FontTestFixture const fixture;
    auto face = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);
    TrueTypeFontInstance instance(face, 12);

    REQUIRE(instance.getSize() == 12);
    REQUIRE(instance.isAntialias() == true);
    REQUIRE_FALSE(instance.isBold());
    REQUIRE(instance.getFace() == face);
}

TEST_CASE("TrueTypeFontInstance getKey matches expected")
{
    FontTestFixture const fixture;
    auto face = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);
    TrueTypeFontInstance instance(face, 12, true, false, false, 0);

    auto key = instance.getKey();
    REQUIRE(key.asset.id == 1);
    REQUIRE(key.size == 12);
    REQUIRE(key.bold == false);
}

TEST_CASE("TrueTypeFontInstance invalid args throw")
{
    FontTestFixture const fixture;
    REQUIRE_THROWS_AS(TrueTypeFontInstance(nullptr, 0), std::invalid_argument);
}

TEST_CASE("FontFamily basic operations")
{
    FontFamily family("Test");
    REQUIRE(family.getId() == "Test");
    REQUIRE(family.isEmpty());

    family.addFace(std::make_shared<FontFace>(AssetHandle{1}), FontWeight::Regular);
    REQUIRE_FALSE(family.isEmpty());

    family.addFallback("Fallback");
    REQUIRE(family.getFallbacks().size() == 1);
}

TEST_CASE("FontFamily selectFace returns correct weight")
{
    FontTestFixture const fixture;
    auto face400 = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);
    auto face700 = std::make_shared<TrueTypeFontFace>(AssetHandle{2}, "tests/data/FreeMono.ttf", 12);

    FontFamily family("Test");
    family.addFace(face400, FontWeight::Regular);
    family.addFace(face700, FontWeight::Bold);

    auto const * selected = family.selectFace(FontWeight::Bold, false, 0x0041);
    REQUIRE(selected != nullptr);
    REQUIRE(selected->getAssetHandle().id == 2);
}

TEST_CASE("FontFamily selectFace fallback to first face when no match")
{
    FontTestFixture const fixture;
    auto face = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);
    FontFamily family("Test");
    family.addFace(face, FontWeight::Light);

    auto const * selected = family.selectFace(FontWeight::Bold, false, 0xFFFF);
    REQUIRE(selected != nullptr);
    REQUIRE(selected->getAssetHandle().id == 1);
}

TEST_CASE("FontFamily selectFace with codepoint support")
{
    FontTestFixture const fixture;

    auto face = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);
    FontFamily family("Test");
    family.addFace(face, FontWeight::Regular);

    auto const * selected = family.selectFace(FontWeight::Regular, false, 0x0041);
    REQUIRE(selected != nullptr);
}

TEST_CASE("FontFamily empty id throws")
{
    REQUIRE_THROWS_AS(FontFamily(""), std::invalid_argument);
}

TEST_CASE("FontInstance getKey produces different keys for different configs")
{
    FontTestFixture const fixture;
    auto face = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);

    TrueTypeFontInstance inst1(face, 12);
    TrueTypeFontInstance inst2(face, 14);

    auto key1 = inst1.getKey();
    auto key2 = inst2.getKey();
    REQUIRE_FALSE(key1 == key2);
}

TEST_CASE("TrueTypeFontInstance getWidth returns positive for ASCII text")
{
    FontTestFixture const fixture;
    auto face = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);
    TrueTypeFontInstance instance(face, 12);

    int const w = instance.getWidth("Hello");
    REQUIRE(w > 0);
}

TEST_CASE("TrueTypeFontInstance getWidth empty string returns zero")
{
    FontTestFixture const fixture;
    auto face = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);
    TrueTypeFontInstance instance(face, 12);

    REQUIRE(instance.getWidth("") == 0);
}

TEST_CASE("TrueTypeFontInstance getHeight respects rowSpacing")
{
    FontTestFixture const fixture;
    auto face = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);

    TrueTypeFontInstance inst1(face, 12, true, false, false, 0, 0, 0);
    TrueTypeFontInstance inst2(face, 12, true, false, false, 0, 0, 5);

    int const h1 = inst1.getHeight();
    int const h2 = inst2.getHeight();
    REQUIRE(h1 > 0);
    REQUIRE(h2 == h1 + 5);
}

TEST_CASE("TrueTypeFontInstance getKey includes all config fields")
{
    FontTestFixture const fixture;
    auto face = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);

    TrueTypeFontInstance instance(face, 14, false, true, true, 1, 2, 3);
    auto key = instance.getKey();

    REQUIRE(key.size == 14);
    REQUIRE(key.bold == true);
    REQUIRE(key.italic == true);
    REQUIRE(key.antialias == false);
    REQUIRE(key.hinting == 1);
}

TEST_CASE("FontFamily fallback chain selection")
{
    FontTestFixture const fixture;

    auto primary  = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);
    auto fallback = std::make_shared<TrueTypeFontFace>(AssetHandle{2}, "tests/data/FreeMono.ttf", 12);

    FontFamily family("Test");
    family.addFace(primary, FontWeight::Regular);
    family.addFallback("Fallback");

    auto const * sel = family.selectFace(FontWeight::Regular, false, 0x0041);
    REQUIRE(sel != nullptr);
    REQUIRE(sel->getAssetHandle().id == 1);
}

TEST_CASE("FontFamily selectFace uses fallback for unsupported codepoint")
{
    FontTestFixture const fixture;

    auto regular = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);
    FontFamily family("Test");
    family.addFace(regular, FontWeight::Regular);

    auto const * sel = family.selectFace(FontWeight::Regular, false, 0xFFFF);
    REQUIRE(sel != nullptr);
    REQUIRE(sel->getAssetHandle().id == 1);
}

TEST_CASE("FontFamily fallback chain is stored correctly")
{
    FontFamily family("Test");
    family.addFallback("FB1");
    family.addFallback("FB2");

    REQUIRE(family.getFallbacks().size() == 2);
    REQUIRE(family.getFallbacks().at(0) == "FB1");
    REQUIRE(family.getFallbacks().at(1) == "FB2");
}

TEST_CASE("TrueTypeFontInstance getWidth handles multiline text")
{
    FontTestFixture const fixture;
    auto face = std::make_shared<TrueTypeFontFace>(AssetHandle{1}, "tests/data/FreeMono.ttf", 12);

    TrueTypeFontInstance instance(face, 12);
    int const wSingle = instance.getWidth("Hello");
    int const wMulti  = instance.getWidth("Hello\nWorld");

    REQUIRE(wSingle > 0);
    REQUIRE(wMulti > 0);
}
