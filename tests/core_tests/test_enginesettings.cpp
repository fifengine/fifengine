// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "controller/enginesettings.h"

// Standard C++ library includes
#include <algorithm>
#include <cctype>
#include <string>

// Platform specific includes
#include <SDL3/SDL.h>

#include <catch2/catch_test_macros.hpp>

using FIFE::EngineSettings;

namespace
{
    std::string getFirstAvailableVideoDriver()
    {
        // SDL compiles in at least the dummy driver, but guard anyway.
        if (SDL_GetNumVideoDrivers() > 0) {
            return SDL_GetVideoDriver(0);
        }
        return std::string();
    }

    bool sdlProvidesDriver(char const * name)
    {
        for (int i = 0, count = SDL_GetNumVideoDrivers(); i != count; ++i) {
            if (SDL_strcasecmp(SDL_GetVideoDriver(i), name) == 0) {
                return true;
            }
        }
        return false;
    }
} // namespace

TEST_CASE("EngineSettings setVideoDriver stores a valid driver name", "[core][enginesettings]")
{
    EngineSettings settings;
    std::string const valid = getFirstAvailableVideoDriver();
    if (!valid.empty()) {
        settings.setVideoDriver(valid);
        CHECK(settings.getVideoDriver() == valid);
    }
}

TEST_CASE("EngineSettings setVideoDriver stores an empty driver name (SDL chooses)", "[core][enginesettings]")
{
    EngineSettings settings;
    settings.setVideoDriver("");
    CHECK(settings.getVideoDriver().empty());
}

TEST_CASE("EngineSettings setVideoDriver rejects an unknown driver", "[core][enginesettings]")
{
    EngineSettings settings;
    settings.setVideoDriver("not-a-real-video-driver");
    CHECK(settings.getVideoDriver().empty());
}

TEST_CASE("EngineSettings setVideoDriver canonicalizes case-insensitive matches", "[core][enginesettings]")
{
    EngineSettings settings;
    std::string const valid = getFirstAvailableVideoDriver();
    if (!valid.empty()) {
        std::string upper = valid;
        std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c) {
            return static_cast<char>(std::toupper(c));
        });
        settings.setVideoDriver(upper);
        CHECK(settings.getVideoDriver() == valid);
    }
}

TEST_CASE("EngineSettings setVideoDriver accepts the dummy driver when SDL provides it", "[core][enginesettings]")
{
    EngineSettings settings;
    if (sdlProvidesDriver("dummy")) {
        settings.setVideoDriver("dummy");
        CHECK(settings.getVideoDriver() == "dummy");
    }
}

TEST_CASE("EngineSettings setVideoDriver replaces a valid value with an invalid one", "[core][enginesettings]")
{
    EngineSettings settings;
    std::string const valid = getFirstAvailableVideoDriver();
    if (!valid.empty()) {
        settings.setVideoDriver(valid);
        CHECK(settings.getVideoDriver() == valid);
        settings.setVideoDriver("not-a-real-video-driver");
        CHECK(settings.getVideoDriver().empty());
    }
}

TEST_CASE("EngineSettings setVideoDriver rejects a comma-separated driver list", "[core][enginesettings]")
{
    // SDL supports comma-separated driver lists in the hint; we validate
    // single names only, so a list reverts to the default ("SDL chooses").
    EngineSettings settings;
    settings.setVideoDriver("wayland,x11");
    CHECK(settings.getVideoDriver().empty());
}
