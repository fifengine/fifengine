// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEVICECAPS_H
#define FIFE_DEVICECAPS_H

// Standard C++ library includes
#include <string>
#include <vector>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/rect.h"

namespace FIFE
{

    class ScreenMode
    {
    public:
        /** Default Constructor
         * @note You shouldn't construct these objects yourself.  This default
         * constructor was provided because swig complained that was none.
         */
        ScreenMode();
        ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t SDLFlags);
        ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint16_t rate, uint32_t SDLFlags);
        ScreenMode(const ScreenMode& rhs);

        ScreenMode& operator=(const ScreenMode&) = default;

        /** Destructor.
         */
        ~ScreenMode() = default;

        static const uint32_t WINDOWED_OPENGL   = 0x00000002;
        static const uint32_t FULLSCREEN_OPENGL = 0x00000003;
        static const uint32_t WINDOWED_SDL      = 0;
        static const uint32_t FULLSCREEN_SDL    = 0x00000001;

        bool operator<(const ScreenMode& rhs) const;

        /** Returns the width of the screen mode.
         * @note If both width and height are 0 it means that ALL modes are available
         * for use with the specified flags.  Most likely this is a windowed mode.
         */
        uint16_t getWidth() const
        {
            return m_width;
        }

        /** Returns the height of the screen mode.
         * @note If both width and height are 0 it means that ALL modes are available
         * for use with the specified flags.  Most likely this is a windowed mode.
         */
        uint16_t getHeight() const
        {
            return m_height;
        }

        /** Returns the number of bits per pixel this mode uses.
         */
        uint16_t getBPP() const
        {
            return m_bpp;
        }

        /** Returns the refresh rate in Hz of this mode.
         */
        uint16_t getRefreshRate() const
        {
            return m_refreshRate;
        }

        /** Returns the SDL flags used when testing this mode.
         */
        uint32_t getSDLFlags() const
        {
            return m_SDLFlags;
        }

        /** True if this is a fullscreen mode.  False if it is a windowed mode.
         */
        bool isFullScreen() const
        {
            return (m_SDLFlags & SDL_WINDOW_FULLSCREEN) != 0;
        }

        /** True if this mode uses the OpenGL renderer.  False otherwise.
         */
        bool isOpenGL() const
        {
            return (m_SDLFlags & SDL_WINDOW_OPENGL) != 0;
        }

        /** Is this screen mode an SDL only screen mode
         */
        bool isSDL() const
        {
            return (m_SDLFlags & SDL_WINDOW_OPENGL) == 0;
        }

        /** Sets the pixel format enum.
         */
        void setFormat(uint32_t format)
        {
            m_format = format;
        }

        /** Returns the pixel format enum.
         */
        uint32_t getFormat() const
        {
            return m_format;
        }

        /** Sets the display index.
         */
        void setDisplay(uint8_t display)
        {
            m_display = display;
        }

        /** Returns the display index.
         */
        uint8_t getDisplay() const
        {
            return m_display;
        }

        /** Sets window X position (-1 = use default/centered)
         */
        void setWindowPositionX(int32_t x)
        {
            m_windowX = x;
        }

        /** Returns the window X position (-1 = use default/centered)
         */
        int32_t getWindowPositionX() const
        {
            return m_windowX;
        }

        /** Sets window Y position (-1 = use default/centered)
         */
        void setWindowPositionY(int32_t y)
        {
            m_windowY = y;
        }

        /** Returns the window Y position (-1 = use default/centered)
         */
        int32_t getWindowPositionY() const
        {
            return m_windowY;
        }

        /** Sets the render driver index.
         */
        void setRenderDriverIndex(int8_t index)
        {
            m_renderDriverIndex = index;
        }

        /** Returns the render driver index.
         */
        int8_t getRenderDriverIndex() const
        {
            return m_renderDriverIndex;
        }

        /** Sets the render driver name.
         */
        void setRenderDriverName(const std::string& driver)
        {
            m_renderDriver = driver;
        }

        /** Returns the render driver name.
         */
        const std::string& getRenderDriverName() const
        {
            return m_renderDriver;
        }

    private:
        uint16_t m_width;
        uint16_t m_height;
        uint16_t m_bpp;
        uint16_t m_refreshRate;
        uint32_t m_SDLFlags;
        uint32_t m_format;
        uint8_t m_display;
        int32_t m_windowX;
        int32_t m_windowY;
        std::string m_renderDriver;
        int8_t m_renderDriverIndex;
    }; // ScreenMode

    class DeviceCaps
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
        const std::vector<std::string>& getAvailableVideoDrivers() const
        {
            return m_availableVideoDrivers;
        };

        /** Gets the available render drivers for your operating system
         */
        const std::vector<std::string>& getAvailableRenderDrivers() const
        {
            return m_availableRenderDrivers;
        };

        /** Returns a vector containing screen modes.
         */
        const std::vector<ScreenMode>& getSupportedScreenModes() const
        {
            return m_screenModes;
        };

        /** Gets the nearest valid screen mode based on the arguments passed
         */
        ScreenMode getNearestScreenMode(
            uint16_t width, uint16_t height, uint16_t bpp, const std::string& renderer, bool fs) const;

        /** Gets the nearest valid screen mode based on the arguments passed
         */
        ScreenMode getNearestScreenMode(
            uint16_t width,
            uint16_t height,
            uint16_t bpp,
            const std::string& renderer,
            bool fs,
            uint16_t refresh,
            uint8_t display = 0) const;

        /** Returns the name of the current video driver.
         */
        const std::string& getVideoDriverName() const
        {
            return m_videoDriverName;
        }

        /** Sets the name of the video driver.
         */
        void setVideoDriverName(const std::string& driver)
        {
            m_videoDriverName = driver;
        }

        /** Returns the name of the current render driver or
         *  an empty string to initialize the first one supporting the requested flags.
         */
        const std::string& getRenderDriverName() const
        {
            return m_renderDriverName;
        }

        /** Sets the name of the render driver.
         */
        void setRenderDriverName(const std::string& driver);

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
        std::vector<ScreenMode> m_screenModes;
        std::string m_videoDriverName;
        std::vector<std::string> m_availableVideoDrivers;

        std::string m_renderDriverName;
        int8_t m_renderDriverIndex;
        std::vector<std::string> m_availableRenderDrivers;

        /** Called in the constructor.  No need for anyone to call this
         */
        void fillAvailableDrivers();
    }; // DeviceCaps
} // namespace FIFE

#endif // FIFE_DEVICECAPS_H
