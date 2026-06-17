// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "renderbackendsdl.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <format>
#include <limits>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include <SDL3_image/SDL_image.h>

#include "sdlimage.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "video/devicecaps.h"
#include "video/window/window.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_VIDEO);
            return log;
        }
    } // namespace

    namespace
    {
        int32_t toInt32Dimension(uint32_t value)
        {
            assert(value <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
            return static_cast<int32_t>(value);
        }

        int32_t toDisplayWindowPos(uint8_t displayIndex, bool centered)
        {
            return centered ? static_cast<int32_t>(SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex)) :
                              static_cast<int32_t>(SDL_WINDOWPOS_UNDEFINED_DISPLAY(displayIndex));
        }
    } // namespace

    RenderBackendSDL::RenderBackendSDL(SDL_Color const & colorkey) : RenderBackend(colorkey), m_renderer(nullptr)
    {
    }

    RenderBackendSDL::~RenderBackendSDL()
    {
        SDL_DestroyRenderer(m_renderer);
        if (m_windowObject == nullptr) {
            SDL_DestroyWindow(m_window);
        }
        deinit();
    }

    std::string const & RenderBackendSDL::getName() const
    {
        static std::string const backend_name = "SDL";
        return backend_name;
    }

    void RenderBackendSDL::init(std::string const & driver)
    {
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
            throw SDLException(SDL_GetError());
        }
        if (!driver.empty()) {
            SDL_SetHint(SDL_HINT_VIDEO_DRIVER, driver.c_str());
        }
    }

    void RenderBackendSDL::clearBackBuffer()
    {
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(m_renderer);
    }

    void RenderBackendSDL::createMainScreen(std::string const & title, std::string const & icon)
    {
        if (m_window != nullptr) {
            if (!icon.empty()) {
                SDL_Surface* img = IMG_Load(icon.c_str());
                if (img != nullptr) {
                    SDL_SetWindowIcon(m_window, img);
                    SDL_DestroySurface(img);
                }
            }
            SDL_SetWindowTitle(m_window, title.c_str());
        }

        // If we don't have a renderer yet, create a minimal one
        if (m_renderer == nullptr) {
            SDL_Window* window = (m_windowObject != nullptr) ? m_windowObject->getSDLWindow() : m_window;
            if (window != nullptr) {
                m_renderer = SDL_CreateRenderer(window, nullptr);
                if (m_renderer == nullptr) {
                    throw SDLException(SDL_GetError());
                }
                if (!m_vSync) {
                    SDL_SetRenderVSync(m_renderer, 0);
                }
                SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
                clearBackBuffer();
            }
        }

        // Create a dummy surface if we don't have one yet
        if (m_screen == nullptr && m_window != nullptr) {
            int w = 0;
            int h = 0;
            SDL_GetWindowSize(m_window, &w, &h);
            uint16_t const surfaceW = static_cast<uint16_t>(std::max(1, w));
            uint16_t const surfaceH = static_cast<uint16_t>(std::max(1, h));
            m_screen          = SDL_CreateSurface(surfaceW, surfaceH, SDL_PIXELFORMAT_RGBA8888);
            m_target          = m_screen;
            if (m_screen == nullptr) {
                throw SDLException(SDL_GetError());
            }
            m_rgba_format = *SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888);
        }
    }

    void RenderBackendSDL::endFrame()
    {
        SDL_RenderPresent(m_renderer);
        RenderBackend::endFrame();
    }

    std::unique_ptr<Image> RenderBackendSDL::createImage(IResourceLoader* loader)
    {
        return std::make_unique<SDLImage>(loader);
    }

    std::unique_ptr<Image> RenderBackendSDL::createImage(std::string const & name, IResourceLoader* loader)
    {
        return std::make_unique<SDLImage>(name, loader);
    }

    std::unique_ptr<Image> RenderBackendSDL::createImage(SDL_Surface* surface)
    {
        if (surface->format == m_rgba_format.format) {
            return std::make_unique<SDLImage>(surface);
        }

        SDL_Surface* conv = SDL_ConvertSurface(surface, m_rgba_format.format);
        auto image        = std::make_unique<SDLImage>(conv);

        SDL_DestroySurface(surface);
        return image;
    }

    std::unique_ptr<Image> RenderBackendSDL::createImage(std::string const & name, SDL_Surface* surface)
    {
        if (surface->format == m_rgba_format.format) {
            return std::make_unique<SDLImage>(name, surface);
        }

        SDL_Surface* conv = SDL_ConvertSurface(surface, m_rgba_format.format);
        auto image        = std::make_unique<SDLImage>(name, conv);

        SDL_DestroySurface(surface);
        return image;
    }

    std::unique_ptr<Image> RenderBackendSDL::createImage(uint8_t const * data, uint32_t width, uint32_t height)
    {
        return std::make_unique<SDLImage>(data, width, height);
    }

    std::unique_ptr<Image> RenderBackendSDL::createImage(
        std::string const & name, uint8_t const * data, uint32_t width, uint32_t height)
    {
        return std::make_unique<SDLImage>(name, data, width, height);
    }

    void RenderBackendSDL::setLightingModel(uint32_t lighting)
    {
        static_cast<void>(lighting);
        throw SDLException("Lighting not available under SDL");
    }

    uint32_t RenderBackendSDL::getLightingModel() const
    {
        return 0;
    }

    void RenderBackendSDL::setLighting(float red, float green, float blue)
    {
        static_cast<void>(red);
        static_cast<void>(green);
        static_cast<void>(blue);
    }

    void RenderBackendSDL::resetLighting()
    {
    }

    void RenderBackendSDL::resetStencilBuffer(uint8_t buffer)
    {
        static_cast<void>(buffer);
    }

    void RenderBackendSDL::changeBlending(int32_t scr, int32_t dst)
    {
        static_cast<void>(scr);
        static_cast<void>(dst);
    }

    void RenderBackendSDL::renderVertexArrays()
    {
    }

    void RenderBackendSDL::addImageToArray(
        uint32_t id, Rect const & rec, float const * st, uint8_t alpha, uint8_t const * rgba)
    {
        static_cast<void>(id);
        static_cast<void>(rec);
        static_cast<void>(st);
        static_cast<void>(alpha);
        static_cast<void>(rgba);
    }

    void RenderBackendSDL::changeRenderInfos(
        RenderDataType type,
        uint16_t elements,
        int32_t src,
        int32_t dst,
        bool light,
        bool stentest,
        uint8_t stenref,
        GLConstants stenop,
        GLConstants stenfunc,
        OverlayType otype)
    {
        static_cast<void>(type);
        static_cast<void>(elements);
        static_cast<void>(src);
        static_cast<void>(dst);
        static_cast<void>(light);
        static_cast<void>(stentest);
        static_cast<void>(stenref);
        static_cast<void>(stenop);
        static_cast<void>(stenfunc);
        static_cast<void>(otype);
    }

    bool RenderBackendSDL::putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        return SDL_RenderPoint(m_renderer, static_cast<float>(x), static_cast<float>(y));
    }

    void RenderBackendSDL::drawLine(Point const & p1, Point const & p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderLine(
            m_renderer,
            static_cast<float>(p1.x),
            static_cast<float>(p1.y),
            static_cast<float>(p2.x),
            static_cast<float>(p2.y));
    }

    void RenderBackendSDL::drawThickLine(
        Point const & p1, Point const & p2, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        float const xDiff = static_cast<float>(p2.x - p1.x);
        float const yDiff = static_cast<float>(p2.y - p1.y);
        float const halfW = static_cast<float>(width) / 2.0F;
        float angle       = (Mathf::ATan2(yDiff, xDiff) * (180.0F / Mathf::pi())) + 90.0F;
        if (angle < 0.0F) {
            angle += 360.0F;
        } else if (angle > 360.0F) {
            angle -= 360.0F;
        }
        angle *= Mathf::pi() / 180.0F;
        float const cornerX = halfW * Mathf::Cos(angle);
        float const cornerY = halfW * Mathf::Sin(angle);
        int32_t yMax        = p1.y;
        int32_t yMin        = yMax;

        std::vector<Point> points;
        Point p(
            static_cast<int32_t>(static_cast<float>(p1.x) + cornerX),
            static_cast<int32_t>(static_cast<float>(p1.y) + cornerY));
        yMax = std::max(yMax, p.y);
        yMin = std::min(yMin, p.y);
        points.push_back(p);
        p.x  = static_cast<int32_t>(static_cast<float>(p2.x) + cornerX);
        p.y  = static_cast<int32_t>(static_cast<float>(p2.y) + cornerY);
        yMax = std::max(yMax, p.y);
        yMin = std::min(yMin, p.y);
        points.push_back(p);
        p.x  = static_cast<int32_t>(static_cast<float>(p2.x) - cornerX);
        p.y  = static_cast<int32_t>(static_cast<float>(p2.y) - cornerY);
        yMax = std::max(yMax, p.y);
        yMin = std::min(yMin, p.y);
        points.push_back(p);
        p.x  = static_cast<int32_t>(static_cast<float>(p1.x) - cornerX);
        p.y  = static_cast<int32_t>(static_cast<float>(p1.y) - cornerY);
        yMax = std::max(yMax, p.y);
        yMin = std::min(yMin, p.y);
        points.push_back(p);

        // scan-line fill algorithm
        int32_t y           = yMin;
        std::size_t const n = points.size();
        for (; y <= yMax; ++y) {
            std::vector<int32_t> xs;
            std::size_t j = n - 1;
            for (std::size_t i = 0; i < n; j = i++) {
                if ((points.at(i).y < y && y <= points.at(j).y) || (points.at(j).y < y && y <= points.at(i).y)) {
                    auto x = static_cast<int32_t>(
                        static_cast<float>(points.at(i).x) +
                        (static_cast<float>(y - points.at(i).y) / static_cast<float>(points.at(j).y - points.at(i).y) *
                         static_cast<float>(points.at(j).x - points.at(i).x)));
                    xs.push_back(x);
                    for (std::size_t k = xs.size() - 1; (k != 0U) && xs.at(k - 1) > xs.at(k); --k) {
                        std::swap(xs.at(k - 1), xs.at(k));
                    }
                }
            }

            for (std::size_t i = 0; i < xs.size(); i += 2) {
                int32_t x1       = xs.at(i);
                int32_t const x2 = xs.at(i + 1);
                // vertical line
                while (x1 <= x2) {
                    putPixel(x1, y, r, g, b, a);
                    ++x1;
                }
            }
        }
    }

    void RenderBackendSDL::drawPolyLine(
        std::vector<Point> const & points, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (points.size() < 2) {
            return;
        }

        auto it   = points.begin();
        Point old = *it;
        ++it;
        if (width > 1) {
            for (; it != points.end(); ++it) {
                drawThickLine(old, *it, width, r, g, b, a);
                drawFillCircle(old, width / 2, r, g, b, a);
                old = *it;
            }
            drawFillCircle(old, width / 2, r, g, b, a);
        } else {
            for (; it != points.end(); ++it) {
                drawLine(old, *it, r, g, b, a);
                old = *it;
            }
        }
    }

    void RenderBackendSDL::drawBezier(
        std::vector<Point> const & points, int32_t steps, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (points.size() < 2) {
            return;
        }
        int32_t const elements = static_cast<int32_t>(points.size());
        if (elements < 3 || steps < 2) {
            return;
        }

        bool const thick = width > 1;
        float const step = 1.0F / static_cast<float>(steps - 1);
        float t          = 0.0;
        Point old        = getBezierPoint(points, elements + 1, t);
        if (thick) {
            for (int32_t i = 0; i <= (elements * steps); ++i) {
                t += step;
                Point const next = getBezierPoint(points, elements, t);
                drawThickLine(old, next, width, r, g, b, a);
                drawFillCircle(old, width / 2, r, g, b, a);
                old = next;
            }
            drawFillCircle(old, width / 2, r, g, b, a);
        } else {
            for (int32_t i = 0; i <= (elements * steps); ++i) {
                t += step;
                Point const next = getBezierPoint(points, elements, t);
                drawLine(old, next, r, g, b, a);
                old = next;
            }
        }
    }

    void RenderBackendSDL::drawTriangle(
        Point const & p1, Point const & p2, Point const & p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderLine(
            m_renderer,
            static_cast<float>(p1.x),
            static_cast<float>(p1.y),
            static_cast<float>(p2.x),
            static_cast<float>(p2.y));
        SDL_RenderLine(
            m_renderer,
            static_cast<float>(p2.x),
            static_cast<float>(p2.y),
            static_cast<float>(p3.x),
            static_cast<float>(p3.y));
        SDL_RenderLine(
            m_renderer,
            static_cast<float>(p3.x),
            static_cast<float>(p1.y),
            static_cast<float>(p1.x),
            static_cast<float>(p1.y));
    }

    void RenderBackendSDL::drawRectangle(
        Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_FRect rect;
        rect.x = static_cast<float>(p.x);
        rect.y = static_cast<float>(p.y);
        rect.w = static_cast<float>(w);
        rect.h = static_cast<float>(h);
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderRect(m_renderer, &rect);
    }

    void RenderBackendSDL::fillRectangle(
        Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_FRect rect;
        rect.x = static_cast<float>(p.x);
        rect.y = static_cast<float>(p.y);
        rect.w = static_cast<float>(w);
        rect.h = static_cast<float>(h);
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderFillRect(m_renderer, &rect);
    }

    void RenderBackendSDL::drawQuad(
        Point const & p1,
        [[maybe_unused]] Point const & p2,
        Point const & p3,
        [[maybe_unused]] Point const & p4,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a)
    {
        fillRectangle(p1, static_cast<uint16_t>(p3.x - p1.x), static_cast<uint16_t>(p3.y - p1.y), r, g, b, a);
    }

    void RenderBackendSDL::drawVertex(Point const & p, uint8_t const size, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        Point const p1 = Point(p.x - size, p.y + size);
        Point const p2 = Point(p.x + size, p.y + size);
        Point const p3 = Point(p.x + size, p.y - size);
        Point const p4 = Point(p.x - size, p.y - size);

        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderLine(
            m_renderer,
            static_cast<float>(p1.x),
            static_cast<float>(p1.y),
            static_cast<float>(p2.x),
            static_cast<float>(p2.y));
        SDL_RenderLine(
            m_renderer,
            static_cast<float>(p2.x),
            static_cast<float>(p2.y),
            static_cast<float>(p3.x),
            static_cast<float>(p3.y));
        SDL_RenderLine(
            m_renderer,
            static_cast<float>(p3.x),
            static_cast<float>(p3.y),
            static_cast<float>(p4.x),
            static_cast<float>(p4.y));
        SDL_RenderLine(
            m_renderer,
            static_cast<float>(p4.x),
            static_cast<float>(p4.y),
            static_cast<float>(p1.x),
            static_cast<float>(p1.y));
    }

    void RenderBackendSDL::drawCircle(Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        // Midpoint Circle Algorithm
        int32_t x           = static_cast<int32_t>(radius);
        int32_t y           = 0;
        int32_t radiusError = 1 - x;

        while (x >= y) {
            putPixel(x + p.x, y + p.y, r, g, b, a);
            putPixel(y + p.x, x + p.y, r, g, b, a);
            putPixel(-x + p.x, y + p.y, r, g, b, a);
            putPixel(-y + p.x, x + p.y, r, g, b, a);
            putPixel(-x + p.x, -y + p.y, r, g, b, a);
            putPixel(-y + p.x, -x + p.y, r, g, b, a);
            putPixel(x + p.x, -y + p.y, r, g, b, a);
            putPixel(y + p.x, -x + p.y, r, g, b, a);
            y++;
            if (radiusError < 0) {
                radiusError += (2 * y) + 1;
            } else {
                x--;
                radiusError += 2 * (y - x + 1);
            }
        }
    }

    void RenderBackendSDL::drawFillCircle(Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        float const rad = static_cast<float>(radius);
        for (int32_t dy = 1; static_cast<float>(dy) <= rad; ++dy) {
            float const dyf = static_cast<float>(dy);
            float const dx  = Mathf::Floor(Mathf::Sqrt((2.0F * rad * dyf) - (dyf * dyf)));
            int32_t x       = p.x - static_cast<int32_t>(dx);
            for (; static_cast<float>(x) <= static_cast<float>(p.x) + dx; x++) {
                putPixel(x, p.y + static_cast<int32_t>(rad - dyf), r, g, b, a);
                putPixel(x, p.y - static_cast<int32_t>(rad - dyf), r, g, b, a);
            }
        }
    }

    void RenderBackendSDL::drawCircleSegment(
        Point const & p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        float const step = Mathf::twoPi() / 360;
        int32_t s        = (sangle + 360) % 360;
        int32_t e        = (eangle + 360) % 360;
        if (e == 0) {
            e = 360;
        }
        if (s == e) {
            return;
        }

        float angle = static_cast<float>(s) * step;
        Point oldPoint(
            static_cast<int32_t>((static_cast<float>(radius) * Mathf::Cos(angle)) + static_cast<float>(p.x)),
            static_cast<int32_t>((static_cast<float>(radius) * Mathf::Sin(angle)) + static_cast<float>(p.y)));
        for (; s <= e; ++s, angle += step) {
            Point const newPoint(
                static_cast<int32_t>((static_cast<float>(radius) * Mathf::Cos(angle)) + static_cast<float>(p.x)),
                static_cast<int32_t>((static_cast<float>(radius) * Mathf::Sin(angle)) + static_cast<float>(p.y)));
            drawLine(oldPoint, newPoint, r, g, b, a);
            oldPoint = newPoint;
        }
    }

    void RenderBackendSDL::drawFillCircleSegment(
        Point const & p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        float const step = Mathf::twoPi() / 360;
        int32_t s        = (sangle + 360) % 360;
        int32_t e        = (eangle + 360) % 360;
        if (e == 0) {
            e = 360;
        }
        if (s == e) {
            return;
        }

        std::vector<Point> points;
        points.push_back(p);
        int32_t yMax = p.y;
        int32_t yMin = yMax;
        float angle  = static_cast<float>(s) * step;
        for (; s <= e; ++s, angle += step) {
            Point const newPoint(
                static_cast<int32_t>((static_cast<float>(radius) * Mathf::Cos(angle)) + static_cast<float>(p.x)),
                static_cast<int32_t>((static_cast<float>(radius) * Mathf::Sin(angle)) + static_cast<float>(p.y)));
            yMax = std::max(yMax, newPoint.y);
            yMin = std::min(yMin, newPoint.y);
            points.push_back(newPoint);
        }
        // add end point (again)
        angle = static_cast<float>(e) * step;
        Point const newPoint(
            static_cast<int32_t>((static_cast<float>(radius) * Mathf::Cos(angle)) + static_cast<float>(p.x)),
            static_cast<int32_t>((static_cast<float>(radius) * Mathf::Sin(angle)) + static_cast<float>(p.y)));
        points.push_back(newPoint);
        yMax = std::max(yMax, newPoint.y);
        yMin = std::min(yMin, newPoint.y);

        // scan-line fill algorithm
        int32_t y           = yMin;
        std::size_t const n = points.size();
        for (; y <= yMax; ++y) {
            std::vector<int32_t> xs;
            std::size_t j = n - 1;
            for (std::size_t i = 0; i < n; j = i++) {
                if ((points.at(i).y < y && y <= points.at(j).y) || (points.at(j).y < y && y <= points.at(i).y)) {
                    auto x = static_cast<int32_t>(
                        static_cast<float>(points.at(i).x) +
                        (static_cast<float>(y - points.at(i).y) / static_cast<float>(points.at(j).y - points.at(i).y) *
                         static_cast<float>(points.at(j).x - points.at(i).x)));
                    xs.push_back(x);
                    for (std::size_t k = xs.size() - 1; (k != 0U) && xs.at(k - 1) > xs.at(k); --k) {
                        std::swap(xs.at(k - 1), xs.at(k));
                    }
                }
            }

            for (std::size_t i = 0; i < xs.size(); i += 2) {
                int32_t x1       = xs.at(i);
                int32_t const x2 = xs.at(i + 1);
                // vertical line
                while (x1 <= x2) {
                    putPixel(x1, y, r, g, b, a);
                    ++x1;
                }
            }
        }
    }

    void RenderBackendSDL::drawLightPrimitive(
        Point const & p,
        uint8_t intensity,
        float radius,
        int32_t subdivisions,
        float xstretch,
        float ystretch,
        uint8_t red,
        uint8_t green,
        uint8_t blue)
    {
        static_cast<void>(p);
        static_cast<void>(intensity);
        static_cast<void>(radius);
        static_cast<void>(subdivisions);
        static_cast<void>(xstretch);
        static_cast<void>(ystretch);
        static_cast<void>(red);
        static_cast<void>(green);
        static_cast<void>(blue);
    }

    void RenderBackendSDL::enableScissorTest()
    {
    }

    void RenderBackendSDL::disableScissorTest()
    {
    }

    void RenderBackendSDL::captureScreen(std::string const & filename)
    {
        if (m_screen != nullptr) {
            uint32_t const swidth       = getWidth();
            uint32_t const sheight      = getHeight();
            int32_t const surfaceWidth  = toInt32Dimension(swidth);
            int32_t const surfaceHeight = toInt32Dimension(sheight);

            SDL_Surface* surface = SDL_CreateSurface(surfaceWidth, surfaceHeight, SDL_PIXELFORMAT_RGB24);

            if (surface == nullptr) {
                return;
            }

            SDL_BlitSurface(m_screen, nullptr, surface, nullptr);

            Image::saveAsPng(filename, *surface);
            SDL_DestroySurface(surface);
        }
    }

    void RenderBackendSDL::captureScreen(std::string const & filename, uint32_t width, uint32_t height)
    {
        if (m_screen != nullptr) {
            uint32_t const swidth  = getWidth();
            uint32_t const sheight = getHeight();
            bool const same_size   = (width == swidth && height == sheight);

            if (width < 1 || height < 1) {
                return;
            }

            if (same_size) {
                captureScreen(filename);
                return;
            }
            // create source surface
            SDL_Surface* src =
                SDL_CreateSurface(toInt32Dimension(swidth), toInt32Dimension(sheight), SDL_PIXELFORMAT_RGBA8888);

            if (src == nullptr) {
                return;
            }
            // copy screen suface to source surface
            SDL_BlitSurface(m_screen, nullptr, src, nullptr);
            // create destination surface
            SDL_Surface* dst =
                SDL_CreateSurface(toInt32Dimension(width), toInt32Dimension(height), SDL_PIXELFORMAT_RGBA8888);

            auto* src_pointer          = static_cast<uint32_t*>(src->pixels);
            uint32_t* src_help_pointer = src_pointer;
            auto* dst_pointer          = static_cast<uint32_t*>(dst->pixels);

            int32_t x    = 0;
            int32_t y    = 0;
            auto sx      = static_cast<int32_t>(0xffff * src->w / dst->w);
            auto sy      = static_cast<int32_t>(0xffff * src->h / dst->h);
            int32_t sx_c = 0;
            int32_t sy_c = 0;

            // Allocates memory and calculates row wide&height
            std::vector<int32_t> sx_a(static_cast<size_t>(dst->w) + 1);
            for (x = 0; x <= dst->w; x++) {
                sx_a.at(static_cast<size_t>(x)) = sx_c;
                sx_c &= 0xffff;
                sx_c += sx;
            }

            std::vector<int32_t> sy_a(static_cast<size_t>(dst->h) + 1);
            for (y = 0; y <= dst->h; y++) {
                sy_a.at(static_cast<size_t>(y)) = sy_c;
                sy_c &= 0xffff;
                sy_c += sy;
            }

            // Transfers the image data

            if (SDL_MUSTLOCK(src)) {
                SDL_LockSurface(src);
            }

            if (SDL_MUSTLOCK(dst)) {
                SDL_LockSurface(dst);
            }

            for (y = 0; y < dst->h; y++) {
                src_pointer = src_help_pointer;
                for (x = 0; x < dst->w; x++) {
                    *dst_pointer = *src_pointer;
                    src_pointer += // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        (sx_a.at(static_cast<size_t>(x) + 1U) >> 16);
                    ++dst_pointer; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                }
                size_t const syIdx        = static_cast<size_t>(y) + 1U;
                auto* srcBytes            = reinterpret_cast<uint8_t*>(src_help_pointer);
                size_t const srcRowOffset = static_cast<size_t>(sy_a.at(syIdx) >> 16) * static_cast<size_t>(src->pitch);
                src_help_pointer          = reinterpret_cast<uint32_t*>(
                    srcBytes + srcRowOffset); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            }

            if (SDL_MUSTLOCK(dst)) {
                SDL_UnlockSurface(dst);
            }
            if (SDL_MUSTLOCK(src)) {
                SDL_UnlockSurface(src);
            }

            Image::saveAsPng(filename, *dst);

            // Free memory
            SDL_DestroySurface(src);
            SDL_DestroySurface(dst);
        }
    }

    void RenderBackendSDL::setClipArea(Rect const & cliparea, bool clear)
    {
        SDL_Rect rect;
        rect.x = cliparea.x;
        rect.y = cliparea.y;
        rect.w = cliparea.w;
        rect.h = cliparea.h;
        SDL_SetRenderClipRect(m_renderer, &rect);
        if (clear) {
            SDL_FRect frect;
            frect.x = static_cast<float>(cliparea.x);
            frect.y = static_cast<float>(cliparea.y);
            frect.w = static_cast<float>(cliparea.w);
            frect.h = static_cast<float>(cliparea.h);
            if (m_isbackgroundcolor) {
                SDL_SetRenderDrawColor(m_renderer, m_backgroundcolor.r, m_backgroundcolor.g, m_backgroundcolor.b, 255);
            } else {
                SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            }
            SDL_RenderFillRect(m_renderer, &frect);
        }
    }

    void RenderBackendSDL::attachRenderTarget(ImagePtr& img, bool discard)
    {
        auto* image          = dynamic_cast<SDLImage*>(img.get());
        m_target             = img->getSurface();
        SDL_Texture* texture = image->getTexture();
        if (texture == nullptr) {
            texture = SDL_CreateTexture(
                m_renderer,
                m_rgba_format.format,
                SDL_TEXTUREACCESS_TARGET,
                toInt32Dimension(static_cast<uint32_t>(m_target->w)),
                toInt32Dimension(static_cast<uint32_t>(m_target->h)));
            image->setTexture(texture);
        }
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(m_renderer, texture);
        setClipArea(img->getArea(), discard);
    }

    void RenderBackendSDL::detachRenderTarget()
    {
        SDL_RenderPresent(m_renderer);
        m_target = m_screen;
        SDL_SetRenderTarget(m_renderer, nullptr);
    }

    void RenderBackendSDL::renderGuiGeometry(
        std::vector<GuiVertex> const & vertices,
        std::vector<int> const & indices,
        DoublePoint const & translation,
        ImagePtr texture)
    {
        static_cast<void>(vertices);
        static_cast<void>(indices);
        static_cast<void>(translation);
        static_cast<void>(texture);
    }

    void RenderBackendSDL::setScalingMode(ScalingMode mode)
    {
        RenderBackend::setScalingMode(mode);
        if (m_renderer != nullptr) {
            if (mode == ScalingMode::Integer) {
                SDL_SetRenderLogicalPresentation(
                    m_renderer,
                    static_cast<int>(m_baseWidth),
                    static_cast<int>(m_baseHeight),
                    SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
                SDL_SetDefaultTextureScaleMode(m_renderer, SDL_SCALEMODE_NEAREST);
            } else if (mode == ScalingMode::Nearest) {
                SDL_SetRenderLogicalPresentation(m_renderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);
                SDL_SetDefaultTextureScaleMode(m_renderer, SDL_SCALEMODE_NEAREST);
            } else {
                SDL_SetRenderLogicalPresentation(m_renderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);
                SDL_SetDefaultTextureScaleMode(m_renderer, SDL_SCALEMODE_LINEAR);
            }
        }
    }
} // namespace FIFE
