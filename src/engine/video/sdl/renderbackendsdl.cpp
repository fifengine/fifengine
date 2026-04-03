// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <limits>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "video/devicecaps.h"

#include "SDL_image.h"
#include "renderbackendsdl.h"
#include "sdlimage.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIDEO);

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

    RenderBackendSDL::RenderBackendSDL(const SDL_Color& colorkey) : RenderBackend(colorkey), m_renderer(nullptr) { }

    RenderBackendSDL::~RenderBackendSDL()
    {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        deinit();
    }

    const std::string& RenderBackendSDL::getName() const
    {
        static std::string const backend_name = "SDL";
        return backend_name;
    }

    void RenderBackendSDL::init(const std::string& driver)
    {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
            throw SDLException(SDL_GetError());
        }
        if (!driver.empty()) {
            if (SDL_VideoInit(driver.c_str()) < 0) {
                throw SDLException(SDL_GetError());
            }
        }
    }

    void RenderBackendSDL::clearBackBuffer()
    {
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
        SDL_RenderClear(m_renderer);
    }

    void RenderBackendSDL::createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon)
    {
        setScreenMode(mode);
        if (m_window != nullptr) {
            if (!icon.empty()) {
                SDL_Surface* img = IMG_Load(icon.c_str());
                if (img != nullptr) {
                    SDL_SetWindowIcon(m_window, img);
                    SDL_FreeSurface(img);
                }
            }
            SDL_SetWindowTitle(m_window, title.c_str());
        }
    }

    void RenderBackendSDL::setScreenMode(const ScreenMode& mode)
    {
        uint16_t const width        = mode.getWidth();
        uint16_t const height       = mode.getHeight();
        uint16_t const bitsPerPixel = mode.getBPP();
        uint32_t flags              = mode.getSDLFlags();
        // in case of recreating
        if (m_window != nullptr) {
            SDL_DestroyRenderer(m_renderer);
            SDL_DestroyWindow(m_window);
            m_screen = nullptr;
        }
        // create window
        uint8_t const displayIndex = mode.getDisplay();
        int32_t const windowX      = mode.getWindowPositionX();
        int32_t const windowY      = mode.getWindowPositionY();

        // Determine X position
        int xPos{};
        if (windowX >= 0) {
            xPos = windowX;
        } else if (mode.isFullScreen()) {
            xPos = toDisplayWindowPos(displayIndex, false);
        } else {
            xPos = toDisplayWindowPos(displayIndex, true);
        }

        // Determine Y position
        int yPos{};
        if (windowY >= 0) {
            yPos = windowY;
        } else if (mode.isFullScreen()) {
            yPos = toDisplayWindowPos(displayIndex, false);
        } else {
            yPos = toDisplayWindowPos(displayIndex, true);
        }

        if (mode.isFullScreen()) {
            m_window = SDL_CreateWindow(
                "",
                toDisplayWindowPos(displayIndex, false),
                toDisplayWindowPos(displayIndex, false),
                width,
                height,
                flags | SDL_WINDOW_SHOWN);
        } else {
            m_window = SDL_CreateWindow("", xPos, yPos, width, height, flags | SDL_WINDOW_SHOWN);
        }

        if (m_window == nullptr) {
            throw SDLException(SDL_GetError());
        }
        // make sure the window have the right settings
        SDL_DisplayMode displayMode;
        displayMode.format       = mode.getFormat();
        displayMode.w            = width;
        displayMode.h            = height;
        displayMode.refresh_rate = mode.getRefreshRate();
        if (SDL_SetWindowDisplayMode(m_window, &displayMode) != 0) {
            throw SDLException(SDL_GetError());
        }

        // create renderer with given flags
        flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
        if (m_vSync) {
            flags |= SDL_RENDERER_PRESENTVSYNC;
        }
        m_renderer = SDL_CreateRenderer(m_window, mode.getRenderDriverIndex(), flags);
        if (m_renderer == nullptr) {
            throw SDLException(SDL_GetError());
        }
        // set texture filtering
        if (m_textureFilter == TEXTURE_FILTER_ANISOTROPIC) {
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
        } else if (m_textureFilter != TEXTURE_FILTER_NONE) {
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        } else {
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
        }
        // enable alpha blending
        SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

        // set the window surface as main surface, not really needed anymore
        m_screen = SDL_GetWindowSurface(m_window);
        m_target = m_screen;
        if (m_screen == nullptr) {
            throw SDLException(SDL_GetError());
        }

        FL_LOG(
            _log,
            LMsg("RenderBackendSDL") << "Videomode " << width << "x" << height << " at " << int32_t(bitsPerPixel)
                                     << " bpp with " << displayMode.refresh_rate << " Hz");

        // this is needed, otherwise we would have screen pixel formats which will not work with
        // our texture generation. 32 bit surfaces to BitsPerPixel texturen.
        m_rgba_format = *(m_screen->format);
        if (bitsPerPixel != 16) {
            m_rgba_format.format       = SDL_PIXELFORMAT_RGBA8888;
            m_rgba_format.BitsPerPixel = 32;
        } else {
            m_rgba_format.format       = SDL_PIXELFORMAT_RGBA4444;
            m_rgba_format.BitsPerPixel = 16;
        }
        m_rgba_format.Rmask = RMASK;
        m_rgba_format.Gmask = GMASK;
        m_rgba_format.Bmask = BMASK;
        m_rgba_format.Amask = AMASK;

        // update the screen mode with the actual flags used
        m_screenMode = mode;
    }

    void RenderBackendSDL::startFrame()
    {
        RenderBackend::startFrame();
    }

    void RenderBackendSDL::endFrame()
    {
        SDL_RenderPresent(m_renderer);
        RenderBackend::endFrame();
    }

    Image* RenderBackendSDL::createImage(IResourceLoader* loader)
    {
        return new SDLImage(loader);
    }

    Image* RenderBackendSDL::createImage(const std::string& name, IResourceLoader* loader)
    {
        return new SDLImage(name, loader);
    }

    Image* RenderBackendSDL::createImage(SDL_Surface* surface)
    {
        return new SDLImage(surface);
    }

    Image* RenderBackendSDL::createImage(const std::string& name, SDL_Surface* surface)
    {
        return new SDLImage(name, surface);
    }

    Image* RenderBackendSDL::createImage(const uint8_t* data, uint32_t width, uint32_t height)
    {
        return new SDLImage(data, width, height);
    }

    Image* RenderBackendSDL::createImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height)
    {
        return new SDLImage(name, data, width, height);
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

    void RenderBackendSDL::resetLighting() { }

    void RenderBackendSDL::resetStencilBuffer(uint8_t buffer)
    {
        static_cast<void>(buffer);
    }

    void RenderBackendSDL::changeBlending(int32_t scr, int32_t dst)
    {
        static_cast<void>(scr);
        static_cast<void>(dst);
    }

    void RenderBackendSDL::renderVertexArrays() { }

    void RenderBackendSDL::addImageToArray(
        uint32_t id, const Rect& rec, float const * st, uint8_t alpha, uint8_t const * rgba)
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
        return SDL_RenderDrawPoint(m_renderer, x, y) == 0;
    }

    void RenderBackendSDL::drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
    }

    void RenderBackendSDL::drawThickLine(
        const Point& p1, const Point& p2, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        const float xDiff = static_cast<float>(p2.x - p1.x);
        const float yDiff = static_cast<float>(p2.y - p1.y);
        const float halfW = static_cast<float>(width) / 2.0F;
        float angle       = (Mathf::ATan2(yDiff, xDiff) * (180.0F / Mathf::pi())) + 90.0F;
        if (angle < 0.0F) {
            angle += 360.0F;
        } else if (angle > 360.0F) {
            angle -= 360.0F;
        }
        angle *= Mathf::pi() / 180.0F;
        const float cornerX = halfW * Mathf::Cos(angle);
        const float cornerY = halfW * Mathf::Sin(angle);
        int32_t yMax        = p1.y;
        int32_t yMin        = p1.y;

        std::vector<Point> points;
        Point p(p1.x + cornerX, p1.y + cornerY);
        yMax = std::max(yMax, p.y);
        yMin = std::min(yMin, p.y);
        points.push_back(p);
        p.x  = p2.x + cornerX;
        p.y  = p2.y + cornerY;
        yMax = std::max(yMax, p.y);
        yMin = std::min(yMin, p.y);
        points.push_back(p);
        p.x  = p2.x - cornerX;
        p.y  = p2.y - cornerY;
        yMax = std::max(yMax, p.y);
        yMin = std::min(yMin, p.y);
        points.push_back(p);
        p.x  = p1.x - cornerX;
        p.y  = p1.y - cornerY;
        yMax = std::max(yMax, p.y);
        yMin = std::min(yMin, p.y);
        points.push_back(p);

        // scan-line fill algorithm
        int32_t y           = yMin;
        const std::size_t n = points.size();
        for (; y <= yMax; ++y) {
            std::vector<int32_t> xs;
            std::size_t j = n - 1;
            for (std::size_t i = 0; i < n; j = i++) {
                if ((points[i].y < y && y <= points[j].y) || (points[j].y < y && y <= points[i].y)) {
                    auto x = static_cast<int32_t>(
                        points[i].x +
                        (static_cast<float>(y - points[i].y) / static_cast<float>(points[j].y - points[i].y) *
                         static_cast<float>(points[j].x - points[i].x)));
                    xs.push_back(x);
                    for (std::size_t k = xs.size() - 1; (k != 0U) && xs[k - 1] > xs[k]; --k) {
                        std::swap(xs[k - 1], xs[k]);
                    }
                }
            }

            for (std::size_t i = 0; i < xs.size(); i += 2) {
                int32_t x1       = xs[i];
                int32_t const x2 = xs[i + 1];
                // vertical line
                while (x1 <= x2) {
                    putPixel(x1, y, r, g, b, a);
                    ++x1;
                }
            }
        }
    }

    void RenderBackendSDL::drawPolyLine(
        const std::vector<Point>& points, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
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
        const std::vector<Point>& points, int32_t steps, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (points.size() < 2) {
            return;
        }
        const int32_t elements = static_cast<int32_t>(points.size());
        if (elements < 3 || steps < 2) {
            return;
        }

        bool const thick = width > 1;
        float const step = 1.0 / static_cast<float>(steps - 1);
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
        const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
        SDL_RenderDrawLine(m_renderer, p2.x, p2.y, p3.x, p3.y);
        SDL_RenderDrawLine(m_renderer, p3.x, p1.y, p1.x, p1.y);
    }

    void RenderBackendSDL::drawRectangle(
        const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_Rect rect;
        rect.x = p.x;
        rect.y = p.y;
        rect.w = w;
        rect.h = h;
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderDrawRect(m_renderer, &rect);
    }

    void RenderBackendSDL::fillRectangle(
        const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_Rect rect;
        rect.x = p.x;
        rect.y = p.y;
        rect.w = w;
        rect.h = h;
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderFillRect(m_renderer, &rect);
    }

    void RenderBackendSDL::drawQuad(
        const Point& p1, const Point& p2, const Point& p3, const Point& p4, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        fillRectangle(p1, static_cast<uint16_t>(p3.x - p1.x), static_cast<uint16_t>(p3.y - p1.y), r, g, b, a);
    }

    void RenderBackendSDL::drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        Point const p1 = Point(p.x - size, p.y + size);
        Point const p2 = Point(p.x + size, p.y + size);
        Point const p3 = Point(p.x + size, p.y - size);
        Point const p4 = Point(p.x - size, p.y - size);

        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
        SDL_RenderDrawLine(m_renderer, p2.x, p2.y, p3.x, p3.y);
        SDL_RenderDrawLine(m_renderer, p3.x, p3.y, p4.x, p4.y);
        SDL_RenderDrawLine(m_renderer, p4.x, p4.y, p1.x, p1.y);
    }

    void RenderBackendSDL::drawCircle(const Point& p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        // Midpoint Circle Algorithm
        int32_t x           = radius;
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
                radiusError += 2 * y + 1;
            } else {
                x--;
                radiusError += 2 * (y - x + 1);
            }
        }
    }

    void RenderBackendSDL::drawFillCircle(const Point& p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        const float rad = static_cast<float>(radius);
        for (float dy = 1.0F; dy <= rad; dy += 1.0F) {
            const float dx = Mathf::Floor(Mathf::Sqrt((2.0F * rad * dy) - (dy * dy)));
            int32_t x      = p.x - static_cast<int32_t>(dx);
            for (; x <= p.x + dx; x++) {
                putPixel(x, p.y + static_cast<int32_t>(rad - dy), r, g, b, a);
                putPixel(x, p.y - static_cast<int32_t>(rad - dy), r, g, b, a);
            }
        }
    }

    void RenderBackendSDL::drawCircleSegment(
        const Point& p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        const float step = Mathf::twoPi() / 360;
        int32_t s        = (sangle + 360) % 360;
        int32_t e        = (eangle + 360) % 360;
        if (e == 0) {
            e = 360;
        }
        if (s == e) {
            return;
        }

        float angle = static_cast<float>(s) * step;
        Point oldPoint((radius * Mathf::Cos(angle)) + p.x, (radius * Mathf::Sin(angle)) + p.y);
        for (; s <= e; ++s, angle += step) {
            Point const newPoint((radius * Mathf::Cos(angle)) + p.x, (radius * Mathf::Sin(angle)) + p.y);
            drawLine(oldPoint, newPoint, r, g, b, a);
            oldPoint = newPoint;
        }
    }

    void RenderBackendSDL::drawFillCircleSegment(
        const Point& p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        const float step = Mathf::twoPi() / 360;
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
        int32_t yMin = p.y;
        float angle  = static_cast<float>(s) * step;
        for (; s <= e; ++s, angle += step) {
            Point const newPoint((radius * Mathf::Cos(angle)) + p.x, (radius * Mathf::Sin(angle)) + p.y);
            yMax = std::max(yMax, newPoint.y);
            yMin = std::min(yMin, newPoint.y);
            points.push_back(newPoint);
        }
        // add end point (again)
        angle = static_cast<float>(e) * step;
        Point const newPoint((radius * Mathf::Cos(angle)) + p.x, (radius * Mathf::Sin(angle)) + p.y);
        points.push_back(newPoint);
        yMax = std::max(yMax, newPoint.y);
        yMin = std::min(yMin, newPoint.y);

        // scan-line fill algorithm
        int32_t y           = yMin;
        const std::size_t n = points.size();
        for (; y <= yMax; ++y) {
            std::vector<int32_t> xs;
            std::size_t j = n - 1;
            for (std::size_t i = 0; i < n; j = i++) {
                if ((points[i].y < y && y <= points[j].y) || (points[j].y < y && y <= points[i].y)) {
                    auto x = static_cast<int32_t>(
                        points[i].x +
                        (static_cast<float>(y - points[i].y) / static_cast<float>(points[j].y - points[i].y) *
                         static_cast<float>(points[j].x - points[i].x)));
                    xs.push_back(x);
                    for (std::size_t k = xs.size() - 1; (k != 0U) && xs[k - 1] > xs[k]; --k) {
                        std::swap(xs[k - 1], xs[k]);
                    }
                }
            }

            for (std::size_t i = 0; i < xs.size(); i += 2) {
                int32_t x1       = xs[i];
                int32_t const x2 = xs[i + 1];
                // vertical line
                while (x1 <= x2) {
                    putPixel(x1, y, r, g, b, a);
                    ++x1;
                }
            }
        }
    }

    void RenderBackendSDL::drawLightPrimitive(
        const Point& p,
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

    void RenderBackendSDL::enableScissorTest() { }

    void RenderBackendSDL::disableScissorTest() { }

    void RenderBackendSDL::captureScreen(const std::string& filename)
    {
        if (m_screen != nullptr) {
            const uint32_t swidth       = getWidth();
            const uint32_t sheight      = getHeight();
            const int32_t surfaceWidth  = toInt32Dimension(swidth);
            const int32_t surfaceHeight = toInt32Dimension(sheight);

            SDL_Surface* surface =
                SDL_CreateRGBSurface(0, surfaceWidth, surfaceHeight, 24, RMASK, GMASK, BMASK, NULLMASK);

            if (surface == nullptr) {
                return;
            }

            SDL_BlitSurface(m_screen, nullptr, surface, nullptr);

            Image::saveAsPng(filename, *surface);
            SDL_FreeSurface(surface);
        }
    }

    void RenderBackendSDL::captureScreen(const std::string& filename, uint32_t width, uint32_t height)
    {
        if (m_screen != nullptr) {
            const uint32_t swidth  = getWidth();
            const uint32_t sheight = getHeight();
            const bool same_size   = (width == swidth && height == sheight);

            if (width < 1 || height < 1) {
                return;
            }

            if (same_size) {
                captureScreen(filename);
                return;
            }
            // create source surface
            SDL_Surface* src = SDL_CreateRGBSurface(
                0, toInt32Dimension(swidth), toInt32Dimension(sheight), 32, RMASK, GMASK, BMASK, AMASK);

            if (src == nullptr) {
                return;
            }
            // copy screen suface to source surface
            SDL_BlitSurface(m_screen, nullptr, src, nullptr);
            // create destination surface
            SDL_Surface* dst = SDL_CreateRGBSurface(
                0, toInt32Dimension(width), toInt32Dimension(height), 32, RMASK, GMASK, BMASK, AMASK);

            auto* src_pointer          = static_cast<uint32_t*>(src->pixels);
            uint32_t* src_help_pointer = src_pointer;
            auto* dst_pointer          = static_cast<uint32_t*>(dst->pixels);

            int32_t x      = 0;
            int32_t y      = 0;
            int32_t* sx_ca = nullptr;
            int32_t* sy_ca = nullptr;
            auto sx        = static_cast<int32_t>(0xffff * src->w / dst->w);
            auto sy        = static_cast<int32_t>(0xffff * src->h / dst->h);
            int32_t sx_c   = 0;
            int32_t sy_c   = 0;

            // Allocates memory and calculates row wide&height
            auto* sx_a = new int32_t[dst->w + 1];
            sx_ca      = sx_a;
            for (x = 0; x <= dst->w; x++) {
                *sx_ca = sx_c;
                sx_ca++;
                sx_c &= 0xffff;
                sx_c += sx;
            }

            auto* sy_a = new int32_t[dst->h + 1];
            sy_ca      = sy_a;
            for (y = 0; y <= dst->h; y++) {
                *sy_ca = sy_c;
                sy_ca++;
                sy_c &= 0xffff;
                sy_c += sy;
            }
            sy_ca = sy_a;

            // Transfers the image data

            if (SDL_MUSTLOCK(src)) {
                SDL_LockSurface(src);
            }

            if (SDL_MUSTLOCK(dst)) {
                SDL_LockSurface(dst);
            }

            for (y = 0; y < dst->h; y++) {
                src_pointer = src_help_pointer;
                sx_ca       = sx_a;
                for (x = 0; x < dst->w; x++) {
                    *dst_pointer = *src_pointer;
                    sx_ca++;
                    src_pointer += (*sx_ca >> 16);
                    dst_pointer++;
                }
                sy_ca++;
                auto* srcBytes   = static_cast<uint8_t*>(static_cast<void*>(src_help_pointer));
                src_help_pointer = static_cast<uint32_t*>(static_cast<void*>(srcBytes + ((*sy_ca >> 16) * src->pitch)));
            }

            if (SDL_MUSTLOCK(dst)) {
                SDL_UnlockSurface(dst);
            }
            if (SDL_MUSTLOCK(src)) {
                SDL_UnlockSurface(src);
            }

            Image::saveAsPng(filename, *dst);

            // Free memory
            SDL_FreeSurface(src);
            SDL_FreeSurface(dst);
            delete[] sx_a;
            delete[] sy_a;
        }
    }

    void RenderBackendSDL::setClipArea(const Rect& cliparea, bool clear)
    {
        SDL_Rect rect;
        rect.x = cliparea.x;
        rect.y = cliparea.y;
        rect.w = cliparea.w;
        rect.h = cliparea.h;
        SDL_RenderSetClipRect(m_renderer, &rect);
        if (clear) {
            if (m_isbackgroundcolor) {
                SDL_SetRenderDrawColor(m_renderer, m_backgroundcolor.r, m_backgroundcolor.g, m_backgroundcolor.b, 255);
            } else {
                SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            }
            SDL_RenderFillRect(m_renderer, &rect);
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
        const std::vector<GuiVertex>& vertices,
        const std::vector<int>& indices,
        const DoublePoint& translation,
        ImagePtr texture)
    {
        static_cast<void>(vertices);
        static_cast<void>(indices);
        static_cast<void>(translation);
        static_cast<void>(texture);
    }
} // namespace FIFE
