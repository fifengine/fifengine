// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "video/window/window.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdlib>
#include <string>
#include <utility>

#include "util/base/exception.h"

namespace FIFE
{

    namespace
    {
        float queryDisplayScale(SDL_Window* window)
        {
            if (window == nullptr) {
                return 1.0F;
            }
            float const scale = SDL_GetWindowDisplayScale(window);
            return scale > 0.0F ? scale : 1.0F;
        }
    } // namespace

    Window::Window() :
        m_window(nullptr), m_dpiScaleFactor(1.0F), m_displayScale(1.0F), m_windowShown(false), m_lastKnownPixelWidth(0)
    {
    }

    Window::~Window()
    {
        destroy();
    }

    void Window::create(WindowSettings const & settings)
    {
        destroy();

        m_settings = settings;

        // WindowSettings::display is a 0-based index; SDL's display APIs use
        // SDL_DisplayID. Clamp invalid values before they reach SDL.
        int const displayCount = Display::getDisplayCount();
        if (displayCount > 0) {
            m_settings.display = std::clamp(m_settings.display, 0, displayCount - 1);
        } else {
            m_settings.display = 0;
        }
        m_settings.width  = std::max(1, m_settings.width);
        m_settings.height = std::max(1, m_settings.height);

        SDL_WindowFlags flags = toSDLFlags();

        m_window = SDL_CreateWindow(m_settings.title.c_str(), m_settings.width, m_settings.height, flags);

        if (m_window == nullptr) {
            throw SDLException(SDL_GetError());
        }

        SDL_SetWindowMinimumSize(m_window, 640, 400);

        m_windowedX             = -1;
        m_windowedY             = -1;
        m_windowedPositionValid = false;

        // Put the window on the configured display *before* entering
        // fullscreen: desktop fullscreen uses the display the window is on,
        // and SDL_WINDOWPOS_CENTERED_DISPLAY takes an SDL_DisplayID, not an
        // index. -1 means "center on the selected display"; other values,
        // including negative ones (a monitor left of/above the primary), are
        // explicit.
        SDL_DisplayID const displayID = Display::getDisplayID(m_settings.display);
        int posX                      = m_settings.x;
        int posY                      = m_settings.y;
        if (displayID != 0) {
            if (posX == -1) {
                posX = SDL_WINDOWPOS_CENTERED_DISPLAY(displayID);
            }
            if (posY == -1) {
                posY = SDL_WINDOWPOS_CENTERED_DISPLAY(displayID);
            }
        }
        if (posX != -1 && posY != -1) {
            SDL_SetWindowPosition(m_window, posX, posY);
        }

        if (m_settings.windowMode == WindowMode::Fullscreen) {
            // Desktop fullscreen: never switch the display mode. Enter it
            // after positioning so the window stays on the configured display.
            SDL_SetWindowFullscreenMode(m_window, nullptr);
            int windowedX = 0;
            int windowedY = 0;
            if (SDL_GetWindowPosition(m_window, &windowedX, &windowedY)) {
                m_windowedX             = windowedX;
                m_windowedY             = windowedY;
                m_windowedPositionValid = true;
                m_settings.x            = windowedX;
                m_settings.y            = windowedY;
            }
            SDL_SetWindowBordered(m_window, false);
            if (!SDL_SetWindowFullscreen(m_window, true)) {
                // Keep the window usable instead of claiming fullscreen.
                m_settings.windowMode = WindowMode::Windowed;
                SDL_SetWindowBordered(m_window, true);
            }
        }

        // Fullscreen/size/position changes can be asynchronous. Block until
        // the new state is applied so the size we read below (and publish
        // through EngineSettings) is the real one.
        SDL_SyncWindow(m_window);

        // SDL may adjust the requested size (minimum size, window manager
        // constraints, fullscreen). Keep WindowSettings in sync with the
        // window that actually exists so callers do not keep using the
        // requested values. While fullscreen, keep the requested (windowed)
        // size so leaving fullscreen can restore it.
        if (m_settings.windowMode != WindowMode::Fullscreen) {
            int actualWidth  = 0;
            int actualHeight = 0;
            SDL_GetWindowSize(m_window, &actualWidth, &actualHeight);
            if (actualWidth > 0 && actualHeight > 0) {
                m_settings.width  = actualWidth;
                m_settings.height = actualHeight;
            }
        }

        recalculateDPIScale();
    }

