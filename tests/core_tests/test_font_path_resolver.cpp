// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

// Platform specific includes
#include "fife_unittest.h"
#include "fixture.h"

// FIFE includes
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "video/fonts/fontpathresolver.h"
#include "video/fonts/truetypefont.h"

using FIFE::FontPathResolver;
using FIFE::RawData;
using FIFE::TrueTypeFont;
using FIFE::VFS;
using FIFE::VFSDirectory;

static char const * const FONT_FILE = "tests/data/FreeMono.ttf";
static char const * const FONT_DIR  = "tests/data";

TEST_CASE("FontPathResolver resolves font by filename through search paths", "[font][resolver]")
{
    FontTestFixture const env;
    require_font_renderable(FONT_FILE, 16);

    FontPathResolver resolver;
    resolver.setSearchPaths({FONT_DIR});

    RawData* data = resolver.resolve("FreeMono.ttf");
    REQUIRE(data != nullptr);

    auto font = TrueTypeFont::createFromRawData(data, 16, "FreeMono.ttf");
    REQUIRE(font != nullptr);
    CHECK(font->getHeight() > 0);

    delete data;
    delete font;
}

TEST_CASE("FontPathResolver returns null for unresolvable font", "[font][resolver]")
{
    FontTestFixture const env;

    FontPathResolver resolver;
    resolver.setSearchPaths({FONT_DIR});

    RawData* data = resolver.resolve("nonexistent_font.ttf");
    CHECK(data == nullptr);
}

TEST_CASE("FontPathResolver skips non-font extensions", "[font][resolver]")
{
    FontTestFixture const env;

    FontPathResolver resolver;
    resolver.setSearchPaths({FONT_DIR});

    // .txt files should be rejected by extension filter before any search
    RawData* data = resolver.resolve("FreeMono.ttf.txt");
    CHECK(data == nullptr);
}

TEST_CASE("FontPathResolver caches successful resolutions", "[font][resolver]")
{
    FontTestFixture const env;
    require_font_renderable(FONT_FILE, 16);

    FontPathResolver resolver;
    resolver.setSearchPaths({FONT_DIR});

    // First call — cache miss, searches and populates cache
    RawData* data1 = resolver.resolve("FreeMono.ttf");
    REQUIRE(data1 != nullptr);
    delete data1;

    // Second call — cache hit, returns from cache without VFS search
    RawData* data2 = resolver.resolve("FreeMono.ttf");
    REQUIRE(data2 != nullptr);
    CHECK(data2->getDataLength() > 0);
    delete data2;
}

TEST_CASE("FontPathResolver searches paths in order", "[font][resolver]")
{
    FontTestFixture const env;
    require_font_renderable(FONT_FILE, 16);

    // Create a temp directory with a copy of the font
    std::filesystem::create_directories("test_output/resolver_test");
    std::string const tempFontPath = "test_output/resolver_test/FreeMono.ttf";
    std::filesystem::copy_file(FONT_FILE, tempFontPath, std::filesystem::copy_options::overwrite_existing);

    FontPathResolver resolver;
    // Order matters: second path contains the font
    resolver.setSearchPaths({"/nonexistent/path", "test_output/resolver_test"});

    RawData* data = resolver.resolve("FreeMono.ttf");
    REQUIRE(data != nullptr);
    CHECK(data->getDataLength() > 0);
    delete data;

    // Now reverse order: first path contains the font
    resolver.clearCache();
    resolver.setSearchPaths({"test_output/resolver_test", "/nonexistent/path"});

    RawData* data2 = resolver.resolve("FreeMono.ttf");
    REQUIRE(data2 != nullptr);
    CHECK(data2->getDataLength() > 0);
    delete data2;

    std::filesystem::remove(tempFontPath);
}

TEST_CASE("FontPathResolver returns null with empty search paths", "[font][resolver]")
{
    FontTestFixture const env;

    FontPathResolver resolver;
    resolver.setSearchPaths({});

    RawData* data = resolver.resolve("FreeMono.ttf");
    CHECK(data == nullptr);
}

TEST_CASE("FontPathResolver handles .ttc extension", "[font][resolver]")
{
    FontTestFixture const env;

    FontPathResolver resolver;
    resolver.setSearchPaths({FONT_DIR});

    // Should not be rejected by extension filter even though file doesn't exist
    RawData* data = resolver.resolve("fontcollection.ttc");
    CHECK(data == nullptr);
}

TEST_CASE("TrueTypeFont::createFromRawData works with VFS-loaded font", "[font][ttf]")
{
    FontTestFixture const env;
    require_font_renderable(FONT_FILE, 16);

    // Load font through VFS as a resolver would
    VFS* vfs          = VFS::instance();
    RawData* fontData = vfs->open(FONT_FILE);
    REQUIRE(fontData != nullptr);

    auto font = TrueTypeFont::createFromRawData(fontData, 16, FONT_FILE);
    REQUIRE(font != nullptr);
    CHECK(font->getHeight() > 0);

    // Verify it renders
    font->setColor(255, 255, 255, 255);
    SDL_Surface* surface = font->renderString("VFS Font Test");
    REQUIRE(surface != nullptr);
    CHECK(surface->w > 0);

    SDL_DestroySurface(surface);
    delete font;
    delete fontData;
}

TEST_CASE("FontPathResolver cache is cleared when search paths change", "[font][resolver]")
{
    FontTestFixture const env;
    require_font_renderable(FONT_FILE, 16);

    FontPathResolver resolver;
    resolver.setSearchPaths({FONT_DIR});

    // Populate cache with a successful resolution
    RawData* data1 = resolver.resolve("FreeMono.ttf");
    REQUIRE(data1 != nullptr);
    delete data1;

    // Changing paths should clear cache — this file won't be found in empty paths
    resolver.setSearchPaths({});

    RawData* data2 = resolver.resolve("FreeMono.ttf");
    CHECK(data2 == nullptr);
}

TEST_CASE("FontPathResolver lazy registers VFS sources", "[font][resolver]")
{
    FontTestFixture const env;

    VFS* vfs = VFS::instance();

    // The path should not already be a VFS source
    bool alreadySource = vfs->hasSource(FONT_DIR);
    if (alreadySource) {
        // Remove it to test registration
        vfs->removeSource(FONT_DIR);
    }
    CHECK(!vfs->hasSource(FONT_DIR));

    FontPathResolver resolver;
    resolver.setSearchPaths({FONT_DIR});

    // After resolve call, the path should be lazily registered
    RawData* data = resolver.resolve("FreeMono.ttf");
    REQUIRE(data != nullptr);
    CHECK(vfs->hasSource(FONT_DIR));
    delete data;

    // Re-add the source if we removed it
    if (!alreadySource) {
        vfs->addNewSource(FONT_DIR);
    }
}
