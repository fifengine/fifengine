// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_WINDOW_WINDOW_H
#define FIFE_VIDEO_WINDOW_WINDOW_H

#include <SDL3/SDL.h>

#include <functional>
#include <string>

#include "platform.h"
#include "video/window/display.h"
#include "video/window/windowsettings.h"

namespace FIFE
{

    /** @brief Centralized window management class.
     *
     * Owns the SDL_Window handle, manages window lifecycle, DPI scaling,
     * fullscreen mode switching, and window event signals.
     *
     * Usage:
     * @code
     * Window win;
     * win.create(settings);
     * win.show();
     * int w = win.getWidthInPoints();
     * float dpi = win.getDPIScaleFactor();
     * @endcode
     */
    class FIFE_API Window
    {
        public:
            /** Default constructor. Does not create the SDL window. */
            Window();
            /** Destructor. Calls destroy(). */
            ~Window();

            Window(Window const &)            = delete;
            Window& operator=(Window const &) = delete;

            /** Create the SDL window with the given settings. Destroys any existing window first. */
            void create(WindowSettings const & settings);
            /** Destroy the SDL window and reset members. */
            void destroy();

            /** @return The ratio of pixels to points (DPI scale factor). 1.0 = no scaling. */
            float getDPIScaleFactor() const;
            /** @return Logical window width in points (unscaled UI coordinates). */
            int getWidthInPoints() const;
            /** @return Logical window height in points (unscaled UI coordinates). */
            int getHeightInPoints() const;
            /** @return Actual pixel width for rendering (points * DPI scale). */
            int getWidthInPixels() const;
            /** @return Actual pixel height for rendering (points * DPI scale). */
            int getHeightInPixels() const;

            /** Set fullscreen mode (Windowed, Fullscreen, Borderless). */
            void setFullscreenMode(WindowMode mode);
            /** @return Current fullscreen mode. */
            WindowMode getFullscreenMode() const;

            /** Set scaling mode (Linear, Integer, Nearest). */
            void setScalingMode(ScalingMode mode);
            /** @return Current scaling mode. */
            ScalingMode getScalingMode() const;

            /** Set window size in points. */
            void setSize(int width, int height);
            /** Set window position in screen coordinates. */
            void setPosition(int x, int y);
            /** Minimize the window. */
            void minimize();
            /** Maximize the window. */
            void maximize();
            /** Restore the window to normal size. */
            void restore();
            /** Show the window (SDL3 creates windows hidden by default). */
            void show();
            /** Hide the window. */
            void hide();
            /** Set the window title. */
            void setWindowTitle(std::string const & title);
            /** Set the window icon (path to image file). */
            void setWindowIcon(std::string const & icon);
            /** Enable or disable VSync. */
            void setVSync(bool enabled);
            /** @return Whether VSync is enabled. */
            bool getVSync() const;

            /** @internal Access the raw SDL_Window pointer (for backend use only). */
            SDL_Window* getSDLWindow();
            /** @return Current window settings. */
            WindowSettings const & getSettings() const;
            /** @return Current display refresh rate in Hz, or 0 if unknown. */
            int getRefreshRate() const;

            /** Recalculate DPI scale if pixel size has changed since last check. Called every frame by Engine::pump().
             */
            void updateDPIScaleIfNeeded();

            /** @return Reference to the Display object for display queries. */
            Display const & getDisplay() const;

            /** Convenience: connect to the resized signal. */
            void onResized(std::function<void(int, int)> callback);
            /** Convenience: connect to the moved signal. */
            void onMoved(std::function<void(int, int)> callback);
            /** Convenience: connect to the DPI scale changed signal. */
            void onDPIScaleChanged(std::function<void(float)> callback);
            /** Convenience: connect to the shown signal. */
            void onShown(std::function<void()> callback);
            /** Convenience: connect to the hidden signal. */
            void onHidden(std::function<void()> callback);
            /** Convenience: connect to the closed signal. */
            void onClosed(std::function<void()> callback);

            std::function<void(int, int)> signalResized;
            std::function<void(int, int)> signalMoved;
            std::function<void(float)> signalDPIScaleChanged;
            std::function<void()> signalShown;
            std::function<void()> signalHidden;
            std::function<void()> signalClosed;

        private:
            SDL_WindowFlags toSDLFlags() const;
            void recalculateDPIScale();

            WindowSettings m_settings;
            SDL_Window* m_window;
            Display m_display;
            float m_dpiScaleFactor;
            bool m_windowShown;
            int m_lastKnownPixelWidth;
    };

} // namespace FIFE

#endif