    void Window::destroy()
    {
        if (m_window != nullptr) {
            // Engine::destroy() calls SDL_Quit() while member objects still
            // own their SDL handles; SDL_VideoQuit has already freed the
            // window at that point. Do not touch it after the subsystem died.
            if (SDL_WasInit(SDL_INIT_VIDEO)) {
                // Renderers and GL contexts die with the window; give
                // listeners a chance to release textures while they are valid.
                if (signalWindowDestroyed) {
                    signalWindowDestroyed();
                }
                SDL_DestroyWindow(m_window);
            }
            m_window = nullptr;
        }
        m_dpiScaleFactor        = 1.0F;
        m_displayScale          = 1.0F;
        m_windowShown           = false;
        m_lastKnownPixelWidth   = 0;
        m_windowedPositionValid = false;
    }

    float Window::getDPIScaleFactor() const
    {
        return m_dpiScaleFactor;
    }

    float Window::getDisplayScale() const
    {
        return m_displayScale;
    }

    int Window::getWidthInPoints() const
    {
        if (m_window == nullptr) {
            return 0;
        }
        // In fullscreen mode, the window size is the display bounds.
        // This is necessary for headless environments where
        // SDL_GetWindowSize may not reflect the actual fullscreen size.
        if (m_settings.windowMode == WindowMode::Fullscreen) {
            auto const bounds = Display::getDisplayBounds(m_settings.display);
            return bounds.w;
        }
        int w = 0;
        SDL_GetWindowSize(m_window, &w, nullptr);
        return w;
    }

    int Window::getHeightInPoints() const
    {
        if (m_window == nullptr) {
            return 0;
        }
        // In fullscreen mode, the window size is the display bounds.
        // This is necessary for headless environments where
        // SDL_GetWindowSize may not reflect the actual fullscreen size.
        if (m_settings.windowMode == WindowMode::Fullscreen) {
            auto const bounds = Display::getDisplayBounds(m_settings.display);
            return bounds.h;
        }
        int h = 0;
        SDL_GetWindowSize(m_window, nullptr, &h);
        return h;
    }

    int Window::getWidthInPixels() const
    {
        if (m_window == nullptr) {
            return 0;
        }
        // In fullscreen mode, the pixel size is the display bounds.
        // This is necessary for headless environments where
        // SDL_GetWindowSizeInPixels may not reflect the actual fullscreen size.
        if (m_settings.windowMode == WindowMode::Fullscreen) {
            auto const bounds = Display::getDisplayBounds(m_settings.display);
            return bounds.w;
        }
        int w = 0;
        SDL_GetWindowSizeInPixels(m_window, &w, nullptr);
        return w;
    }

    int Window::getHeightInPixels() const
    {
        if (m_window == nullptr) {
            return 0;
        }
        // In fullscreen mode, the pixel size is the display bounds.
        // This is necessary for headless environments where
        // SDL_GetWindowSizeInPixels may not reflect the actual fullscreen size.
        if (m_settings.windowMode == WindowMode::Fullscreen) {
            auto const bounds = Display::getDisplayBounds(m_settings.display);
            return bounds.h;
        }
        int h = 0;
        SDL_GetWindowSizeInPixels(m_window, nullptr, &h);
        return h;
    }

    void Window::setFullscreenMode(WindowMode mode)
    {
        if (m_window == nullptr) {
            return;
        }

        if (m_settings.windowMode == mode) {
            return;
        }

        WindowMode const previousMode = m_settings.windowMode;
        bool const wasFullscreen      = (previousMode == WindowMode::Fullscreen);

        if (!wasFullscreen) {
            // Remember the windowed geometry (including after a user resize)
            // so leaving fullscreen restores it. Positions can be negative on
            // monitors left of/above the primary, so rely on the call result
            // instead of a coordinate-point sentinel.
            m_windowedPositionValid = false;
            int x                   = 0;
            int y                   = 0;
            if (SDL_GetWindowPosition(m_window, &x, &y)) {
                m_windowedX             = x;
                m_windowedY             = y;
                m_windowedPositionValid = true;
                m_settings.x            = x;
                m_settings.y            = y;
            }
            int w = 0;
            int h = 0;
            SDL_GetWindowSize(m_window, &w, &h);
            if (w > 0 && h > 0) {
                m_settings.width  = w;
                m_settings.height = h;
            }
        }

        m_settings.windowMode = mode;

        if (mode == WindowMode::Fullscreen) {
            // Desktop fullscreen: a null fullscreen mode tells SDL3 not to
            // switch the display mode (no resolution change, no flicker).
            SDL_SetWindowFullscreenMode(m_window, nullptr);
            SDL_SetWindowBordered(m_window, false);
            if (!SDL_SetWindowFullscreen(m_window, true)) {
                // Revert to the mode that was actually active before the call.
                m_settings.windowMode = previousMode;
                SDL_SetWindowBordered(m_window, previousMode != WindowMode::Borderless);
            }
        } else {
            // Windowed and Borderless are both windowed; Borderless only
            // removes the decorations.
            if (wasFullscreen) {
                if (!SDL_SetWindowFullscreen(m_window, false)) {
                    // Still fullscreen; keep the state consistent with the
                    // window that actually exists.
                    m_settings.windowMode = previousMode;
                    SDL_SyncWindow(m_window);
                    recalculateDPIScale();
                    return;
                }
                SDL_SetWindowSize(m_window, m_settings.width, m_settings.height);
                if (m_windowedPositionValid) {
                    SDL_SetWindowPosition(m_window, m_windowedX, m_windowedY);
                    m_settings.x = m_windowedX;
                    m_settings.y = m_windowedY;
                }
            }
            SDL_SetWindowBordered(m_window, mode == WindowMode::Windowed);
        }

        SDL_SyncWindow(m_window);
        recalculateDPIScale();
    }

