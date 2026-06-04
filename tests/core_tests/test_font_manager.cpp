// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes
#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "fixture.h"
#include "video/fonts/assetresolver.h"
#include "video/fonts/fontfacecache.h"
#include "video/fonts/fontfamily.h"
#include "video/fonts/fontinstance.h"
#include "video/fonts/fontmanager.h"
#include "video/fonts/fonttypes.h"
#include "video/fonts/imagefontface.h"
#include "video/fonts/truetypefontface.h"

using FIFE::AssetHandle;
using FIFE::AssetProvider;
using FIFE::AssetRequest;
using FIFE::AssetResolver;
using FIFE::FONT_HANDLE_INVALID;
using FIFE::FontFace;
using FIFE::FontFaceCache;
using FIFE::FontFaceKey;
using FIFE::FontHandle;
using FIFE::FontInstance;
using FIFE::FontInstanceKey;
using FIFE::FontManager;
using FIFE::FontWeight;

// ---------------------------------------------------------------------------
// FontFaceCache tests
// ---------------------------------------------------------------------------

TEST_CASE("FontFaceCache put and get")
{
    FontFaceCache cache;

    FontFaceKey const k{.asset = AssetHandle{42}, .ptsize = 0};
    auto face = std::make_shared<FontFace>(AssetHandle{42});

    cache.put(k, face);
    REQUIRE(cache.has(k));
    REQUIRE(cache.get(k) == face);
    REQUIRE(cache.size() == 1);
}

TEST_CASE("FontFaceCache put overwrites existing entry")
{
    FontFaceCache cache;

    FontFaceKey const k{.asset = AssetHandle{1}, .ptsize = 0};
    auto face1 = std::make_shared<FontFace>(AssetHandle{1});
    auto face2 = std::make_shared<FontFace>(AssetHandle{1});

    cache.put(k, face1);
    cache.put(k, face2);
    REQUIRE(cache.size() == 1);
    REQUIRE(cache.get(k) == face2);
}

TEST_CASE("FontFaceCache has returns false for missing key")
{
    FontFaceCache cache;
    REQUIRE_FALSE(cache.has(FontFaceKey{.asset = AssetHandle{999}, .ptsize = 0}));
}

TEST_CASE("FontFaceCache get missing key throws")
{
    FontFaceCache cache;
    REQUIRE_THROWS_AS(cache.get(FontFaceKey{.asset = AssetHandle{999}, .ptsize = 0}), std::out_of_range);
}

TEST_CASE("FontFaceCache put null face throws")
{
    FontFaceCache cache;
    REQUIRE_THROWS_AS(cache.put(FontFaceKey{.asset = AssetHandle{1}, .ptsize = 0}, nullptr), std::invalid_argument);
}

TEST_CASE("FontFaceCache remove")
{
    FontFaceCache cache;

    FontFaceKey const k{.asset = AssetHandle{1}, .ptsize = 0};
    cache.put(k, std::make_shared<FontFace>(AssetHandle{1}));
    REQUIRE(cache.has(k));

    cache.remove(k);
    REQUIRE_FALSE(cache.has(k));
    REQUIRE(cache.size() == 0);
}

TEST_CASE("FontFaceCache remove missing key does nothing")
{
    FontFaceCache cache;
    REQUIRE_NOTHROW(cache.remove(FontFaceKey{.asset = AssetHandle{999}, .ptsize = 0}));
}

TEST_CASE("FontFaceCache clear")
{
    FontFaceCache cache;

    cache.put(FontFaceKey{.asset = AssetHandle{1}, .ptsize = 0}, std::make_shared<FontFace>(AssetHandle{1}));
    cache.put(FontFaceKey{.asset = AssetHandle{2}, .ptsize = 0}, std::make_shared<FontFace>(AssetHandle{2}));
    REQUIRE(cache.size() == 2);

    cache.clear();
    REQUIRE(cache.size() == 0);
    REQUIRE_FALSE(cache.has(FontFaceKey{.asset = AssetHandle{1}, .ptsize = 0}));
}

TEST_CASE("FontFaceCache empty cache size is zero")
{
    FontFaceCache cache;
    REQUIRE(cache.size() == 0);
}

// ---------------------------------------------------------------------------
// Helper: a test asset provider returning sequential handles
// ---------------------------------------------------------------------------

namespace
{

    class TestAssetProvider : public AssetProvider
    {
        public:
            bool canResolve(AssetRequest const &) const override
            {
                return true;
            }
            AssetHandle resolve(AssetRequest const &) const override
            {
                static uint64_t nextId = 100;
                return AssetHandle{nextId++};
            }
    };

} // anonymous namespace

// ---------------------------------------------------------------------------
// FontManager tests
// ---------------------------------------------------------------------------

TEST_CASE("FontManager null resolver throws")
{
    REQUIRE_THROWS_AS(FontManager(nullptr), std::invalid_argument);
}

TEST_CASE("FontManager hasFamily returns false for unknown")
{
    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    REQUIRE_FALSE(mgr.hasFamily("Unknown"));
}

