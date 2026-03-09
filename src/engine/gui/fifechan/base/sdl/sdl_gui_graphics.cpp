// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes
#include <SDL.h>
#include <fifechan/font.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src dir
#include "gui/fifechan/base/gui_image.h"
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "sdl_gui_graphics.h"

namespace FIFE
{
    static Logger _log(LM_GUI);

    SdlGuiGraphics::SdlGuiGraphics()
    {
        m_renderbackend = RenderBackend::instance();
        setTarget(m_renderbackend->getScreenSurface());
    }

    void SdlGuiGraphics::updateTarget()
    {
        setTarget(m_renderbackend->getScreenSurface());
    }

    void SdlGuiGraphics::drawImage(
        const fcn::Image* image, int32_t srcX, int32_t srcY, int32_t dstX, int32_t dstY, int32_t width, int32_t height)
    {
        const auto* g_img = dynamic_cast<const GuiImage*>(image);
        assert(g_img);

        ImagePtr fifeimg               = g_img->getFIFEImage();
        const fcn::ClipRectangle& clip = getCurrentClipArea();
        Rect rect(dstX, dstY, width, height);
        rect.x += clip.xOffset;
        rect.y += clip.yOffset;

        fifeimg->render(rect);
    }

    void SdlGuiGraphics::drawText(const std::string& text, int32_t x, int32_t y, uint32_t alignment)
    {
        if (mFont == nullptr) {
            throw GuiException("SdlGuiGraphics::drawText() - No font set!");
        }

        const auto align = static_cast<fcn::Graphics::Alignment>(alignment);

        switch (align) {
        case fcn::Graphics::Alignment::Left:
            mFont->drawString(this, text, x, y);
            break;
        case fcn::Graphics::Alignment::Center:
            mFont->drawString(this, text, x - (mFont->getWidth(text) / 2), y);
            break;
        case fcn::Graphics::Alignment::Right:
            mFont->drawString(this, text, x - mFont->getWidth(text), y);
            break;
        default:
            FL_WARN(_log, LMsg("SdlGuiGraphics::drawText() - ") << "Unknown alignment: " << alignment);
            mFont->drawString(this, text, x, y);
        }
    }

    void SdlGuiGraphics::drawPoint(int32_t x, int32_t y)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        m_renderbackend->putPixel(x + top.xOffset, y + top.yOffset, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void SdlGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        m_renderbackend->drawLine(
            Point(x1 + top.xOffset, y1 + top.yOffset),
            Point(x2 + top.xOffset, y2 + top.yOffset),
            mColor.r,
            mColor.g,
            mColor.b,
            mColor.a);
    }

    void SdlGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t width)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        m_renderbackend->drawThickLine(
            Point(x1 + top.xOffset, y1 + top.yOffset),
            Point(x2 + top.xOffset, y2 + top.yOffset),
            width,
            mColor.r,
            mColor.g,
            mColor.b,
            mColor.a);
    }

    void SdlGuiGraphics::drawPolyLine(const fcn::PointVector& points, uint32_t width)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        std::vector<Point> npoints;
        auto it = points.begin();
        for (; it != points.end(); ++it) {
            npoints.emplace_back((*it).x + top.xOffset, (*it).y + top.yOffset);
        }
        m_renderbackend->drawPolyLine(npoints, width, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void SdlGuiGraphics::drawBezier(const fcn::PointVector& points, int32_t steps, uint32_t width)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        std::vector<Point> npoints;
        auto it = points.begin();
        for (; it != points.end(); ++it) {
            npoints.emplace_back((*it).x + top.xOffset, (*it).y + top.yOffset);
        }
        m_renderbackend->drawBezier(npoints, steps, width, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void SdlGuiGraphics::drawRectangle(const fcn::Rectangle& rectangle)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        m_renderbackend->drawRectangle(
            Point(rectangle.x + top.xOffset, rectangle.y + top.yOffset),
            rectangle.width,
            rectangle.height,
            mColor.r,
            mColor.g,
            mColor.b,
            mColor.a);
    }

    void SdlGuiGraphics::fillRectangle(const fcn::Rectangle& rectangle)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        m_renderbackend->fillRectangle(
            Point(rectangle.x + top.xOffset, rectangle.y + top.yOffset),
            rectangle.width,
            rectangle.height,
            mColor.r,
            mColor.g,
            mColor.b,
            mColor.a);
    }

    void SdlGuiGraphics::drawCircle(const fcn::Point& p, uint32_t radius)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        m_renderbackend->drawCircle(
            Point(p.x + top.xOffset, p.y + top.yOffset), radius, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void SdlGuiGraphics::drawFillCircle(const fcn::Point& p, uint32_t radius)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        m_renderbackend->drawFillCircle(
            Point(p.x + top.xOffset, p.y + top.yOffset), radius, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void SdlGuiGraphics::drawCircleSegment(const fcn::Point& p, uint32_t radius, int32_t sangle, int32_t eangle)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        m_renderbackend->drawCircleSegment(
            Point(p.x + top.xOffset, p.y + top.yOffset),
            radius,
            sangle,
            eangle,
            mColor.r,
            mColor.g,
            mColor.b,
            mColor.a);
    }

    void SdlGuiGraphics::drawFillCircleSegment(const fcn::Point& p, uint32_t radius, int32_t sangle, int32_t eangle)
    {
        const fcn::ClipRectangle& top = mClipStack.top();
        m_renderbackend->drawFillCircleSegment(
            Point(p.x + top.xOffset, p.y + top.yOffset),
            radius,
            sangle,
            eangle,
            mColor.r,
            mColor.g,
            mColor.b,
            mColor.a);
    }

    void SdlGuiGraphics::_beginDraw()
    {
        fcn::Rectangle area(0, 0, mTarget->w, mTarget->h);
        fcn::Graphics::pushClipArea(area);
        m_renderbackend->pushClipArea(Rect(0, 0, mTarget->w, mTarget->h), false);
    }

    void SdlGuiGraphics::_endDraw()
    {
        // Cleanup
        fcn::Graphics::popClipArea();
        m_renderbackend->popClipArea();
    }

    bool SdlGuiGraphics::pushClipArea(fcn::Rectangle area)
    {
        fcn::Graphics::pushClipArea(area);

        // Due to some odd conception in Fifechan some of area
        // has xOffset and yOffset > 0. And if it happens we
        // need to offset our clip area. Or we can use Fifechan stack.
        const fcn::ClipRectangle& top = mClipStack.top();

        m_renderbackend->pushClipArea(Rect(top.x, top.y, top.width, top.height), false);

        return true;
    }

    void SdlGuiGraphics::popClipArea()
    {
        fcn::Graphics::popClipArea();
        m_renderbackend->popClipArea();
    }

    void SdlGuiGraphics::setColor(const fcn::Color& color)
    {
        mColor = color;
    }
} // namespace FIFE
