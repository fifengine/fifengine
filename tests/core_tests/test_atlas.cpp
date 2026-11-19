// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// 3rd party library includes
#include <catch2/catch_test_macros.hpp>

// Standard C++ library includes
#include <cstdint>
#include <string>
#include <vector>

// Platform specific includes
#include <SDL3/SDL.h>

// FIFE includes
#include "loaders/native/map/atlasloader.h"
#include "util/structures/rect.h"
#include "video/image.h"

namespace
{
    /** Minimal concrete Image so AtlasData entries can hold non-null images. */
    class TestImage final : public FIFE::Image
    {
        public:
            explicit TestImage(std::string const & name) : FIFE::Image(name)
            {
                ;
            }

            void invalidate() override {}

            void render(FIFE::Rect const &, uint8_t, uint8_t const *) override {}

            void setSurface(SDL_Surface*) override {}

            void useSharedImage(FIFE::ImagePtr const &, FIFE::Rect const &) override {}

            void forceLoadInternal() override {}
    };
} // namespace

TEST_CASE("Atlas::getImage(uint32_t) returns the stored subimage for valid indices", "[atlas]")
{
    FIFE::Atlas atlas("test-atlas");
    REQUIRE(atlas.getImageCount() == 0);

    // std::map is ordered by key and the "subN" names sort in insertion order
    std::vector<FIFE::ImagePtr> images;
    for (uint32_t i = 0; i < 3; ++i) {
        std::string const name = "sub" + std::to_string(i);
        FIFE::ImagePtr image(new TestImage(name));
        images.push_back(image);
        FIFE::AtlasData data{.rect = FIFE::Rect(static_cast<int>(i), 0, 4, 4), .image = image};
        REQUIRE(atlas.addImage(name, data));
    }

    REQUIRE(atlas.getImageCount() == 3);
    for (uint32_t i = 0; i < images.size(); ++i) {
        REQUIRE(images[i].get() != nullptr);
        CHECK(atlas.getImage(i) == images[i]);
    }
}

TEST_CASE("Atlas::getImage(uint32_t) returns null for out of range indices", "[atlas]")
{
    // An empty atlas has no valid indices at all, including 0
    FIFE::Atlas empty("test-atlas-empty");
    REQUIRE(empty.getImageCount() == 0);
    CHECK(empty.getImage(0).get() == nullptr);
    CHECK(empty.getImage(1).get() == nullptr);

    FIFE::Atlas atlas("test-atlas-oob");
    FIFE::ImagePtr image(new TestImage("sub0"));
    FIFE::AtlasData data{.rect = FIFE::Rect(0, 0, 4, 4), .image = image};
    REQUIRE(atlas.addImage("sub0", data));
    REQUIRE(atlas.getImageCount() == 1);

    CHECK(atlas.getImage(0) == image);
    // index == count is out of range
    CHECK(atlas.getImage(1).get() == nullptr);
    // and so is anything larger
    CHECK(atlas.getImage(2).get() == nullptr);
    CHECK(atlas.getImage(UINT32_MAX).get() == nullptr);
}
