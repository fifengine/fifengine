// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_OPENGL_GRAPHICS_H
#define FIFE_GUI_OPENGL_GRAPHICS_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// FIFE includes - GLEW must be included before OpenGL headers
#include "video/opengl/fife_opengl.h"

// 3rd party library includes
#include <fifechan/backends/opengl/graphics.hpp>

// FIFE includes
// First block: files included from the FIFE root src dir

namespace FIFE
{
    class RenderBackendOpenGL;

    /** Overrides Fifechan Graphics to enable usage of normal fife images & related facilities
     */
    class /*FIFE_API*/ OpenGLGuiGraphics : public fcn::opengl::Graphics
    {
        public:
            /** Constructor
             */
            OpenGLGuiGraphics();

            OpenGLGuiGraphics(OpenGLGuiGraphics const &)            = delete;
            OpenGLGuiGraphics& operator=(OpenGLGuiGraphics const &) = delete;

            void updateTarget();
            virtual void drawImage(
                fcn::Image const * image,
                int32_t srcX,
                int32_t srcY,
                int32_t dstX,
                int32_t dstY,
                int32_t width,
                int32_t height);

            virtual void drawText(std::string const & text, int32_t x, int32_t y, uint32_t alignment);
            virtual void drawPoint(int32_t x, int32_t y);
            virtual void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
            virtual void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t width);
            virtual void drawPolyLine(fcn::PointVector const & points, uint32_t width);
            virtual void drawBezier(fcn::PointVector const & points, int32_t steps, uint32_t width);
            virtual void drawRectangle(fcn::Rectangle const & rectangle);
            virtual void fillRectangle(fcn::Rectangle const & rectangle);
            virtual void drawCircle(fcn::Point const & p, uint32_t radius);
            virtual void drawFillCircle(fcn::Point const & p, uint32_t radius);
            virtual void drawCircleSegment(fcn::Point const & p, uint32_t radius, int32_t sangle, int32_t eangle);
            virtual void drawFillCircleSegment(fcn::Point const & p, uint32_t radius, int32_t sangle, int32_t eangle);

            virtual void _beginDraw();
            virtual void _endDraw();

            virtual bool pushClipArea(fcn::Rectangle area);
            virtual void popClipArea();

            virtual void setColor(fcn::Color const & color);

        private:
            RenderBackendOpenGL* m_renderbackend;
    };
} // namespace FIFE

#endif