    WindowMode Window::getFullscreenMode() const
    {
        return m_settings.windowMode;
    }

    void Window::setScalingMode(ScalingMode mode)
    {
        m_settings.scalingMode = mode;
    }

    ScalingMode Window::getScalingMode() const
    {
        return m_settings.scalingMode;
    }

    void Window::setSize(int width, int height)
    {
        if (m_window != nullptr) {
            SDL_SetWindowSize(m_window, width, height);
            m_settings.width  = width;
            m_settings.height = height;
            recalculateDPIScale();
        }
    }

    void Window::setPosition(int x, int y)
    {
        if (m_window != nullptr) {
            SDL_SetWindowPosition(m_window, x, y);
            m_settings.x = x;
            m_settings.y = y;
        }
    }

    void Window::setResizable(bool resizable)
    {
        m_settings.resizable = resizable;
        if (m_window != nullptr) {
            SDL_SetWindowResizable(m_window, resizable);
        }
    }

    bool Window::isResizable() const
    {
        return m_settings.resizable;
    }

    void Window::minimize()
    {
        if (m_window != nullptr) {
            SDL_MinimizeWindow(m_window);
        }
    }

    void Window::maximize()
    {
        if (m_window != nullptr) {
            SDL_MaximizeWindow(m_window);
        }
    }

    void Window::restore()
    {
        if (m_window != nullptr) {
            SDL_RestoreWindow(m_window);
        }
    }

    void Window::show()
    {
        if (m_window != nullptr && !m_windowShown) {
            SDL_ShowWindow(m_window);
            m_windowShown = true;
            // A fullscreen window created while hidden applies its size when
            // it becomes visible; make that state observable immediately and
            // notify listeners of the initial size/DPI.
            SDL_SyncWindow(m_window);
            recalculateDPIScale();
        }
    }

    void Window::hide()
    {
        if (m_window != nullptr) {
            SDL_HideWindow(m_window);
            m_windowShown = false;
        }
    }

    void Window::setWindowTitle(std::string const & title)
    {
        m_settings.title = title;
        if (m_window != nullptr) {
            SDL_SetWindowTitle(m_window, title.c_str());
        }
    }

    void Window::setWindowIcon(std::string const & icon)
    {
        m_settings.icon = icon;
    }

    void Window::setVSync(bool enabled)
    {
        m_settings.vsync = enabled;
        if (m_window == nullptr) {
            return;
        }

        if (m_settings.opengl) {
            SDL_GL_SetSwapInterval(static_cast<uint8_t>(enabled));
        } else {
            SDL_Window* win        = m_window;
            SDL_Renderer* renderer = SDL_GetRenderer(win);
            if (renderer != nullptr) {
                SDL_SetRenderVSync(renderer, static_cast<uint8_t>(enabled));
            }
        }
    }

    bool Window::getVSync() const
    {
        return m_settings.vsync;
    }

    Display const & Window::getDisplay() const
    {
        return m_display;
    }

    void Window::onResized(std::function<void(int, int)> callback)
    {
        signalResized = std::move(callback);
    }
    void Window::onMoved(std::function<void(int, int)> callback)
    {
        signalMoved = std::move(callback);
    }
    void Window::onDPIScaleChanged(std::function<void(float)> callback)
    {
        signalDPIScaleChanged = std::move(callback);
    }
    void Window::onDisplayScaleChanged(std::function<void(float)> callback)
    {
        signalDisplayScaleChanged = std::move(callback);
    }
    void Window::onShown(std::function<void()> callback)
    {
        signalShown = std::move(callback);
    }
    void Window::onHidden(std::function<void()> callback)
    {
        signalHidden = std::move(callback);
    }
    void Window::onClosed(std::function<void()> callback)
    {
        signalClosed = std::move(callback);
    }

