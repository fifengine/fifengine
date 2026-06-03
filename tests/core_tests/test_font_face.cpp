// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "fixture.h"
#include "video/fonts/fontface.h"
#include "video/fonts/fonttypes.h"
#include "video/fonts/imagefontface.h"
#include "video/fonts/truetypefontface.h"

#include <catch2/catch_test_macros.hpp>

using FIFE::AssetHandle;
using FIFE::FontFace;
using FIFE::ImageFontFace;
using FIFE::TrueTypeFontFace;

TEST_CASE("FontFace base class")
{
    AssetHandle handle{42};
    FontFace face(handle);
    REQUIRE(face.getAssetHandle().id == 42);
    REQUIRE_FALSE(face.supports(0x0041));
}

TEST_CASE("TrueTypeFontFace from file")
{
    FontTestFixture const fixture;

    AssetHandle handle{1};
    std::string const fontPath = "tests/data/FreeMono.ttf";
    TrueTypeFontFace face(handle, fontPath, 12);

    REQUIRE(face.getAssetHandle().id == 1);
    REQUIRE(face.getFont() != nullptr);
    REQUIRE(face.getPointSize() == 12);
    REQUIRE(face.supports(0x0041));
    REQUIRE(face.supports(0x007E));
}

TEST_CASE("TrueTypeFontFace invalid file throws")
{
    FontTestFixture const fixture;
    AssetHandle handle{2};
    REQUIRE_THROWS_AS(TrueTypeFontFace(handle, "nonexistent.ttf", 12), std::exception);
}

TEST_CASE("ImageFontFace from PNG")
{
    AssetHandle handle{3};
    std::string const fontPath = "tests/data/rpgfont.png";

    std::ifstream f(fontPath.c_str());
    if (f.good()) {
        f.close();
        std::string const glyphs =
            " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
        ImageFontFace face(handle, fontPath, glyphs);
        REQUIRE(face.getAssetHandle().id == 3);
        REQUIRE(face.supports('A'));
        REQUIRE_FALSE(face.supports(0xFFFF));
        REQUIRE(face.getHeight() > 0);
    } else {
        WARN("rpgfont.png not found, skipping ImageFontFace test");
    }
}

TEST_CASE("ImageFontFace empty glyph string throws")
{
    AssetHandle handle{4};
    REQUIRE_THROWS_AS(ImageFontFace(handle, "tests/data/rpgfont.png", ""), std::exception);
}

TEST_CASE("TrueTypeFontFace supports ASCII range")
{
    FontTestFixture const fixture;
    AssetHandle handle{5};
    TrueTypeFontFace face(handle, "tests/data/FreeMono.ttf", 12);

    for (char c = 0x20; c <= 0x7E; ++c) {
        REQUIRE(face.supports(static_cast<uint32_t>(c)));
    }
    REQUIRE_FALSE(face.supports(0xFFFF));
}

TEST_CASE("TrueTypeFontFace from memory")
{
    FontTestFixture const fixture;

    std::ifstream file("tests/data/FreeMono.ttf", std::ios::binary | std::ios::ate);
    REQUIRE(file.is_open());
    auto fileSize = file.tellg();
    std::vector<char> buf(static_cast<size_t>(fileSize));
    file.seekg(0);
    file.read(buf.data(), fileSize);
    file.close();

    AssetHandle handle{10};
    TrueTypeFontFace face(
        handle, static_cast<uint8_t const *>(static_cast<void const *>(buf.data())), buf.size(), 12, "memory");

    REQUIRE(face.getAssetHandle().id == 10);
    REQUIRE(face.getFont() != nullptr);
    REQUIRE(face.getPointSize() == 12);
    REQUIRE(face.supports(0x0041));
}

TEST_CASE("TrueTypeFontFace invalid point size throws")
{
    FontTestFixture const fixture;
    AssetHandle handle{2};
    REQUIRE_THROWS_AS(TrueTypeFontFace(handle, "tests/data/FreeMono.ttf", 0), std::exception);
    REQUIRE_THROWS_AS(TrueTypeFontFace(handle, "tests/data/FreeMono.ttf", -1), std::exception);
}

TEST_CASE("TrueTypeFontFace memory constructor null data throws")
{
    FontTestFixture const fixture;
    AssetHandle handle{3};
    REQUIRE_THROWS_AS(TrueTypeFontFace(handle, nullptr, 0, 12, "null"), std::exception);
}

TEST_CASE("FontFace invalid AssetHandle")
{
    REQUIRE_THROWS_AS(FontFace(AssetHandle{0}), std::exception);
}
