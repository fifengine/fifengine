// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "opengl_gui_graphics.h"

// Standard C++ library includes
#include <string>
#include <vector>

// GLEW must be included first before any OpenGL headers
#include <fifechan/backends/opengl/graphics.hpp>
#include <fifechan/font.hpp>

#include "video/opengl/fife_opengl.h"

// FIFE includes
#include "gui/fifechan/base/gui_image.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"

namespace FIFE
{
    static Logger _log(LM_GUI);

    OpenGLGuiGraphics::OpenGLGuiGraphics() :
        m_renderbackend(dynamic_cast<RenderBackendOpenGL*>(RenderBackend::instance()))
    {
        mColor = fcn::Color(255, 255, 255, 255);
        setTargetPlane(m_renderbackend->getWidth(), m_renderbackend->getHeight());
    }

    void OpenGLGuiGraphics::updateTarget()
    {
        setTargetPlane(m_renderbackend->getWidth(), m_renderbackend->getHeight());
    }

    void OpenGLGuiGraphics::drawImage(
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
        fcn::ClipRectangle const & clip = mClipStack.top();
        fifeimg->render(Rect(dstX + clip.xOffset, dstY + clip.yOffset, width, height));
    }

    void OpenGLGuiGraphics::drawText(std::string const & text, int32_t x, int32_t y, fcn::Graphics::Alignment alignment)
    {
        if (mFont == nullptr) {
            throw GuiException("OpenGLGuiGraphics::drawText() - No font set!");
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
                _log, LMsg("OpenGLGuiGraphics::drawText() - ") << "Unknown alignment: " << static_cast<int>(alignment));
            mFont->drawString(this, text, x, y);
        }
    }

    void OpenGLGuiGraphics::drawPoint(int32_t x, int32_t y)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->putPixel(x + top.xOffset, y + top.yOffset, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void OpenGLGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        x1 += top.xOffset;
        x2 += top.xOffset;
        y1 += top.yOffset;
        y2 += top.yOffset;

        /*Point pbegin(static_cast<int32_t>(ceil(x1 + 0.375f)), static_cast<int32_t>(ceil(y1 + 0.375f)));
        Point pend(static_cast<int32_t>(ceil(x2 + 0.625f)), static_cast<int32_t>(ceil(y2 + 0.625f)));*/
        /*if (x1 == x2) {
                y2 += 1;
        } else if (y1 == y2) {
                x2 += 1;
        }*/
        Point const pbegin(x1, y1);
        Point const pend(x2, y2);

        m_renderbackend->drawLine(pbegin, pend, mColor.r, mColor.g, mColor.b, mColor.a);
        // m_renderbackend->putPixel(pbegin.x, pbegin.y,
        //	mColor.r, mColor.g, mColor.b, mColor.a);
        m_renderbackend->putPixel(pend.x, pend.y, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void OpenGLGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t width)
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

    void OpenGLGuiGraphics::drawPolyLine(fcn::PointVector const & points, uint32_t width)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        std::vector<Point> npoints;
        auto it = points.begin();
        for (; it != points.end(); ++it) {
            npoints.emplace_back((*it).x + top.xOffset, (*it).y + top.yOffset);
        }
        m_renderbackend->drawPolyLine(npoints, width, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void OpenGLGuiGraphics::drawBezier(fcn::PointVector const & points, int32_t steps, uint32_t width)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        std::vector<Point> npoints;
        auto it = points.begin();
        for (; it != points.end(); ++it) {
            npoints.emplace_back((*it).x + top.xOffset, (*it).y + top.yOffset);
        }
        m_renderbackend->drawBezier(npoints, steps, width, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void OpenGLGuiGraphics::drawRectangle(fcn::Rectangle const & rectangle)
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

    void OpenGLGuiGraphics::fillRectangle(fcn::Rectangle const & rectangle)
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

    void OpenGLGuiGraphics::drawCircle(fcn::Point const & p, uint32_t radius)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawCircle(
            Point(p.x + top.xOffset, p.y + top.yOffset), radius, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void OpenGLGuiGraphics::drawFillCircle(fcn::Point const & p, uint32_t radius)
    {
        fcn::ClipRectangle const & top = mClipStack.top();
        m_renderbackend->drawFillCircle(
            Point(p.x + top.xOffset, p.y + top.yOffset), radius, mColor.r, mColor.g, mColor.b, mColor.a);
    }

    void OpenGLGuiGraphics::drawCircleSegment(fcn::Point const & p, uint32_t radius, int32_t sangle, int32_t eangle)
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

    void OpenGLGuiGraphics::drawFillCircleSegment(fcn::Point const & p, uint32_t radius, int32_t sangle, int32_t eangle)
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

    void OpenGLGuiGraphics::_beginDraw()
    {
        fcn::Rectangle const area(0, 0, mWidth, mHeight);
        pushClipArea(area);
        m_renderbackend->pushClipArea(Rect(0, 0, mWidth, mHeight), false);
    }

    void OpenGLGuiGraphics::_endDraw()
    {
        m_renderbackend->renderVertexArrays();

        // Cleanup
        popClipArea();
        m_renderbackend->popClipArea();
    }

    bool OpenGLGuiGraphics::pushClipArea(fcn::Rectangle area)
    {
        // Render what we gathered so far
        m_renderbackend->renderVertexArrays();

        // Use the generic fcn clip-stack logic. The fcn::opengl::Graphics
        // popClipArea implementation in current dependencies is recursive.
        fcn::Graphics::pushClipArea(area);

        // Due to some odd conception in guiChan some of area
        // has xOffset and yOffset > 0. And if it happens we
        // need to offset our clip area. Or we can use Fifechan stack.
        fcn::ClipRectangle const & top = mClipStack.top();

        m_renderbackend->pushClipArea(Rect(top.x, top.y, top.width, top.height), false);

        return true;
    }

    void OpenGLGuiGraphics::popClipArea()
    {
        // Render what we gathered so far
        m_renderbackend->renderVertexArrays();

        // Use the generic fcn clip-stack logic. The fcn::opengl::Graphics
        // popClipArea implementation in current dependencies is recursive.
        fcn::Graphics::popClipArea();

        m_renderbackend->popClipArea();
    }

    void OpenGLGuiGraphics::setColor(fcn::Color const & color)
    {
        mColor = color;
    }
} // namespace FIFE
