// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "glcore_gui_graphics.h"

// Standard C++ library includes
#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

// 3rd party library includes
#include <SDL3/SDL.h>

#include <fifechan/cliprectangle.hpp>
#include <fifechan/image.hpp>

// FIFE includes
#include "gui/fifechan/base/gui_image.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/renderbackend.h"

namespace FIFE
{
    GLCoreGuiGraphics::GLCoreGuiGraphics() :
        m_renderbackend(RenderBackend::instance()),
        m_width(static_cast<int32_t>(m_renderbackend->getWidth())),
        m_height(static_cast<int32_t>(m_renderbackend->getHeight()))
    {
    }

    void GLCoreGuiGraphics::updateTarget()
    {
        m_renderbackend = RenderBackend::instance();
        m_width         = static_cast<int32_t>(m_renderbackend->getWidth());
        m_height        = static_cast<int32_t>(m_renderbackend->getHeight());
    }

    uint8_t GLCoreGuiGraphics::effectiveAlpha() const
    {
        float const alpha = static_cast<float>(m_color.a) * mOpacity;
        if (alpha <= 0.0F) {
            return 0;
        }
        if (alpha >= 255.0F) {
            return 255;
        }
        return static_cast<uint8_t>(alpha);
    }

    void GLCoreGuiGraphics::drawImage(
        fcn::Image const * image,
        [[maybe_unused]] int32_t srcX,
        [[maybe_unused]] int32_t srcY,
        int32_t dstX,
        int32_t dstY,
        int32_t width,
        int32_t height)
    {
        if (image == nullptr || m_renderbackend == nullptr || width <= 0 || height <= 0) {
            return;
        }

        int xOffset = 0;
        int yOffset = 0;
        if (!mClipStack.empty()) {
            xOffset = mClipStack.top().xOffset;
            yOffset = mClipStack.top().yOffset;
        }
        Rect const rect(dstX + xOffset, dstY + yOffset, width, height);

        // GuiImage is the bridge between a Fifechan image and a FIFE image.
        // Image::render() then routes to the backend's generic image hook.
        auto const * guiImage = dynamic_cast<GuiImage const *>(image);
        if (guiImage != nullptr) {
            ImagePtr const fifeimg = guiImage->getFIFEImage();
            if (fifeimg) {
                fifeimg->render(rect, effectiveAlpha());
            }
            return;
        }

        // Generic Fifechan images have no backend handle. Rebuild their pixels
        // as a backend image and render through the same generic hook.
        // Fifechan's getPixel() is non-const even though it only reads.
        auto* source = const_cast<fcn::Image*>(image); // NOLINT(cppcoreguidelines-pro-type-const-cast)
        int const sw = source->getWidth();
        int const sh = source->getHeight();
        if (sw <= 0 || sh <= 0) {
            return;
        }

        auto const pixelCount = static_cast<std::size_t>(sw) * static_cast<std::size_t>(sh);
        std::vector<uint8_t> rgba(pixelCount * 4U, 0);
        for (int y = 0; y < sh; ++y) {
            for (int x = 0; x < sw; ++x) {
                fcn::Color const color = source->getPixel(x, y);
                std::size_t const i =
                    ((static_cast<std::size_t>(y) * static_cast<std::size_t>(sw)) + static_cast<std::size_t>(x)) * 4U;
                rgba.at(i + 0U) = color.r;
                rgba.at(i + 1U) = color.g;
                rgba.at(i + 2U) = color.b;
                rgba.at(i + 3U) = static_cast<uint8_t>(
                    static_cast<uint16_t>(color.a) * static_cast<uint16_t>(effectiveAlpha()) / 255U);
            }
        }
        auto fallback = m_renderbackend->createImage(rgba.data(), static_cast<uint32_t>(sw), static_cast<uint32_t>(sh));
        if (fallback) {
            fallback->render(rect);
        }
    }

    void GLCoreGuiGraphics::drawSurface(SDL_Surface* surface, int dstX, int dstY)
    {
        if (surface == nullptr || m_renderbackend == nullptr) {
            return;
        }

        int xOffset = 0;
        int yOffset = 0;
        if (!mClipStack.empty()) {
            xOffset = mClipStack.top().xOffset;
            yOffset = mClipStack.top().yOffset;
        }

        // createImage() takes ownership, so duplicate the caller-owned surface.
        SDL_Surface* copy = SDL_DuplicateSurface(surface);
        if (copy == nullptr) {
            return;
        }
        auto image = m_renderbackend->createImage(copy);
        if (!image) {
            SDL_DestroySurface(copy);
            return;
        }

        Rect const rect(dstX + xOffset, dstY + yOffset, surface->w, surface->h);
        std::array<uint8_t, 4> const rgb{m_color.r, m_color.g, m_color.b, 255};
        image->render(rect, effectiveAlpha(), rgb.data());
    }

