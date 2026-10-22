// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// 3rd party library includes
#include <catch2/catch_test_macros.hpp>

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// FIFE includes
#include "fixture.h"
#include "video/fonts/fontinstanceifontadapter.h"
#include "video/fonts/truetypefontface.h"

namespace
{

    std::string wrapText(std::string const & text, int32_t width)
    {
        auto face     = std::make_shared<FIFE::TrueTypeFontFace>(FIFE::AssetHandle{1}, "tests/data/FreeMono.ttf", 12);
        auto instance = std::make_shared<FIFE::TrueTypeFontInstance>(face, 12);
        FIFE::FontInstanceIFontAdapter adapter(instance);
        return adapter.splitTextToWidth(text, width);
    }

    size_t countOccurrences(std::string const & haystack, std::string const & needle)
    {
        size_t count = 0;
        for (size_t pos = haystack.find(needle); pos != std::string::npos; pos = haystack.find(needle, pos + 1)) {
            ++count;
        }
        return count;
    }

    std::vector<std::string> words(std::string const & text)
    {
        std::vector<std::string> result;
        std::istringstream stream(text);
        std::string word;
        while (stream >> word) {
            result.push_back(word);
        }
        return result;
    }

    /// Removes all whitespace so tests can assert that wrapping neither drops
    /// nor duplicates characters, independently of where lines are broken.
    std::string compact(std::string const & text)
    {
        std::string result = text;
        std::erase(result, ' ');
        std::erase(result, '\n');
        std::erase(result, '\t');
        return result;
    }

} // namespace

TEST_CASE("splitTextToWidth keeps hard newlines without duplicating text", "[fonts][wrap]")
{
    FontTestFixture const fixture;
    std::string const text =
        "You and your crew were on the way to a colony of England in the Pacific Ocean.\n"
        "    Everything went fine until you came into a storm.";

    for (int32_t const width : {80, 100, 120, 160, 200, 240, 300}) {
        std::string const wrapped = wrapText(text, width);
        INFO("width " << width << " -> " << wrapped);
        REQUIRE(wrapped.find('\0') == std::string::npos);
        REQUIRE(countOccurrences(wrapped, "Ocean.") == 1);
        REQUIRE(compact(wrapped) == compact(text));
    }
}

TEST_CASE("splitTextToWidth preserves words for a range of widths", "[fonts][wrap]")
{
    FontTestFixture const fixture;
    std::string const text =
        "Description: You are the captain of a ship under the flag of England in 1740. "
        "You and your crew were on the way to a colony of England in the Pacific Ocean.";

    for (int32_t width = 60; width <= 320; width += 20) {
        std::string const wrapped = wrapText(text, width);
        INFO("width " << width << " -> " << wrapped);
        REQUIRE(wrapped.find('\0') == std::string::npos);
        REQUIRE(compact(wrapped) == compact(text));
        // Words are only guaranteed to stay intact when they fit on a line.
        if (width >= 120) {
            REQUIRE(words(wrapped) == words(text));
        }
    }
}

TEST_CASE("splitTextToWidth handles edge cases", "[fonts][wrap]")
{
    FontTestFixture const fixture;

    SECTION("empty text is returned unchanged")
    {
        REQUIRE(wrapText("", 100).empty());
    }

    SECTION("non positive width returns the input")
    {
        REQUIRE(wrapText("hello world", 0) == "hello world");
        REQUIRE(wrapText("hello world", -5) == "hello world");
    }

    SECTION("consecutive newlines are kept")
    {
        std::string const wrapped = wrapText("a\n\nb", 200);
        REQUIRE(wrapped.find('\0') == std::string::npos);
        REQUIRE(wrapped == "a\n\nb");
    }

    SECTION("newline right after a space does not repeat the word")
    {
        std::string const wrapped = wrapText("alpha beta \ngamma", 200);
        REQUIRE(wrapped.find('\0') == std::string::npos);
        REQUIRE(wrapped.find("beta \ngamma") != std::string::npos);
        REQUIRE(countOccurrences(wrapped, "beta") == 1);
    }
}
