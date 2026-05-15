// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <type_traits>

// Platform specific includes
#include "fife_unittest.h"

// FIFE includes
#include "eventchannel/key/key.h"

using FIFE::Key;

// ── Static assertions — compile-time only, zero runtime cost ──────────────

static_assert(std::is_same_v<std::underlying_type_t<Key::KeyType>, uint32_t>, "KeyType backing type must be uint32_t");

static_assert(::FIFE::Keys::KEY_UNKNOWN == 0, "KEY_UNKNOWN must be 0");
static_assert(::FIFE::Keys::RETURN == 0x0D, "RETURN must be 0x0D");
static_assert(::FIFE::Keys::ESCAPE == 27, "ESCAPE must be 27");
static_assert(::FIFE::Keys::SPACE == 32, "SPACE must be 32");
static_assert(::FIFE::Keys::A == 0x61, "A must be 0x61");
static_assert(::FIFE::Keys::Z == 0x7A, "Z must be 0x7A");
static_assert(::FIFE::Keys::F1 == 0x4000003Au, "F1 must be 0x4000003A");
static_assert(::FIFE::Keys::F24 == 0x40000073u, "F24 must be 0x40000073");

// ── Test cases ────────────────────────────────────────────────────────────

TEST_CASE("KeyMappingCompleteness: well-known values match SDL3", "[key]")
{
    CHECK(::FIFE::Keys::KEY_UNKNOWN == 0);
    CHECK(::FIFE::Keys::RETURN == 13);
    CHECK(::FIFE::Keys::ESCAPE == 27);
    CHECK(::FIFE::Keys::BACKSPACE == 8);
    CHECK(::FIFE::Keys::TAB == 9);
    CHECK(::FIFE::Keys::SPACE == 32);
    CHECK(::FIFE::Keys::A == 0x61);
    CHECK(::FIFE::Keys::Z == 0x7A);
    CHECK(::FIFE::Keys::F1 == 0x4000003A);
    CHECK(::FIFE::Keys::F24 == 0x40000073);
    CHECK(::FIFE::Keys::KEY_DELETE == 0x7F);
    CHECK(::FIFE::Keys::KEY_LESS == 0x3C);
    CHECK(::FIFE::Keys::KEY_GREATER == 0x3E);
}

TEST_CASE("Key: construction and value access", "[key]")
{
    Key k(::FIFE::Keys::ESCAPE);
    CHECK(k.getValue() == ::FIFE::Keys::ESCAPE);
    CHECK(k.getValue() == 27);

    Key defaultKey;
    CHECK(defaultKey.getValue() == ::FIFE::Keys::KEY_UNKNOWN);
}

TEST_CASE("Key: isFunctionKey range", "[key]")
{
    CHECK(Key(::FIFE::Keys::F1).isFunctionKey());
    CHECK(Key(::FIFE::Keys::F10).isFunctionKey());
    CHECK(Key(::FIFE::Keys::F24).isFunctionKey());
    CHECK_FALSE(Key(::FIFE::Keys::A).isFunctionKey());
    CHECK_FALSE(Key(::FIFE::Keys::SPACE).isFunctionKey());
}

TEST_CASE("Key::get_name and isPrintable", "[key]")
{
    Key esc(::FIFE::Keys::ESCAPE);
    CHECK_FALSE(esc.get_name().empty());
    CHECK(esc.get_name() == esc.getAsString());

    Key a(::FIFE::Keys::A);
    CHECK(a.isPrintable());
    CHECK(a.is_printable());
}

// ── Static assertion: ensure no SDLK_* values map to 0 except KEY_UNKNOWN ─

TEST_CASE("KeyMappingCompleteness: no unexpected zero values", "[key]")
{
    // SDLK_UNKNOWN is deliberately 0. No other key should be 0.
    // Spot-check a handful of well-known keys.
    CHECK(::FIFE::Keys::RETURN != 0);
    CHECK(::FIFE::Keys::A != 0);
    CHECK(::FIFE::Keys::F1 != 0);
    CHECK(::FIFE::Keys::LEFT != 0);
    CHECK(::FIFE::Keys::LCTRL != 0);
    CHECK(::FIFE::Keys::KP_0 != 0);
    CHECK(::FIFE::Keys::VOLUMEUP != 0);
    CHECK(::FIFE::Keys::MEDIA_PLAY != 0);
    CHECK(::FIFE::Keys::AC_SEARCH != 0);
}