    void GLCoreGuiGraphics::drawPoint(int32_t x, int32_t y)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->putPixel(x + top.xOffset, y + top.yOffset, m_color.r, m_color.g, m_color.b, effectiveAlpha());
    }

    void GLCoreGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawLine(
            Point(x1 + top.xOffset, y1 + top.yOffset),
            Point(x2 + top.xOffset, y2 + top.yOffset),
            m_color.r,
            m_color.g,
            m_color.b,
            effectiveAlpha());
    }

    void GLCoreGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t width)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawThickLine(
            Point(x1 + top.xOffset, y1 + top.yOffset),
            Point(x2 + top.xOffset, y2 + top.yOffset),
            static_cast<uint8_t>(width),
            m_color.r,
            m_color.g,
            m_color.b,
            effectiveAlpha());
    }

    void GLCoreGuiGraphics::drawPolyLine(fcn::PointVector const & points, uint32_t width)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        std::vector<Point> transformed;
        transformed.reserve(points.size());
        std::ranges::transform(points, std::back_inserter(transformed), [&top](fcn::Point const & point) {
            return Point(point.x + top.xOffset, point.y + top.yOffset);
        });
        m_renderbackend->drawPolyLine(
            transformed, static_cast<uint8_t>(width), m_color.r, m_color.g, m_color.b, effectiveAlpha());
    }

    void GLCoreGuiGraphics::drawBezier(fcn::PointVector const & points, int32_t steps, uint32_t width)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        std::vector<Point> transformed;
        transformed.reserve(points.size());
        std::ranges::transform(points, std::back_inserter(transformed), [&top](fcn::Point const & point) {
            return Point(point.x + top.xOffset, point.y + top.yOffset);
        });
        m_renderbackend->drawBezier(
            transformed, steps, static_cast<uint8_t>(width), m_color.r, m_color.g, m_color.b, effectiveAlpha());
    }

    void GLCoreGuiGraphics::drawRectangle(fcn::Rectangle const & rectangle)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawRectangle(
            Point(rectangle.x + top.xOffset, rectangle.y + top.yOffset),
            static_cast<uint16_t>(rectangle.width),
            static_cast<uint16_t>(rectangle.height),
            m_color.r,
            m_color.g,
            m_color.b,
            effectiveAlpha());
    }

    void GLCoreGuiGraphics::fillRectangle(fcn::Rectangle const & rectangle)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->fillRectangle(
            Point(rectangle.x + top.xOffset, rectangle.y + top.yOffset),
            static_cast<uint16_t>(rectangle.width),
            static_cast<uint16_t>(rectangle.height),
            m_color.r,
            m_color.g,
            m_color.b,
            effectiveAlpha());
    }

    void GLCoreGuiGraphics::drawCircle(fcn::Point const & p, uint32_t radius)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawCircle(
            Point(p.x + top.xOffset, p.y + top.yOffset), radius, m_color.r, m_color.g, m_color.b, effectiveAlpha());
    }

    void GLCoreGuiGraphics::drawFillCircle(fcn::Point const & p, uint32_t radius)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawFillCircle(
            Point(p.x + top.xOffset, p.y + top.yOffset), radius, m_color.r, m_color.g, m_color.b, effectiveAlpha());
    }

    void GLCoreGuiGraphics::drawCircleSegment(fcn::Point const & p, uint32_t radius, int32_t sangle, int32_t eangle)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawCircleSegment(
            Point(p.x + top.xOffset, p.y + top.yOffset),
            radius,
            sangle,
            eangle,
            m_color.r,
            m_color.g,
            m_color.b,
            effectiveAlpha());
    }

    void GLCoreGuiGraphics::drawFillCircleSegment(fcn::Point const & p, uint32_t radius, int32_t sangle, int32_t eangle)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawFillCircleSegment(
            Point(p.x + top.xOffset, p.y + top.yOffset),
            radius,
            sangle,
            eangle,
            m_color.r,
            m_color.g,
            m_color.b,
            effectiveAlpha());
    }

    void GLCoreGuiGraphics::fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3)
    {
        if (m_renderbackend == nullptr || mClipStack.empty()) {
            return;
        }
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->fillTriangle(
            Point(x1 + top.xOffset, y1 + top.yOffset),
            Point(x2 + top.xOffset, y2 + top.yOffset),
            Point(x3 + top.xOffset, y3 + top.yOffset),
            m_color.r,
            m_color.g,
            m_color.b,
            effectiveAlpha());
    }

    void GLCoreGuiGraphics::_beginDraw()
    {
        pushClipArea(fcn::Rectangle(0, 0, m_width, m_height));
    }

    void GLCoreGuiGraphics::_endDraw()
    {
        popClipArea();
    }

    bool GLCoreGuiGraphics::pushClipArea(fcn::Rectangle area)
    {
        bool const result = fcn::Graphics::pushClipArea(area);
        if (m_renderbackend != nullptr && !mClipStack.empty()) {
            fcn::ClipRectangle const & top = mClipStack.top();
            m_renderbackend->pushClipArea(Rect(top.x, top.y, top.width, top.height), false);
        }
        return result;
    }

    void GLCoreGuiGraphics::popClipArea()
    {
        fcn::Graphics::popClipArea();
        if (m_renderbackend != nullptr) {
            m_renderbackend->popClipArea();
        }
    }

    void GLCoreGuiGraphics::setColor(fcn::Color const & color)
    {
        m_color = color;
    }

    fcn::Color const & GLCoreGuiGraphics::getColor() const
    {
        return m_color;
    }
} // namespace FIFE
