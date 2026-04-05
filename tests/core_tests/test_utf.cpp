// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstring>
#include <string>
#include <vector>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes
#include <utf8.h>

// FIFE includes

namespace
{

    std::string make_utf8(const unsigned char* bytes, size_t len)
    {
        return std::string(reinterpret_cast<const char*>(bytes), len);
    }

} // namespace

TEST_CASE("utf8_russian_hello")
{
    // "Привет мир" (Hello world in Russian)
    const unsigned char russian[] = {
        0xD0,
        0x9F,
        0xD1,
        0x80,
        0xD0,
        0xB8,
        0xD0,
        0xB2,
        0xD0,
        0xB5,
        0xD1,
        0x82,
        0x20,
        0xD0,
        0xBC,
        0xD0,
        0xB8,
        0xD1,
        0x80};
    std::string s = make_utf8(russian, sizeof(russian));

    const char* it = s.data();
    CHECK_EQ(utf8::unchecked::next(it), 0x041F); // П
    CHECK_EQ(utf8::unchecked::next(it), 0x0440); // р
    CHECK_EQ(utf8::unchecked::next(it), 0x0438); // и
    CHECK_EQ(utf8::unchecked::next(it), 0x0432); // в
    CHECK_EQ(utf8::unchecked::next(it), 0x0435); // е
    CHECK_EQ(utf8::unchecked::next(it), 0x0442); // т
    CHECK_EQ(utf8::unchecked::distance(s.data(), s.data() + s.size()), 10);
}

TEST_CASE("utf8_french_hello")
{
    // "Bonjour le monde" (Hello world in French)
    const unsigned char french[] = {
        0x42, 0x6f, 0x6e, 0x6a, 0x6f, 0x75, 0x72, 0x20, 0x6c, 0x65, 0x20, 0x6d, 0x6f, 0x6e, 0x64, 0x65};
    std::string s = make_utf8(french, sizeof(french));

    CHECK_EQ(utf8::unchecked::distance(s.data(), s.data() + s.size()), 16);
    CHECK_EQ(utf8::unchecked::peek_next(s.data()), 0x0042); // B
}

TEST_CASE("utf8_french_accents")
{
    // "él" - é = C3 A9, l = 6C
    const unsigned char french[] = {0xc3, 0xa9, 0x6c};
    std::string s                = make_utf8(french, sizeof(french));

    const char* it = s.data();
    CHECK_EQ(utf8::unchecked::next(it), 0x00E9); // é
    CHECK_EQ(utf8::unchecked::next(it), 0x006C); // l
    CHECK_EQ(utf8::unchecked::distance(s.data(), s.data() + s.size()), 2);
}

TEST_CASE("utf8_italian_hello")
{
    // "Ciao mondo" (Hello world in Italian)
    const unsigned char italian[] = {0x43, 0x69, 0x61, 0x6f, 0x20, 0x6d, 0x6f, 0x6e, 0x64, 0x6f};
    std::string s                 = make_utf8(italian, sizeof(italian));

    CHECK_EQ(utf8::unchecked::distance(s.data(), s.data() + s.size()), 10);

    const char* it = s.data();
    CHECK_EQ(utf8::unchecked::next(it), 0x0043); // C
    CHECK_EQ(utf8::unchecked::next(it), 0x0069); // i
    CHECK_EQ(utf8::unchecked::next(it), 0x0061); // a
    CHECK_EQ(utf8::unchecked::next(it), 0x006F); // o
}

TEST_CASE("utf8_hebrew_hello")
{
    // "שלום עולם" (Hello world in Hebrew - RTL)
    const unsigned char hebrew[] = {
        0xd7, 0xa9, 0xd7, 0x9c, 0xd7, 0x95, 0xd7, 0x9d, 0x20, 0xd7, 0xa2, 0xd7, 0x95, 0xd7, 0x9c, 0xd7, 0x9d, 0x00};
    std::string s = make_utf8(hebrew, sizeof(hebrew) - 1);

    CHECK_EQ(utf8::unchecked::distance(s.data(), s.data() + s.size()), 9);

    const char* it = s.data();
    CHECK_EQ(utf8::unchecked::next(it), 0x05E9); // ש
    CHECK_EQ(utf8::unchecked::next(it), 0x05DC); // ל
    CHECK_EQ(utf8::unchecked::next(it), 0x05D5); // ו
    CHECK_EQ(utf8::unchecked::next(it), 0x05DD); // ם
}

TEST_CASE("utf8_japanese_hello")
{
    // "日本" (Japan/Nihon in Japanese)
    const unsigned char japanese[] = {0xe6, 0x97, 0xa5, 0xe6, 0x9c, 0xac};
    std::string s                  = make_utf8(japanese, sizeof(japanese));

    const char* it = s.data();
    CHECK_EQ(utf8::unchecked::next(it), 0x65E5); // 日
    CHECK_EQ(utf8::unchecked::next(it), 0x672C); // 本

    std::vector<uint32_t> codepoints;
    utf8::unchecked::utf8to32(s.data(), s.data() + s.size(), std::back_inserter(codepoints));
    CHECK_EQ(codepoints.size(), 2u);
    CHECK_EQ(codepoints[0], 0x65E5);
    CHECK_EQ(codepoints[1], 0x672C);
}

