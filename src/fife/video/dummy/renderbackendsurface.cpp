// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "video/dummy/renderbackendsurface.h"

// Standard C++ library includes
#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "util/base/exception.h"
#include "util/math/fife_math.h"
#include "video/dummy/dummyimage.h"

namespace FIFE
{
    namespace
    {
        int32_t toInt32Dimension(uint32_t value)
        {
            assert(value <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
            return static_cast<int32_t>(value);
        }
    } // namespace

    RenderBackendSurface::RenderBackendSurface(SDL_Color const & colorkey, uint32_t width, uint32_t height) :
        RenderBackendDummy(colorkey, width, height), m_renderer(nullptr), m_targetTexture(nullptr)
    {
    }

    RenderBackendSurface::~RenderBackendSurface()
    {
        // Destroy dependents first: the renderer references both the screen
        // surface and any target texture.
        if (m_targetTexture != nullptr) {
            SDL_DestroyTexture(m_targetTexture);
            m_targetTexture = nullptr;
        }
        if (m_renderer != nullptr) {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }
        if (m_screen != nullptr) {
            SDL_DestroySurface(m_screen);
            m_screen = nullptr;
        }
        m_target = nullptr;
        // The software renderer needs no SDL subsystem init, so there is no
        // matching quit here and deinit() is deliberately not called.
    }

    std::string const & RenderBackendSurface::getName() const
    {
        static std::string const name = "DUMMY_SURFACE";
        return name;
    }

    void RenderBackendSurface::createMainScreen(std::string const &, std::string const &)
    {
        if (m_screen == nullptr) {
            m_screen =
                SDL_CreateSurface(toInt32Dimension(m_width), toInt32Dimension(m_height), SDL_PIXELFORMAT_RGBA8888);
            if (m_screen == nullptr) {
                throw SDLException(SDL_GetError());
            }
            m_target                = m_screen;
            m_rgba_format           = *SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888);
            Uint32 const background = SDL_MapSurfaceRGBA(m_screen, 0, 0, 0, 255);
            SDL_FillSurfaceRect(m_screen, nullptr, background);
        }

        if (m_renderer == nullptr) {
            m_renderer = SDL_CreateSoftwareRenderer(m_screen);
            if (m_renderer == nullptr) {
                throw SDLException(SDL_GetError());
            }
            SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
        }
    }

    void RenderBackendSurface::endFrame()
    {
        // Commit queued drawing to the software surface without presenting a
        // frame (there is no window).
        if (m_renderer != nullptr) {
            SDL_FlushRenderer(m_renderer);
        }
        RenderBackend::endFrame();
    }

    std::unique_ptr<Image> RenderBackendSurface::createImage(IResourceLoader* loader)
    {
        return std::make_unique<DummyImage>(loader);
    }

    std::unique_ptr<Image> RenderBackendSurface::createImage(std::string const & name, IResourceLoader* loader)
    {
        return std::make_unique<DummyImage>(name, loader);
    }

    std::unique_ptr<Image> RenderBackendSurface::createImage(uint8_t const * data, uint32_t width, uint32_t height)
    {
        return std::make_unique<DummyImage>(data, width, height);
    }

    std::unique_ptr<Image> RenderBackendSurface::createImage(
        std::string const & name, uint8_t const * data, uint32_t width, uint32_t height)
    {
        return std::make_unique<DummyImage>(name, data, width, height);
    }

    std::unique_ptr<Image> RenderBackendSurface::createImage(SDL_Surface* surface)
    {
        return std::make_unique<DummyImage>(surface);
    }

    std::unique_ptr<Image> RenderBackendSurface::createImage(std::string const & name, SDL_Surface* surface)
    {
        return std::make_unique<DummyImage>(name, surface);
    }

