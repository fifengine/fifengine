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

    Window::Window() : m_window(nullptr), m_dpiScaleFactor(1.0F), m_windowShown(false), m_lastKnownPixelWidth(0)
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

        SDL_WindowFlags flags = toSDLFlags();

        m_window = SDL_CreateWindow(m_settings.title.c_str(), m_settings.width, m_settings.height, flags);

        if (m_window == nullptr) {
            throw SDLException(SDL_GetError());
        }

        SDL_SetWindowMinimumSize(m_window, 640, 400);

        if (m_settings.x >= 0 && m_settings.y >= 0) {
            SDL_SetWindowPosition(m_window, m_settings.x, m_settings.y);
        }

        recalculateDPIScale();
    }

    void Window::destroy()
    {
        if (m_window != nullptr) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
        m_dpiScaleFactor      = 1.0F;
        m_windowShown         = false;
        m_lastKnownPixelWidth = 0;
    }

    float Window::getDPIScaleFactor() const
    {
        return m_dpiScaleFactor;
    }

    int Window::getWidthInPoints() const
    {
        int w = 0;
        if (m_window != nullptr) {
            SDL_GetWindowSize(m_window, &w, nullptr);
        }
        return w;
    }

    int Window::getHeightInPoints() const
    {
        int h = 0;
        if (m_window != nullptr) {
            SDL_GetWindowSize(m_window, nullptr, &h);
        }
        return h;
    }

    int Window::getWidthInPixels() const
    {
        int w = 0;
        if (m_window != nullptr) {
            SDL_GetWindowSizeInPixels(m_window, &w, nullptr);
        }
        return w;
    }

    int Window::getHeightInPixels() const
    {
        int h = 0;
        if (m_window != nullptr) {
            SDL_GetWindowSizeInPixels(m_window, nullptr, &h);
        }
        return h;
    }

    void Window::setFullscreenMode(WindowMode mode)
    {
        if (m_window == nullptr) {
            return;
        }

        m_settings.windowMode = mode;

        switch (mode) {
        case WindowMode::Fullscreen:
            SDL_SetWindowFullscreen(m_window, true);
            break;
        case WindowMode::Borderless:
            SDL_SetWindowBordered(m_window, false);
            SDL_SetWindowFullscreen(m_window, true);
            break;
        case WindowMode::Windowed:
        default:
            SDL_SetWindowFullscreen(m_window, false);
            SDL_SetWindowBordered(m_window, true);
            SDL_SetWindowSize(m_window, m_settings.width, m_settings.height);
            if (m_settings.x >= 0 && m_settings.y >= 0) {
                SDL_SetWindowPosition(m_window, m_settings.x, m_settings.y);
            }
            break;
        }

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
            recalculateDPIScale();
        }
    }

    void Window::setPosition(int x, int y)
    {
        if (m_window != nullptr) {
            SDL_SetWindowPosition(m_window, x, y);
        }
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
        int currentPixelW;
        SDL_GetWindowSizeInPixels(m_window, &currentPixelW, nullptr);
        if (currentPixelW != m_lastKnownPixelWidth) {
            recalculateDPIScale();
            m_lastKnownPixelWidth = currentPixelW;
        }
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
        case WindowMode::Fullscreen:
            flags |= SDL_WINDOW_FULLSCREEN;
            break;
        case WindowMode::Borderless:
            flags |= SDL_WINDOW_FULLSCREEN;
            break;
        case WindowMode::Windowed:
        default:
            break;
        }

        return flags;
    }

    void Window::recalculateDPIScale()
    {
        if (m_window == nullptr) {
            m_dpiScaleFactor = 1.0F;
            return;
        }

        int pointsW = 0;
        int pixelsW = 0;
        SDL_GetWindowSize(m_window, &pointsW, nullptr);
        SDL_GetWindowSizeInPixels(m_window, &pixelsW, nullptr);

        if (pointsW > 0) {
            float newScale = static_cast<float>(pixelsW) / static_cast<float>(pointsW);
            if (newScale != m_dpiScaleFactor) {
                m_dpiScaleFactor = newScale;
                if (signalDPIScaleChanged) {
                    signalDPIScaleChanged(m_dpiScaleFactor);
                }
            }
        }

        if (signalResized) {
            int h = 0;
            SDL_GetWindowSize(m_window, nullptr, &h);
            signalResized(pointsW, h);
        }
    }

} // namespace FIFE
