// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdint>
#include <type_traits>

// Platform specific includes
#include "fife_unittest.h"

// FIFE includes
#include "video/fonts/fonttypes.h"

using FIFE::FONT_HANDLE_INVALID;
using FIFE::FontHandle;

TEST_CASE("AssetHandle default id is 0")
{
    FIFE::AssetHandle const h;
    REQUIRE(h.id == 0);
}

TEST_CASE("Two AssetHandles with same id are equal")
{
    FIFE::AssetHandle a{42};
    FIFE::AssetHandle b{42};
    CHECK(a == b);
}

TEST_CASE("FontHandle is uint32_t")
{
    static_assert(std::is_same_v<FontHandle, uint32_t>);
}

TEST_CASE("FontInstanceKey with same fields is equal")
{
    FIFE::FontInstanceKey k1;
    FIFE::FontInstanceKey k2;
    CHECK(k1 == k2);
}

TEST_CASE("FontInstanceKey with different size is not equal")
{
    FIFE::FontInstanceKey k1;
    FIFE::FontInstanceKey k2;
    k2.size = 16;
    CHECK(k1 != k2);
}

TEST_CASE("FontFaceKey wrapping same AssetHandle is equal")
{
    FIFE::FontFaceKey fk1{.asset = {42}};
    FIFE::FontFaceKey fk2{.asset = {42}};
    CHECK(fk1 == fk2);
}

TEST_CASE("AssetRequest default type is Font")
{
    FIFE::AssetRequest const req;
    CHECK(req.type == FIFE::AssetRequest::Type::Font);
}

TEST_CASE("FontDefinition has default size 12")
{
    FIFE::FontDefinition const fd;
    CHECK(fd.size == 12);
}

TEST_CASE("FontHandle INVALID is 0")
{
    CHECK(FONT_HANDLE_INVALID == 0);
}

TEST_CASE("Hash of same keys produces same value")
{
    std::hash<FIFE::AssetHandle> const hasher;
    FIFE::AssetHandle const a{42};
    FIFE::AssetHandle const b{42};
    CHECK(hasher(a) == hasher(b));
}

TEST_CASE("Hash of different keys produces different value")
{
    std::hash<FIFE::FontInstanceKey> const hasher;
    FIFE::FontInstanceKey const k1;
    FIFE::FontInstanceKey k2;
    k2.size = 16;
    CHECK(hasher(k1) != hasher(k2));
}