    void RenderBackendSurface::drawImageToTarget(
        Image const & image, Rect const & rect, uint8_t alpha, uint8_t const * rgb)
    {
        if (m_renderer == nullptr || alpha == 0 || image.getWidth() == 0 || image.getHeight() == 0) {
            return;
        }

        SDL_Surface const * source = image.getSurface();
        Rect const region          = image.isSharedImage() ? image.getSubImageRect() : image.getArea();
        if (region.w <= 0 || region.h <= 0 || rect.w <= 0 || rect.h <= 0) {
            return;
        }

        // SDL3's texture creation takes a non-const surface but does not modify it.
        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, const_cast<SDL_Surface*>(source));
        if (texture == nullptr) {
            return;
        }
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        uint8_t alphaMod = 255;
        if (rgb != nullptr) {
            std::array<uint8_t, 4> color{};
            std::memcpy(color.data(), rgb, color.size());
            SDL_SetTextureColorMod(texture, color.at(0), color.at(1), color.at(2));
            alphaMod = color.at(3);
        } else {
            SDL_SetTextureColorMod(texture, 255, 255, 255);
        }
        // SDL_RenderTexture() blits use the texture alpha mod, not the render
        // draw color, so fold both alpha inputs into the effective mod value.
        uint16_t const effectiveAlpha =
            static_cast<uint16_t>((static_cast<uint16_t>(alpha) * static_cast<uint16_t>(alphaMod)) / 255U);
        SDL_SetTextureAlphaMod(texture, static_cast<uint8_t>(effectiveAlpha));