TEST_CASE("FontManager getFamily throws for unknown")
{
    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    REQUIRE_THROWS_AS(mgr.getFamily("Unknown"), std::out_of_range);
}

TEST_CASE("FontManager loadManifestFromString creates families")
{
    FontTestFixture const fixture;
    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">tests/data/FreeMono.ttf</face>
  </family>
</fonts>)";

    REQUIRE_NOTHROW(mgr.loadManifestFromString(xml));
    REQUIRE(mgr.hasFamily("UI"));
    CHECK(mgr.getFaceCacheSize() > 0);
}

TEST_CASE("FontManager loadManifest with missing file throws")
{
    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    REQUIRE_THROWS_AS(mgr.loadManifest("nonexistent.xml"), std::exception);
}

TEST_CASE("FontManager getFontHandle caches same key")
{
    FontTestFixture const fixture;

    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">tests/data/FreeMono.ttf</face>
  </family>
</fonts>)";

    mgr.loadManifestFromString(xml);

    FontHandle h1 = mgr.getFontHandle("UI", 12);
    FontHandle h2 = mgr.getFontHandle("UI", 12);

    CHECK(h1 == h2);
    CHECK(mgr.getInstanceCacheSize() == 1);
}

TEST_CASE("FontManager getFontHandle different size gives different handle")
{
    FontTestFixture const fixture;

    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">tests/data/FreeMono.ttf</face>
  </family>
</fonts>)";

    mgr.loadManifestFromString(xml);

    FontHandle h1 = mgr.getFontHandle("UI", 12);
    FontHandle h2 = mgr.getFontHandle("UI", 14);

    CHECK(h1 != h2);
    CHECK(mgr.getInstanceCacheSize() == 2);
}

TEST_CASE("FontManager getFontInstance returns valid instance")
{
    FontTestFixture const fixture;

    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">tests/data/FreeMono.ttf</face>
  </family>
</fonts>)";

    mgr.loadManifestFromString(xml);

    FontHandle h  = mgr.getFontHandle("UI", 12);
    auto instance = mgr.getFontInstance(h);

    REQUIRE(instance != nullptr);
    CHECK(instance->getSize() == 12);
}

TEST_CASE("FontManager getFontInstance invalid handle throws")
{
    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    REQUIRE_THROWS_AS(mgr.getFontInstance(999), std::out_of_range);
}

TEST_CASE("FontManager getFontHandle unknown family throws")
{
    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    REQUIRE_THROWS_AS(mgr.getFontHandle("Unknown", 12), std::runtime_error);
}

TEST_CASE("FontManager clearInstanceCache resets state")
{
    FontTestFixture const fixture;

    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">tests/data/FreeMono.ttf</face>
  </family>
</fonts>)";

    mgr.loadManifestFromString(xml);

    mgr.getFontHandle("UI", 12);
    CHECK(mgr.getInstanceCacheSize() == 1);

    mgr.clearInstanceCache();
    CHECK(mgr.getInstanceCacheSize() == 0);

    // After clear, getting the same key should produce a fresh handle
    FontHandle h = mgr.getFontHandle("UI", 12);
    CHECK(h != FONT_HANDLE_INVALID);
    CHECK(mgr.getInstanceCacheSize() == 1);
}

TEST_CASE("FontManager face cache size reflects loaded fonts")
{
    FontTestFixture const fixture;
    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    std::string const xml =
        R"(<?xml version="1.0"?>
<fonts version="1">
  <family id="UI">
    <face weight="400">tests/data/FreeMono.ttf</face>
  </family>
  <family id="Mono">
    <face weight="400">tests/data/FreeMono.ttf</face>
  </family>
</fonts>)";

    mgr.loadManifestFromString(xml);
    CHECK(mgr.getFaceCacheSize() == 2);
}

TEST_CASE("FontManager non-copyable")
{
    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    CHECK_FALSE(std::is_copy_constructible_v<FontManager>);
    CHECK_FALSE(std::is_copy_assignable_v<FontManager>);
}

TEST_CASE("FontManager instance cache clear invalidates handles")
{
    FontTestFixture const fixture;

    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    mgr.loadManifestFromString(
        R"(<fonts><family id="TTF"><face weight="400">tests/data/FreeMono.ttf</face></family></fonts>)");

    auto handle = mgr.getFontHandle("TTF", 12);
    REQUIRE(handle != 0);

    auto inst = mgr.getFontInstance(handle);
    REQUIRE(inst != nullptr);

    mgr.clearInstanceCache();

    REQUIRE_THROWS_AS(mgr.getFontInstance(handle), std::exception);
}

TEST_CASE("FontManager getFontHandle with asset not in face cache throws")
{
    auto resolver = std::make_unique<AssetResolver>();
    resolver->addProvider(std::make_unique<TestAssetProvider>());
    FontManager mgr(std::move(resolver));

    FontInstanceKey key;
    key.asset.id = 999;
    key.size     = 12;

    REQUIRE_THROWS_AS(mgr.getFontHandle(key), std::exception);
}
