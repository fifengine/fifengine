// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "devicecaps.h"

// Standard C++ library includes
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <string>

// 3rd party library includes
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

// FIFE includes
#include "renderbackend.h"
#include "util/base/exception.h"

namespace FIFE
{

    DeviceCaps::DeviceCaps() : m_videoDriverName("dummy"), m_renderDriverIndex(-1)
    {
    }

    DeviceCaps::~DeviceCaps() = default;

    void DeviceCaps::reset()
    {
        m_renderDriverName  = "";
        m_renderDriverIndex = -1;

        fillAvailableDrivers();
    }

    void DeviceCaps::fillAvailableDrivers()
    {
        // video driver section (x11, windows, dummy, ...)
        m_availableVideoDrivers.clear();
        int const driverCount = SDL_GetNumVideoDrivers();
        for (int i = 0; i != driverCount; ++i) {
            std::string const driver(SDL_GetVideoDriver(i));
            m_availableVideoDrivers.push_back(driver);
        }
        m_videoDriverName = std::string(SDL_GetCurrentVideoDriver());

        // render driver section (opengl, direct3d, software, ...)
        m_availableRenderDrivers.clear();
        int const renderDriverCount = SDL_GetNumRenderDrivers();
        for (int i = 0; i < renderDriverCount; ++i) {
            char const * name = SDL_GetRenderDriver(i);
            if (name != nullptr) {
                m_availableRenderDrivers.emplace_back(name);
            }
        }
    }

    void DeviceCaps::fillDeviceCaps()
    {
        fillAvailableDrivers();
    }

    void DeviceCaps::setRenderDriverName(std::string const & driver)
    {
        bool found = false;
        assert(m_availableRenderDrivers.size() <= std::numeric_limits<uint8_t>::max());
        uint8_t const driverCount = static_cast<uint8_t>(m_availableRenderDrivers.size());
        for (uint8_t i = 0; i != driverCount; i++) {
            if (driver == m_availableRenderDrivers.at(i)) {
                m_renderDriverName  = driver;
                m_renderDriverIndex = static_cast<int8_t>(i);
                found               = true;
                break;
            }
        }
        if (!found) {
            if (!driver.empty()) {
                throw NotSupported("Could not find a matching render driver!");
            }
            m_renderDriverName  = "";
            m_renderDriverIndex = -1;
        }
        // refill
        fillDeviceCaps();
    }

    uint8_t DeviceCaps::getDisplayCount() const
    {
        int displayCount        = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&displayCount);
        // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)
        SDL_free(displays);
        assert(displayCount >= 0);
        assert(std::cmp_less_equal(displayCount, std::numeric_limits<uint8_t>::max()));
        return static_cast<uint8_t>(displayCount);
    }

    SDL_DisplayID DeviceCaps::getDisplayId(uint8_t display) const
    {
        int count               = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&count);
        if (displays == nullptr || count <= 0 || display >= static_cast<uint8_t>(count)) {
            if (displays != nullptr) {
                // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)
                SDL_free(static_cast<void*>(displays));
            }
            throw NotSupported("Could not find a matching display!");
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        SDL_DisplayID const displayId = displays[display];
        // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)
        SDL_free(displays);
        return displayId;
    }

    SDL_DisplayMode const * DeviceCaps::getDesktopMode(uint8_t display) const
    {
        SDL_DisplayID const displayId = getDisplayId(display);
        SDL_DisplayMode const * mode  = SDL_GetDesktopDisplayMode(displayId);
        if (mode == nullptr) {
            throw SDLException(SDL_GetError());
        }
        return mode;
    }

    std::string DeviceCaps::getDisplayName(uint8_t display) const
    {
        return {SDL_GetDisplayName(getDisplayId(display))};
    }

    uint32_t DeviceCaps::getDesktopFormat(uint8_t display) const
    {
        return getDesktopMode(display)->format;
    }

    int32_t DeviceCaps::getDesktopRefreshRate(uint8_t display) const
    {
        return static_cast<int32_t>(getDesktopMode(display)->refresh_rate);
    }

    int32_t DeviceCaps::getDesktopWidth(uint8_t display) const
    {
        return getDesktopMode(display)->w;
    }

    int32_t DeviceCaps::getDesktopHeight(uint8_t display) const
    {
        return getDesktopMode(display)->h;
    }

    Rect DeviceCaps::getDisplayBounds(uint8_t display) const
    {
        SDL_DisplayID const displayId = getDisplayId(display);

        SDL_Rect srec;
        if (!SDL_GetDisplayBounds(displayId, &srec)) {
            throw SDLException(SDL_GetError());
        }

        // SDL can occasionally report unusable bounds on X11/Xvfb (e.g. height 0).
        // Fall back to desktop mode dimensions to keep window creation valid.
        if (srec.w <= 0 || srec.h <= 0) {
            SDL_DisplayMode const * mode = SDL_GetDesktopDisplayMode(displayId);
            if (mode == nullptr) {
                throw SDLException(SDL_GetError());
            }
            srec.x = 0;
            srec.y = 0;
            srec.w = mode->w;
            srec.h = mode->h;
        }
        Rect rec(srec.x, srec.y, srec.w, srec.h);
        return rec;
    }

} // namespace FIFE