        SDL_FRect const src{
            .x = static_cast<float>(region.x),
            .y = static_cast<float>(region.y),
            .w = static_cast<float>(region.w),
            .h = static_cast<float>(region.h)};
        SDL_FRect const dst{
            .x = static_cast<float>(rect.x),
            .y = static_cast<float>(rect.y),
            .w = static_cast<float>(rect.w),
            .h = static_cast<float>(rect.h)};

        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        SDL_RenderTexture(m_renderer, texture, &src, &dst);
        // SDL flushes any queued commands that still reference the texture.
        SDL_DestroyTexture(texture);
    }

    SDL_Surface* RenderBackendSurface::readTarget() const
    {
        if (m_renderer == nullptr) {
            return nullptr;
        }
        SDL_FlushRenderer(m_renderer);
        return SDL_RenderReadPixels(m_renderer, nullptr);
    }

    void RenderBackendSurface::captureScreen(std::string const & filename)
    {
        SDL_Surface* captured = readTarget();
        if (captured == nullptr) {
            return;
        }
        Image::saveAsPng(filename, *captured);
        SDL_DestroySurface(captured);
    }

    void RenderBackendSurface::captureScreen(std::string const & filename, uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0) {
            return;
        }
        SDL_Surface* captured = readTarget();
        if (captured == nullptr) {
            return;
        }

        if (width == static_cast<uint32_t>(captured->w) && height == static_cast<uint32_t>(captured->h)) {
            Image::saveAsPng(filename, *captured);
        } else {
            SDL_Surface* scaled =
                SDL_ScaleSurface(captured, toInt32Dimension(width), toInt32Dimension(height), SDL_SCALEMODE_LINEAR);
            if (scaled != nullptr) {
                Image::saveAsPng(filename, *scaled);
                SDL_DestroySurface(scaled);
            }
        }
        SDL_DestroySurface(captured);
    }

    bool RenderBackendSurface::putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (m_renderer == nullptr) {
            return false;
        }
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        return SDL_RenderPoint(m_renderer, static_cast<float>(x), static_cast<float>(y));
    }

    void RenderBackendSurface::drawLine(Point const & p1, Point const & p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (m_renderer == nullptr) {
            return;
        }
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderLine(
            m_renderer,
            static_cast<float>(p1.x),
            static_cast<float>(p1.y),
            static_cast<float>(p2.x),
            static_cast<float>(p2.y));
    }

    void RenderBackendSurface::drawThickLine(
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

        // Scan-line fill of the four corner points.
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

            for (std::size_t i = 0; i + 1 < xs.size(); i += 2) {
                int32_t x1       = xs.at(i);
                int32_t const x2 = xs.at(i + 1);
                while (x1 <= x2) {
                    putPixel(x1, y, r, g, b, a);
                    ++x1;
                }
            }
        }
    }

    void RenderBackendSurface::drawPolyLine(
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

    void RenderBackendSurface::drawBezier(
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
        float t          = 0.0F;
        Point old        = getBezierPoint(points, elements, t);
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

    void RenderBackendSurface::fillTriangle(
        Point const & p1, Point const & p2, Point const & p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (m_renderer == nullptr) {
            return;
        }
        SDL_FColor color{
            .r = static_cast<float>(r) / 255.0F,
            .g = static_cast<float>(g) / 255.0F,
            .b = static_cast<float>(b) / 255.0F,
            .a = static_cast<float>(a) / 255.0F};

        std::array<Point, 3> const points = {p1, p2, p3};
        std::array<SDL_Vertex, 3> vertices{};
        for (std::size_t i = 0; i < points.size(); ++i) {
            vertices.at(i).position.x  = static_cast<float>(points.at(i).x);
            vertices.at(i).position.y  = static_cast<float>(points.at(i).y);
            vertices.at(i).color       = color;
            vertices.at(i).tex_coord.x = 0.0F;
            vertices.at(i).tex_coord.y = 0.0F;
        }

        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderGeometry(m_renderer, nullptr, vertices.data(), 3, nullptr, 0);
    }

    void RenderBackendSurface::drawRectangle(
        Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (m_renderer == nullptr) {
            return;
        }
        SDL_FRect rect{
            .x = static_cast<float>(p.x),
            .y = static_cast<float>(p.y),
            .w = static_cast<float>(w),
            .h = static_cast<float>(h)};
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderRect(m_renderer, &rect);
    }

    void RenderBackendSurface::fillRectangle(
        Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (m_renderer == nullptr) {
            return;
        }
        SDL_FRect rect{
            .x = static_cast<float>(p.x),
            .y = static_cast<float>(p.y),
            .w = static_cast<float>(w),
            .h = static_cast<float>(h)};
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderFillRect(m_renderer, &rect);
    }

    void RenderBackendSurface::drawCircle(Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        // Midpoint circle algorithm, identical in shape to the SDL backend.
        int32_t x           = toInt32Dimension(radius);
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
            ++y;
            if (radiusError < 0) {
                radiusError += (2 * y) + 1;
            } else {
                --x;
                radiusError += 2 * (y - x + 1);
            }
        }
    }

    void RenderBackendSurface::drawFillCircle(
        Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        float const rad = static_cast<float>(radius);
        for (int32_t dy = 1; static_cast<float>(dy) <= rad; ++dy) {
            float const dyf = static_cast<float>(dy);
            float const dx  = Mathf::Floor(Mathf::Sqrt((2.0F * rad * dyf) - (dyf * dyf)));
            int32_t x       = p.x - static_cast<int32_t>(dx);
            for (; static_cast<float>(x) <= static_cast<float>(p.x) + dx; ++x) {
                putPixel(x, p.y + static_cast<int32_t>(rad - dyf), r, g, b, a);
                putPixel(x, p.y - static_cast<int32_t>(rad - dyf), r, g, b, a);
            }
        }
    }

    void RenderBackendSurface::drawCircleSegment(
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

    void RenderBackendSurface::drawFillCircleSegment(
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
        angle = static_cast<float>(e) * step;
        Point const newPoint(
            static_cast<int32_t>((static_cast<float>(radius) * Mathf::Cos(angle)) + static_cast<float>(p.x)),
            static_cast<int32_t>((static_cast<float>(radius) * Mathf::Sin(angle)) + static_cast<float>(p.y)));
        points.push_back(newPoint);
        yMax = std::max(yMax, newPoint.y);
        yMin = std::min(yMin, newPoint.y);

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

            for (std::size_t i = 0; i + 1 < xs.size(); i += 2) {
                int32_t x1       = xs.at(i);
                int32_t const x2 = xs.at(i + 1);
                while (x1 <= x2) {
                    putPixel(x1, y, r, g, b, a);
                    ++x1;
                }
            }
        }
    }

    void RenderBackendSurface::attachRenderTarget(ImagePtr& img, bool discard)
    {
        if (m_renderer == nullptr || !img) {
            return;
        }
        SDL_Surface* surface = img->getSurface();
        if (surface == nullptr) {
            return;
        }

        if (m_targetTexture != nullptr) {
            float textureWidth  = 0.0F;
            float textureHeight = 0.0F;
            if (!SDL_GetTextureSize(m_targetTexture, &textureWidth, &textureHeight) ||
                static_cast<int32_t>(textureWidth) != surface->w || static_cast<int32_t>(textureHeight) != surface->h) {
                // A different target was attached: rebuild the backing texture.
                SDL_DestroyTexture(m_targetTexture);
                m_targetTexture = nullptr;
            }
        }
        m_target = surface;
        if (m_targetTexture == nullptr) {
            m_targetTexture = SDL_CreateTexture(
                m_renderer,
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_TARGET,
                toInt32Dimension(static_cast<uint32_t>(surface->w)),
                toInt32Dimension(static_cast<uint32_t>(surface->h)));
        }
        if (m_targetTexture == nullptr) {
            return;
        }
        if (!discard) {
            // Standard FIFE images use SDL_PIXELFORMAT_RGBA32 (ABGR8888 on
            // little-endian), which is a different enum from the target
            // texture's RGBA8888. Convert before uploading so existing pixels
            // are not silently dropped when discard is false.
            SDL_Surface* const upload = surface->format == SDL_PIXELFORMAT_RGBA8888 ?
                                            surface :
                                            SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA8888);
            if (upload != nullptr) {
                SDL_UpdateTexture(m_targetTexture, nullptr, upload->pixels, upload->pitch);
                if (upload != surface) {
                    SDL_DestroySurface(upload);
                }
            }
        }

        SDL_SetRenderTarget(m_renderer, m_targetTexture);
        setClipArea(img->getArea(), discard);
    }

    void RenderBackendSurface::detachRenderTarget()
    {
        if (m_renderer == nullptr) {
            return;
        }

        // Read the texture target before switching back to the screen surface.
        if (m_targetTexture != nullptr && m_target != nullptr && m_target != m_screen) {
            SDL_Surface* captured = readTarget();
            if (captured != nullptr) {
                SDL_Surface* converted =
                    captured->format == m_target->format ? captured : SDL_ConvertSurface(captured, m_target->format);
                if (converted != nullptr) {
                    if (converted->w == m_target->w && converted->h == m_target->h) {
                        SDL_Rect const dst{0, 0, m_target->w, m_target->h};
                        SDL_BlitSurface(converted, nullptr, m_target, &dst);
                    }
                    if (converted != captured) {
                        SDL_DestroySurface(converted);
                    }
                }
                SDL_DestroySurface(captured);
            }
        }

        SDL_SetRenderTarget(m_renderer, nullptr);
        if (m_targetTexture != nullptr) {
            SDL_DestroyTexture(m_targetTexture);
            m_targetTexture = nullptr;
        }
        m_target = m_screen;
    }

    void RenderBackendSurface::setClipArea(Rect const & cliparea, bool clear)
    {
        if (m_renderer == nullptr) {
            return;
        }
        SDL_Rect const rect{cliparea.x, cliparea.y, cliparea.w, cliparea.h};
        SDL_SetRenderClipRect(m_renderer, &rect);
        if (clear) {
            SDL_FRect const frect{
                .x = static_cast<float>(cliparea.x),
                .y = static_cast<float>(cliparea.y),
                .w = static_cast<float>(cliparea.w),
                .h = static_cast<float>(cliparea.h)};
            if (m_isbackgroundcolor) {
                SDL_SetRenderDrawColor(m_renderer, m_backgroundcolor.r, m_backgroundcolor.g, m_backgroundcolor.b, 255);
            } else {
                SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            }
            SDL_RenderFillRect(m_renderer, &frect);
        }
    }

} // namespace FIFE
