// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "sdl_gui_graphics.h"

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes
#include <SDL3/SDL.h>

#include <fifechan/font.hpp>

// FIFE includes
#include "gui/fifechan/base/gui_image.h"
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/renderbackend.h"
#include "video/sdl/renderbackendsdl.h"

namespace FIFE
{
    static Logger _log(LM_GUI);

    SdlGuiGraphics::SdlGuiGraphics() : m_renderbackend(RenderBackend::instance())
    {
        // New fifechan SDL backend expects an SDL_Renderer and dimensions
        auto* rb_sdl = dynamic_cast<RenderBackendSDL*>(m_renderbackend);
        setTarget(rb_sdl->getRenderer(), m_renderbackend->getWidth(), m_renderbackend->getHeight());
    }

    void SdlGuiGraphics::updateTarget()
    {
        auto* rb_sdl = dynamic_cast<RenderBackendSDL*>(m_renderbackend);
        setTarget(rb_sdl->getRenderer(), m_renderbackend->getWidth(), m_renderbackend->getHeight());
    }

    void SdlGuiGraphics::drawImage(
        fcn::Image const * image,
        [[maybe_unused]] int32_t srcX,
        [[maybe_unused]] int32_t srcY,
        int32_t dstX,
        int32_t dstY,
        int32_t width,
        int32_t height)
    {
        auto const * g_img = dynamic_cast<GuiImage const *>(image);
        assert(g_img);

        ImagePtr const fifeimg          = g_img->getFIFEImage();
        fcn::ClipRectangle const & clip = getCurrentClipArea();
        Rect rect(dstX, dstY, width, height);
        rect.x += clip.xOffset;
        rect.y += clip.yOffset;

        fifeimg->render(rect);
    }

    void SdlGuiGraphics::drawText(std::string const & text, int32_t x, int32_t y, fcn::Graphics::Alignment alignment)
    {
        if (mFont == nullptr) {
            throw GuiException("SdlGuiGraphics::drawText() - No font set!");
        }

        switch (alignment) {
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
            FL_WARN(
                _log, LMsg("SdlGuiGraphics::drawText() - ") << "Unknown alignment: " << static_cast<int>(alignment));
            mFont->drawString(this, text, x, y);
        }
    }

    void SdlGuiGraphics::drawPoint(int32_t x, int32_t y)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->putPixel(x + top.xOffset, y + top.yOffset, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void SdlGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
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
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawThickLine(
            Point(x1 + top.xOffset, y1 + top.yOffset),
            Point(x2 + top.xOffset, y2 + top.yOffset),
            width,
            mColor.r,
            mColor.g,
            mColor.b,
            mColor.a);
    }

    void SdlGuiGraphics::drawPolyLine(fcn::PointVector const & points, uint32_t width)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        std::vector<Point> npoints;
        auto it = points.begin();
        for (; it != points.end(); ++it) {
            npoints.emplace_back((*it).x + top.xOffset, (*it).y + top.yOffset);
        }
        m_renderbackend->drawPolyLine(npoints, width, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void SdlGuiGraphics::drawBezier(fcn::PointVector const & points, int32_t steps, uint32_t width)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        std::vector<Point> npoints;
        auto it = points.begin();
        for (; it != points.end(); ++it) {
            npoints.emplace_back((*it).x + top.xOffset, (*it).y + top.yOffset);
        }
        m_renderbackend->drawBezier(npoints, steps, width, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void SdlGuiGraphics::drawRectangle(fcn::Rectangle const & rectangle)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawRectangle(
            Point(rectangle.x + top.xOffset, rectangle.y + top.yOffset),
            rectangle.width,
            rectangle.height,
            mColor.r,
            mColor.g,
            mColor.b,
            mColor.a);
    }

    void SdlGuiGraphics::fillRectangle(fcn::Rectangle const & rectangle)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->fillRectangle(
            Point(rectangle.x + top.xOffset, rectangle.y + top.yOffset),
            rectangle.width,
            rectangle.height,
            mColor.r,
            mColor.g,
            mColor.b,
            mColor.a);
    }

    void SdlGuiGraphics::drawCircle(fcn::Point const & p, uint32_t radius)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawCircle(
            Point(p.x + top.xOffset, p.y + top.yOffset), radius, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void SdlGuiGraphics::drawFillCircle(fcn::Point const & p, uint32_t radius)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawFillCircle(
            Point(p.x + top.xOffset, p.y + top.yOffset), radius, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void SdlGuiGraphics::drawCircleSegment(fcn::Point const & p, uint32_t radius, int32_t sangle, int32_t eangle)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
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

    void SdlGuiGraphics::drawFillCircleSegment(fcn::Point const & p, uint32_t radius, int32_t sangle, int32_t eangle)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
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
        fcn::Rectangle const area(
            0, 0, static_cast<int>(m_renderbackend->getWidth()), static_cast<int>(m_renderbackend->getHeight()));
        pushClipArea(area);
        m_renderbackend->pushClipArea(
            Rect(0, 0, static_cast<int>(m_renderbackend->getWidth()), static_cast<int>(m_renderbackend->getHeight())),
            false);
    }

    void SdlGuiGraphics::_endDraw()
    {
        // Cleanup
        popClipArea();
        m_renderbackend->popClipArea();
    }

    bool SdlGuiGraphics::pushClipArea(fcn::Rectangle area)
    {
        fcn::sdl3::Graphics::pushClipArea(area);

        // Due to some odd conception in Fifechan some of area
        // has xOffset and yOffset > 0. And if it happens we
        // need to offset our clip area. Or we can use Fifechan stack.
        fcn::ClipRectangle const & top = mClipStack.top();

        m_renderbackend->pushClipArea(Rect(top.x, top.y, top.width, top.height), false);

        return true;
    }

    void SdlGuiGraphics::popClipArea()
    {
        fcn::sdl3::Graphics::popClipArea();
        m_renderbackend->popClipArea();
    }

    void SdlGuiGraphics::setColor(fcn::Color const & color)
    {
        mColor = color;
    }
} // namespace FIFE
