// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "video/window/display.h"

#include <string>
#include <vector>

namespace FIFE
{

    namespace
    {
        SDL_DisplayID getDisplayId(int index)
        {
            int count               = 0;
            SDL_DisplayID* displays = SDL_GetDisplays(&count);
            if (displays == nullptr || count <= 0 || index >= count) {
                SDL_free(displays);
                return 0;
            }
            SDL_DisplayID id = displays[index];
            SDL_free(displays);
            return id;
        }
    } // namespace

    int Display::getDisplayCount()
    {
        int count               = 0;
        SDL_DisplayID* displays = SDL_GetDisplays(&count);
        SDL_free(displays);
        return count > 0 ? count : 0;
    }

    std::string Display::getDisplayName(int index)
    {
        SDL_DisplayID id = getDisplayId(index);
        if (id == 0) {
            return {};
        }
        char const * name = SDL_GetDisplayName(id);
        return name != nullptr ? std::string(name) : std::string();
    }

    SDL_Rect Display::getDisplayBounds(int index)
    {
        SDL_Rect rect    = {};
        SDL_DisplayID id = getDisplayId(index);
        if (id != 0) {
            SDL_GetDisplayBounds(id, &rect);
        }
        return rect;
    }

    SDL_Rect Display::getUsableBounds(int index)
    {
        SDL_Rect rect    = {};
        SDL_DisplayID id = getDisplayId(index);
        if (id != 0) {
            SDL_GetDisplayUsableBounds(id, &rect);
        }
        return rect;
    }

    int Display::getDisplayWidth(int index)
    {
        return getDisplayBounds(index).w;
    }

    int Display::getDisplayHeight(int index)
    {
        return getDisplayBounds(index).h;
    }

    float Display::getDisplayContentScale(int index)
    {
        SDL_DisplayID id = getDisplayId(index);
        if (id == 0) {
            return 1.0F;
        }
        return SDL_GetDisplayContentScale(id);
    }

    SDL_DisplayMode Display::getDesktopMode(int index)
    {
        SDL_DisplayMode mode = {};
        SDL_DisplayID id     = getDisplayId(index);
        if (id != 0) {
            SDL_DisplayMode const * dm = SDL_GetDesktopDisplayMode(id);
            if (dm != nullptr) {
                mode = *dm;
            }
        }
        return mode;
    }

    std::vector<SDL_DisplayMode> Display::getSupportedModes(int index)
    {
        std::vector<SDL_DisplayMode> modes;
        SDL_DisplayID id = getDisplayId(index);
        if (id == 0) {
            return modes;
        }

        int count                = 0;
        SDL_DisplayMode** dmList = SDL_GetFullscreenDisplayModes(id, &count);
        if (dmList != nullptr && count > 0) {
            modes.reserve(static_cast<size_t>(count));
            for (int i = 0; i < count; ++i) {
                if (dmList[i] != nullptr) {
                    modes.push_back(*dmList[i]);
                }
            }
            SDL_free(static_cast<void*>(dmList));
        }
        return modes;
    }

    SDL_DisplayMode Display::getClosestMode(int index, int width, int height, int refreshRate)
    {
        SDL_DisplayMode mode = {};
        SDL_DisplayID id     = getDisplayId(index);
        if (id == 0) {
            return mode;
        }

        SDL_GetClosestFullscreenDisplayMode(id, width, height, static_cast<float>(refreshRate), false, &mode);
        return mode;
    }

} // namespace FIFE
