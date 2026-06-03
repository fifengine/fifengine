// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "util/base/exception.h"
#include "video/fonts/fontdefinitionloader.h"
#include "video/fonts/fonttypes.h"

using FIFE::FontDefinitionLoader;
using FIFE::FontType;

TEST_CASE("Parse valid XML with 2 families, 3 faces total", "[font][definition]")
{
    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">fonts/FreeSans.ttf</face>
    <face weight="700">fonts/FreeSansBold.ttf</face>
  </family>
  <family id="Mono">
    <face weight="400">fonts/FreeMono.ttf</face>
  </family>
</fonts>)";

    auto const defs = FontDefinitionLoader::loadFromXml(xml);
    REQUIRE(defs.size() == 3);
}

TEST_CASE("Verify parsed FontDefinition fields", "[font][definition]")
{
    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">fonts/FreeSans.ttf</face>
  </family>
</fonts>)";

    auto const defs = FontDefinitionLoader::loadFromXml(xml);
    REQUIRE(defs.size() == 1);

    auto const & d = defs.at(0);
    CHECK(d.id == "UI/400");
    CHECK(d.type == FontType::TRUETYPE);
    CHECK(d.source == "fonts/FreeSans.ttf");
    CHECK(d.size == 12);
    CHECK(d.antialias == true);
}

TEST_CASE("Parse bitmap face, verify type is SUBIMAGE and glyphs are populated", "[font][definition]")
{
    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="RPGDefault">
    <face type="bitmap" glyphs="ABCDEFG">fonts/rpgfont.png</face>
  </family>
</fonts>)";

    auto const defs = FontDefinitionLoader::loadFromXml(xml);
    REQUIRE(defs.size() == 1);

    auto const & d = defs.at(0);
    CHECK(d.type == FontType::SUBIMAGE);
    CHECK(d.glyphs == "ABCDEFG");
    CHECK(d.source == "fonts/rpgfont.png");
    CHECK(d.id == "RPGDefault/400");
}

TEST_CASE("Parse minimal XML (one family, one face)", "[font][definition]")
{
    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="Test">
    <face>fonts/test.ttf</face>
  </family>
</fonts>)";

    auto const defs = FontDefinitionLoader::loadFromXml(xml);
    REQUIRE(defs.size() == 1);
    CHECK(defs.at(0).id == "Test/400");
    CHECK(defs.at(0).source == "fonts/test.ttf");
    CHECK(defs.at(0).type == FontType::TRUETYPE);
}

TEST_CASE("Parse with fallback references", "[font][definition]")
{
    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">fonts/FreeSans.ttf</face>
    <fallback>CJK</fallback>
    <fallback>Emoji</fallback>
  </family>
  <family id="Mono">
    <face weight="400">fonts/FreeMono.ttf</face>
  </family>
</fonts>)";

    // Should parse successfully (fallback elements are recognized but
    // not yet mapped to FontDefinition fields in this track)
    auto const defs = FontDefinitionLoader::loadFromXml(xml);
    REQUIRE(defs.size() == 2);
    CHECK(defs.at(0).id == "UI/400");
    CHECK(defs.at(1).id == "Mono/400");
}

TEST_CASE("Empty string throws exception", "[font][definition]")
{
    REQUIRE_THROWS_AS(FontDefinitionLoader::loadFromXml(""), FIFE::InvalidFormat);
}

TEST_CASE("Malformed XML throws exception", "[font][definition]")
{
    std::string const xml = "this is not xml";
    REQUIRE_THROWS_AS(FontDefinitionLoader::loadFromXml(xml), FIFE::InvalidFormat);
}

TEST_CASE("Duplicate font id throws exception", "[font][definition]")
{
    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">fonts/FreeSans.ttf</face>
  </family>
  <family id="UI">
    <face weight="400">fonts/FreeSansDup.ttf</face>
  </family>
</fonts>)";

    REQUIRE_THROWS_AS(FontDefinitionLoader::loadFromXml(xml), FIFE::Duplicate);
}

TEST_CASE("Face with empty source throws exception", "[font][definition]")
{
    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face></face>
  </family>
</fonts>)";

    REQUIRE_THROWS_AS(FontDefinitionLoader::loadFromXml(xml), FIFE::InvalidFormat);
}

TEST_CASE("Missing version attribute is handled (logs warning, doesn't throw)", "[font][definition]")
{
    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts>
  <family id="Test">
    <face>fonts/test.ttf</face>
  </family>
</fonts>)";

    // Should not throw despite missing version attribute
    auto const defs = FontDefinitionLoader::loadFromXml(xml);
    REQUIRE(defs.size() == 1);
    CHECK(defs.at(0).id == "Test/400");
}
