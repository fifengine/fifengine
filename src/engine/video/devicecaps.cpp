// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "devicecaps.h"

// Standard C++ library includes
#include <algorithm>
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

    ScreenMode::ScreenMode() :
        m_width(0),
        m_height(0),
        m_bpp(0),
        m_refreshRate(0),
        m_SDLFlags(0),
        m_format(0),
        m_display(0),
        m_windowX(-1),
        m_windowY(-1),

        m_renderDriverIndex(-1)
    {
    }

    ScreenMode::ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t SDLFlags) :
        m_width(width),
        m_height(height),
        m_bpp(bpp),
        m_refreshRate(0),
        m_SDLFlags(SDLFlags),
        m_format(0),
        m_display(0),
        m_windowX(-1),
        m_windowY(-1),

        m_renderDriverIndex(-1)
    {
    }

    ScreenMode::ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint16_t rate, uint32_t SDLFlags) :
        m_width(width),
        m_height(height),
        m_bpp(bpp),
        m_refreshRate(rate),
        m_SDLFlags(SDLFlags),
        m_format(0),
        m_display(0),
        m_windowX(-1),
        m_windowY(-1),

        m_renderDriverIndex(-1)
    {
    }

    ScreenMode::ScreenMode(ScreenMode const & rhs) :
        m_width(rhs.getWidth()),
        m_height(rhs.getHeight()),
        m_bpp(rhs.getBPP()),
        m_refreshRate(rhs.getRefreshRate()),
        m_SDLFlags(rhs.getSDLFlags()),
        m_format(rhs.getFormat()),
        m_display(rhs.getDisplay()),
        m_windowX(rhs.getWindowPositionX()),
        m_windowY(rhs.getWindowPositionY()),
        m_renderDriver(rhs.getRenderDriverName()),
        m_renderDriverIndex(rhs.getRenderDriverIndex())
    {
    }

    bool ScreenMode::operator<(ScreenMode const & rhs) const
    {
        // first by display, from lower to higher
        if (m_display > rhs.getDisplay()) {
            return true;
        }
        if (m_display < rhs.getDisplay()) {
            return false;
        }

        // sort by fullscreen first
        if (!isFullScreen() && rhs.isFullScreen()) {
            return true;
        }
        if (isFullScreen() && !rhs.isFullScreen()) {
            return false;
        }

        // next by bpp
        if (m_bpp < rhs.getBPP()) {
            return true;
        }
        if (m_bpp > rhs.getBPP()) {
            return false;
        }

        // then by screen dimensions
        if (m_width < rhs.getWidth() || m_height < rhs.getHeight()) {
            return true;
        }
        if (m_width > rhs.getWidth() || m_height > rhs.getHeight()) {
            return false;
        }
        // last by refresh rate
        if (m_refreshRate < rhs.getRefreshRate()) {
            return true;
        }

        return false;
    }

    DeviceCaps::DeviceCaps() : m_videoDriverName("dummy"), m_renderDriverIndex(-1)
    {
    }

    DeviceCaps::~DeviceCaps() = default;

    void DeviceCaps::reset()
    {
        m_screenModes.clear();
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
            if (name) {
                m_availableRenderDrivers.push_back(std::string(name));
            }
        }
    }

    void DeviceCaps::fillDeviceCaps()
    {
        // clear in case this is called twice
        m_screenModes.clear();
        fillAvailableDrivers();
        // FLAGS
#ifdef HAVE_OPENGL
        constexpr uint32_t kNumFlags = 4;
        uint32_t flags[kNumFlags];

        // OpenGL, windowed
        flags[0] = SDL_WINDOW_OPENGL;
        // OpenGL, fullscreen
        flags[1] = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
        // SDL, windowed
        flags[2] = 0;
        // SDL, fullscreen
        flags[3] = SDL_WINDOW_FULLSCREEN;

#else
        constexpr uint32_t kNumFlags = 2;
        uint32_t flags[kNumFlags];

        // SDL, windowed
        flags[0] = 0;
        // SDL, fullscreen
        flags[1] = SDL_WINDOW_FULLSCREEN;
#endif

        // BITS PER PIXEL
        constexpr uint32_t kNumBPP = 3;
        int16_t bpps[kNumBPP];

        bpps[0] = 16;
        bpps[1] = 24;
        bpps[2] = 32;

        bool const renderDriver = m_renderDriverIndex != -1;
        int displayCount        = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&displayCount);
        for (int i = 0; i < displayCount; ++i) {
            int numModes;
            SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displays[i], &numModes);
            for (int m = 0; m < numModes; ++m) {
                SDL_DisplayMode mode = *modes[m];
                for (int16_t const bpp : bpps) {
                    for (unsigned int const flag : flags) {
                        // m_screenModes.push_back(ScreenMode(mode.w, mode.h, SDL_BITSPERPIXEL(mode.format),
                        // mode.refresh_rate, flags[j]));
                        uint16_t const modeW = static_cast<uint16_t>(
                            std::clamp(mode.w, 0, static_cast<int>(std::numeric_limits<uint16_t>::max())));
                        uint16_t const modeH = static_cast<uint16_t>(
                            std::clamp(mode.h, 0, static_cast<int>(std::numeric_limits<uint16_t>::max())));
                        uint16_t const modeRefresh = static_cast<uint16_t>(std::clamp(
                            static_cast<int>(mode.refresh_rate),
                            0,
                            static_cast<int>(std::numeric_limits<uint16_t>::max())));
                        ScreenMode sm(modeW, modeH, static_cast<uint16_t>(bpp), modeRefresh, flag);
                        sm.setFormat(mode.format);
                        assert(i <= std::numeric_limits<uint8_t>::max());
                        sm.setDisplay(static_cast<uint8_t>(i));
                        if (renderDriver) {
                            sm.setRenderDriverName(m_renderDriverName);
                            sm.setRenderDriverIndex(m_renderDriverIndex);
                        }
                        m_screenModes.push_back(sm);
                    }
                }
            }
            SDL_free(modes);
        }
        SDL_free(displays);

        // sort the list to keep the most preferred modes at the top of the selection process
        std::ranges::sort(m_screenModes, [](ScreenMode const & lhs, ScreenMode const & rhs) {
            return lhs < rhs;
        });
        std::ranges::reverse(m_screenModes);
    }

    ScreenMode DeviceCaps::getNearestScreenMode(
        uint16_t width, uint16_t height, uint16_t bpp, std::string const & renderer, bool fs) const
    {
        // refresh rate is set to 0 so that desktop setting is used and the first display is used
        return getNearestScreenMode(width, height, bpp, renderer, fs, 0, 0);
    }

    ScreenMode DeviceCaps::getNearestScreenMode(
        uint16_t width,
        uint16_t height,
        uint16_t bpp,
        std::string const & renderer,
        bool fs,
        uint16_t refresh,
        uint8_t display) const
    {
        ScreenMode mode;
        uint32_t flags = 0;
        if (renderer == "OpenGL") {
            if (fs) {
                flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
            } else {
                flags = SDL_WINDOW_OPENGL;
            }
        } else {
            if (fs) {
                flags = SDL_WINDOW_FULLSCREEN;
            }
        }

        if (!fs) {
            SDL_DisplayID displayId = SDL_GetDisplayForWindow(RenderBackend::instance()->getWindow());
            if (displayId == 0) {
                displayId = SDL_GetPrimaryDisplay();
            }
            SDL_DisplayMode const * desktopMode = SDL_GetDesktopDisplayMode(displayId);
            if (desktopMode == nullptr) {
                throw SDLException(SDL_GetError());
            }

            // Keep requested dimensions in windowed mode.
            // SDL_GetClosestFullscreenDisplayMode() can pick a desktop/fullscreen-sized mode and unexpectedly enlarge
            // windowed windows (for example across a virtual combined desktop).
            uint16_t const desktopRefresh = static_cast<uint16_t>(std::clamp(
                static_cast<int>(desktopMode->refresh_rate),
                0,
                static_cast<int>(std::numeric_limits<uint16_t>::max())));
            mode                          = ScreenMode(width, height, bpp, desktopRefresh, flags);
            mode.setFormat(desktopMode->format);
            mode.setDisplay(display);
            if (m_renderDriverIndex != -1) {
                mode.setRenderDriverName(m_renderDriverName);
                mode.setRenderDriverIndex(m_renderDriverIndex);
            }
            return mode;
        }

        SDL_DisplayMode target;

        // Set the desired fullscreen resolution, etc.
        target.w = width;
        target.h = height;
        if (bpp == 0) {
            target.format = SDL_PIXELFORMAT_UNKNOWN; // don't care, should be desktop bpp
        } else if (bpp == 16) {
            target.format = SDL_PIXELFORMAT_RGB565;
        } else {
            target.format = SDL_PIXELFORMAT_XRGB8888;
        }
        target.refresh_rate = refresh;

        SDL_DisplayID displayId = SDL_GetDisplayForWindow(RenderBackend::instance()->getWindow());
        if (displayId == 0) {
            displayId = SDL_GetPrimaryDisplay();
        }
        SDL_DisplayMode closest;
        if (!SDL_GetClosestFullscreenDisplayMode(
                displayId, width, height, static_cast<float>(refresh), false, &closest)) {
            throw NotSupported("Could not find a matching screen mode for the values given!");
        }

        uint16_t const closestW =
            static_cast<uint16_t>(std::clamp(closest.w, 0, static_cast<int>(std::numeric_limits<uint16_t>::max())));
        uint16_t const closestH =
            static_cast<uint16_t>(std::clamp(closest.h, 0, static_cast<int>(std::numeric_limits<uint16_t>::max())));
        uint16_t const closestRefresh = static_cast<uint16_t>(std::clamp(
            static_cast<int>(closest.refresh_rate), 0, static_cast<int>(std::numeric_limits<uint16_t>::max())));
        mode                          = ScreenMode(closestW, closestH, bpp, closestRefresh, flags);
        mode.setFormat(closest.format);
        mode.setDisplay(display);
        if (m_renderDriverIndex != -1) {
            mode.setRenderDriverName(m_renderDriverName);
            mode.setRenderDriverIndex(m_renderDriverIndex);
        }

        return mode;
    }

    void DeviceCaps::setRenderDriverName(std::string const & driver)
    {
        bool found = false;
        assert(m_availableRenderDrivers.size() <= std::numeric_limits<uint8_t>::max());
        uint8_t const driverCount = static_cast<uint8_t>(m_availableRenderDrivers.size());
        for (uint8_t i = 0; i != driverCount; i++) {
            if (driver == m_availableRenderDrivers[i]) {
                m_renderDriverName  = driver;
                m_renderDriverIndex = i;
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
        SDL_free(displays);
        assert(displayCount >= 0);
        assert(displayCount <= std::numeric_limits<uint8_t>::max());
        return static_cast<uint8_t>(displayCount);
    }

    std::string DeviceCaps::getDisplayName(uint8_t display) const
    {
        if (display >= getDisplayCount()) {
            throw NotSupported("Could not find a matching display!");
        }
        int count               = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&count);
        std::string displayName(SDL_GetDisplayName(displays[display]));
        SDL_free(displays);
        return displayName;
    }

    uint32_t DeviceCaps::getDesktopFormat(uint8_t display) const
    {
        int count               = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&count);
        SDL_DisplayID displayId = displays[display];
        SDL_free(displays);
        SDL_DisplayMode const * mode = SDL_GetDesktopDisplayMode(displayId);
        if (mode == nullptr) {
            throw SDLException(SDL_GetError());
        }
        return mode->format;
    }

    int32_t DeviceCaps::getDesktopRefreshRate(uint8_t display) const
    {
        int count               = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&count);
        SDL_DisplayID displayId = displays[display];
        SDL_free(displays);
        SDL_DisplayMode const * mode = SDL_GetDesktopDisplayMode(displayId);
        if (mode == nullptr) {
            throw SDLException(SDL_GetError());
        }
        return mode->refresh_rate;
    }

    int32_t DeviceCaps::getDesktopWidth(uint8_t display) const
    {
        int count               = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&count);
        SDL_DisplayID displayId = displays[display];
        SDL_free(displays);
        SDL_DisplayMode const * mode = SDL_GetDesktopDisplayMode(displayId);
        if (mode == nullptr) {
            throw SDLException(SDL_GetError());
        }
        return mode->w;
    }

    int32_t DeviceCaps::getDesktopHeight(uint8_t display) const
    {
        int count               = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&count);
        SDL_DisplayID displayId = displays[display];
        SDL_free(displays);
        SDL_DisplayMode const * mode = SDL_GetDesktopDisplayMode(displayId);
        if (mode == nullptr) {
            throw SDLException(SDL_GetError());
        }
        return mode->h;
    }

    Rect DeviceCaps::getDisplayBounds(uint8_t display) const
    {
        SDL_Rect srec;
        if (SDL_GetDisplayBounds(display, &srec) != 0) {
            throw SDLException(SDL_GetError());
        }
        Rect rec(srec.x, srec.y, srec.w, srec.h);
        return rec;
    }

} // namespace FIFE
