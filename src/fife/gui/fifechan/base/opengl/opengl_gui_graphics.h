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
    class FIFE_API OpenGLGuiGraphics : public fcn::opengl::Graphics
    {
        public:
            /** Constructor
             */
            OpenGLGuiGraphics();

            OpenGLGuiGraphics(OpenGLGuiGraphics const &)            = delete;
            OpenGLGuiGraphics& operator=(OpenGLGuiGraphics const &) = delete;

            void updateTarget();
            void drawSurface(SDL_Surface* surface, int dstX, int dstY) override;
            void drawImage(
                fcn::Image const * image,
                int32_t srcX,
                int32_t srcY,
                int32_t dstX,
                int32_t dstY,
                int32_t width,
                int32_t height) override;

            void drawText(std::string const & text, int32_t x, int32_t y, fcn::Graphics::Alignment alignment) override;
            void drawPoint(int32_t x, int32_t y) override;
            void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2) override;
            void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t width) override;
            void drawPolyLine(fcn::PointVector const & points, uint32_t width) override;
            void drawBezier(fcn::PointVector const & points, int32_t steps, uint32_t width) override;
            void drawRectangle(fcn::Rectangle const & rectangle) override;
            void fillRectangle(fcn::Rectangle const & rectangle) override;
            void drawCircle(fcn::Point const & p, uint32_t radius) override;
            void drawFillCircle(fcn::Point const & p, uint32_t radius) override;
            void drawCircleSegment(fcn::Point const & p, uint32_t radius, int32_t sangle, int32_t eangle) override;
            void drawFillCircleSegment(fcn::Point const & p, uint32_t radius, int32_t sangle, int32_t eangle) override;

            void _beginDraw() override;
            void _endDraw() override;

            bool pushClipArea(fcn::Rectangle area) override;
            void popClipArea() override;

            void setColor(fcn::Color const & color) override;

        private:
            RenderBackendOpenGL* m_renderbackend;
    };
} // namespace FIFE

#endif
