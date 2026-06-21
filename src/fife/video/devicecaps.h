// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEVICECAPS_H
#define FIFE_DEVICECAPS_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>
// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "util/structures/rect.h"

namespace FIFE
{

    class FIFE_API DeviceCaps
    {
        public:
            /** Constructor.
             */
            DeviceCaps();

            /** Destructor.
             */
            ~DeviceCaps();

            /** Should be called AFTER SDL_Init() has been called
             */
            void fillDeviceCaps();

            /** Clears all information gathered for the device
             */
            void reset();

            /** Gets the available video drivers for your operating system
             */
            std::vector<std::string> const & getAvailableVideoDrivers() const
            {
                return m_availableVideoDrivers;
            };

            /** Gets the available render drivers for your operating system
             */
            std::vector<std::string> const & getAvailableRenderDrivers() const
            {
                return m_availableRenderDrivers;
            };

            /** Returns the name of the current video driver.
             */
            std::string const & getVideoDriverName() const
            {
                return m_videoDriverName;
            }

            /** Sets the name of the video driver.
             */
            void setVideoDriverName(std::string const & driver)
            {
                m_videoDriverName = driver;
            }

            /** Returns the name of the current render driver or
             *  an empty string to initialize the first one supporting the requested flags.
             */
            std::string const & getRenderDriverName() const
            {
                return m_renderDriverName;
            }

            /** Sets the name of the render driver.
             */
            void setRenderDriverName(std::string const & driver);

            /** Returns the number of displays.
             */
            uint8_t getDisplayCount() const;

            /** Returns the display name for the given display index.
             */
            std::string getDisplayName(uint8_t display = 0) const;

            /** Returns the SDL_PixelFormatEnum of the desktop for the given display index.
             */
            uint32_t getDesktopFormat(uint8_t display = 0) const;

            /** Returns the refresh rate in Hz of the desktop for the given display index.
             */
            int32_t getDesktopRefreshRate(uint8_t display = 0) const;

            /** Returns the width of the desktop resolution for the given display index.
             */
            int32_t getDesktopWidth(uint8_t display = 0) const;

            /** Returns the height of the desktop resolution for the given display index.
             */
            int32_t getDesktopHeight(uint8_t display = 0) const;

            /** Returns the bounding points for the given display index.
             */
            Rect getDisplayBounds(uint8_t display = 0) const;

        private:
            std::string m_videoDriverName;
            std::vector<std::string> m_availableVideoDrivers;

            std::string m_renderDriverName;
            int8_t m_renderDriverIndex;
            std::vector<std::string> m_availableRenderDrivers;

            SDL_DisplayID getDisplayId(uint8_t display) const;
            SDL_DisplayMode const * getDesktopMode(uint8_t display) const;

            /** Called in the constructor.  No need for anyone to call this
             */
            void fillAvailableDrivers();
    }; // DeviceCaps
} // namespace FIFE

#endif // FIFE_DEVICECAPS_H