TEST_CASE("utf8_emoji_wave_globe")
{
    // "👋🌍" (waving hand + earth globe)
    const unsigned char emoji[] = {0xf0, 0x9f, 0x91, 0x8b, 0xf0, 0x9f, 0x8c, 0x8d};
    std::string s               = make_utf8(emoji, sizeof(emoji));

    const char* it = s.data();
    CHECK_EQ(utf8::unchecked::next(it), 0x0001F44B); // 👋
    CHECK_EQ(utf8::unchecked::next(it), 0x0001F30D); // 🌍

    std::vector<uint32_t> codepoints;
    utf8::unchecked::utf8to32(s.data(), s.data() + s.size(), std::back_inserter(codepoints));
    CHECK_EQ(codepoints.size(), 2u);
}

TEST_CASE("utf8_mixed_languages")
{
    // "Hello 世界" - H, e, l, l, o, space, 世, 界
    const unsigned char mixed[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0xe4, 0xb8, 0x96, 0xe7, 0x95, 0x8c};
    std::string s               = make_utf8(mixed, sizeof(mixed));

    CHECK_EQ(utf8::unchecked::distance(s.data(), s.data() + s.size()), 8);

    const char* it = s.data();
    uint32_t cp1   = utf8::unchecked::next(it);
    uint32_t cp2   = utf8::unchecked::next(it);
    uint32_t cp3   = utf8::unchecked::next(it);
    uint32_t cp4   = utf8::unchecked::next(it);
    CHECK_EQ(cp1, 72);
    CHECK_EQ(cp2, 101);
    CHECK_EQ(cp3, 108);
    CHECK_EQ(cp4, 108);
}

TEST_CASE("utf8_append_codepoints")
{
    std::string result;
    utf8::unchecked::append(0x0057, std::back_inserter(result)); // W
    utf8::unchecked::append(0x006f, std::back_inserter(result)); // o
    utf8::unchecked::append(0x0072, std::back_inserter(result)); // r
    utf8::unchecked::append(0x006c, std::back_inserter(result)); // l
    utf8::unchecked::append(0x0064, std::back_inserter(result)); // d

    CHECK_EQ(result, "World");
}

TEST_CASE("utf8_append_russian")
{
    std::string result;
    utf8::unchecked::append(0x041F, std::back_inserter(result)); // П
    utf8::unchecked::append(0x0440, std::back_inserter(result)); // р
    utf8::unchecked::append(0x0438, std::back_inserter(result)); // и
    utf8::unchecked::append(0x0432, std::back_inserter(result)); // в
    utf8::unchecked::append(0x0435, std::back_inserter(result)); // е
    utf8::unchecked::append(0x0442, std::back_inserter(result)); // т

    const unsigned char expected[] = {0xD0, 0x9F, 0xD1, 0x80, 0xD0, 0xB8, 0xD0, 0xB2, 0xD0, 0xB5, 0xD1, 0x82};
    CHECK_EQ(result.size(), 12u);
    CHECK_EQ(std::memcmp(result.data(), expected, 12), 0);
}

TEST_CASE("utf8_roundtrip_russian")
{
    const unsigned char russian[] = {0xD0, 0x9F, 0xD1, 0x80, 0xD0, 0xB8, 0xD0, 0xB2, 0xD0, 0xB5, 0xD1, 0x82};
    std::string s                 = make_utf8(russian, sizeof(russian));

    std::vector<uint32_t> utf32;
    utf8::unchecked::utf8to32(s.data(), s.data() + s.size(), std::back_inserter(utf32));

    std::string result;
    utf8::unchecked::utf32to8(utf32.data(), utf32.data() + utf32.size(), std::back_inserter(result));

    CHECK_EQ(result.size(), s.size());
    CHECK_EQ(std::memcmp(result.data(), s.data(), s.size()), 0);
}

TEST_CASE("utf8_prior_navigation")
{
    // Navigate backwards through "日本語"
    const unsigned char text[] = {0xe6, 0x97, 0xa5, 0xe6, 0x9c, 0xac, 0xe8, 0xaa, 0x9e};
    std::string s              = make_utf8(text, sizeof(text));
    const char* it             = s.data() + s.size();

    CHECK_EQ(utf8::unchecked::prior(it), 0x8A9E); // 語
    CHECK_EQ(utf8::unchecked::prior(it), 0x672C); // 本
    CHECK_EQ(utf8::unchecked::prior(it), 0x65E5); // 日
}

TEST_CASE("utf8_advance_through_languages")
{
    // "HelloПри" - 5 ASCII + 2 Cyrillic (each 2 bytes)
    const unsigned char text[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0xD0, 0x9F, 0xD1, 0x80};
    std::string s              = make_utf8(text, sizeof(text));

    // "HelloПри" = H(0), e(1), l(2), l(3), o(4), П(5), р(6)
    const char* it1 = s.data();
    utf8::unchecked::advance(it1, 5);
    uint32_t cp1 = utf8::unchecked::next(it1);
    CHECK_EQ(cp1, 1055); // П = 0x041F

    const char* it2 = s.data();
    utf8::unchecked::advance(it2, 6);
    uint32_t cp2 = utf8::unchecked::next(it2);
    CHECK_EQ(cp2, 1088); // р = 0x0440
}