    SDL_Window* Window::getSDLWindow()
    {
        return m_window;
    }

    WindowSettings const & Window::getSettings() const
    {
        return m_settings;
    }

    int Window::getRefreshRate() const
    {
        if (m_window == nullptr) {
            return 0;
        }
        SDL_DisplayID displayID      = SDL_GetDisplayForWindow(m_window);
        SDL_DisplayMode const * mode = SDL_GetCurrentDisplayMode(displayID);
        if (mode != nullptr) {
            return static_cast<int>(mode->refresh_rate);
        }
        return 0;
    }

    void Window::updateDPIScaleIfNeeded()
    {
        if (m_window == nullptr) {
            return;
        }
        int currentPixelW = 0;

        // In fullscreen mode, use display bounds to get the expected size
        // to handle headless environments where SDL_GetWindowSizeInPixels
        // may not reflect the actual fullscreen size.
        if (m_settings.windowMode == WindowMode::Fullscreen) {
            auto const bounds = Display::getDisplayBounds(m_settings.display);
            currentPixelW = bounds.w;
        } else {
            SDL_GetWindowSizeInPixels(m_window, &currentPixelW, nullptr);
        }

        if (currentPixelW != m_lastKnownPixelWidth) {
            recalculateDPIScale();
            m_lastKnownPixelWidth = currentPixelW;
        } else {
            // The pixel/point ratio can stay the same while the OS content
            // scale changes (for example moving a window between two Windows
            // monitors at 100% and 150%). Poll it separately.
            updateDisplayScale();
        }
    }

    bool Window::updateDisplayScale()
    {
        float const newScale = queryDisplayScale(m_window);
        if (newScale == m_displayScale) {
            return false;
        }
        m_displayScale = newScale;
        if (signalDisplayScaleChanged) {
            signalDisplayScaleChanged(m_displayScale);
        }
        return true;
    }

    SDL_WindowFlags Window::toSDLFlags() const
    {
        SDL_WindowFlags flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;

        if (m_settings.opengl) {
            flags |= SDL_WINDOW_OPENGL;
        }

        if (m_settings.resizable) {
            flags |= SDL_WINDOW_RESIZABLE;
        }

        switch (m_settings.windowMode) {
        case WindowMode::Borderless:
            flags |= SDL_WINDOW_BORDERLESS;
            break;
        case WindowMode::Windowed:
        case WindowMode::Fullscreen:
        default:
            // Fullscreen is entered after creation so the window can be
            // positioned on the configured display first (see create()).
            break;
        }

        return flags;
    }

    void Window::recalculateDPIScale()
    {
        // Refresh display scale and pixel/point ratio before firing either
        // signal. Engine::applyGUIScale() divides one by the other, so a
        // listener that runs between the two updates would briefly compute the
        // GUI scale from mismatched values.
        float const newDisplayScale    = queryDisplayScale(m_window);
        bool const displayScaleChanged = newDisplayScale != m_displayScale;
        m_displayScale                 = newDisplayScale;

        if (m_window == nullptr) {
            m_dpiScaleFactor = 1.0F;
            return;
        }

        int pointsW = 0;
        int pointsH = 0;
        int pixelsW = 0;
        int pixelsH = 0;

        // In fullscreen mode, use display bounds for sizes
        // to handle headless environments where SDL_GetWindowSize may
        // not reflect the actual fullscreen size.
        if (m_settings.windowMode == WindowMode::Fullscreen) {
            auto const bounds = Display::getDisplayBounds(m_settings.display);
            pointsW = bounds.w;
            pointsH = bounds.h;
            pixelsW = bounds.w; // On X11/Wayland, bounds are in pixels with scale 1.0
            pixelsH = bounds.h;
        } else {
            SDL_GetWindowSize(m_window, &pointsW, &pointsH);
            SDL_GetWindowSizeInPixels(m_window, &pixelsW, &pixelsH);
        }

        bool dpiScaleChanged = false;
        if (pointsW > 0) {
            float const newScale = static_cast<float>(pixelsW) / static_cast<float>(pointsW);
            if (newScale != m_dpiScaleFactor) {
                m_dpiScaleFactor = newScale;
                dpiScaleChanged  = true;
            }
        }

        if (displayScaleChanged && signalDisplayScaleChanged) {
            signalDisplayScaleChanged(m_displayScale);
        }
        if (dpiScaleChanged && signalDPIScaleChanged) {
            signalDPIScaleChanged(m_dpiScaleFactor);
        }

        if (signalResized) {
            signalResized(pointsW, pointsH);
        }
    }

} // namespace FIFE
