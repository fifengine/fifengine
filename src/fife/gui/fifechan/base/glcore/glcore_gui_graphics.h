// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_GLCORE_GUI_GRAPHICS_H
#define FIFE_GUI_GLCORE_GUI_GRAPHICS_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <fifechan/color.hpp>
#include <fifechan/graphics.hpp>

// FIFE includes
#include "util/base/fife_stdint.h"

namespace FIFE
{
    class RenderBackend;

    /** Fifechan graphics adapter for the shader-based OpenGL backends.
     *
     * All drawing is forwarded through the generic RenderBackend singleton, so
     * the adapter works with both OpenGL_CORE and GLES2 without ever
     * casting to a concrete backend.
     */
    class FIFE_API GLCoreGuiGraphics : public fcn::Graphics
    {
        public:
            GLCoreGuiGraphics();

            GLCoreGuiGraphics(GLCoreGuiGraphics const &)            = delete;
            GLCoreGuiGraphics& operator=(GLCoreGuiGraphics const &) = delete;

            /** Refresh the cached backend and its target dimensions.
             *
             * Called by FifechanManager::resizeTopContainer() and safe to call
             * at any time while a RenderBackend singleton exists.
             */
            void updateTarget();

            void drawImage(
                fcn::Image const * image,
                int32_t srcX,
                int32_t srcY,
                int32_t dstX,
                int32_t dstY,
                int32_t width,
                int32_t height) override;

            /** Draws an SDL surface (used by Fifechan's text renderer).
             *
             * The base implementation throws for backends without native
             * surface support, so the dummy adapter blits it through the
             * generic image path instead.
             */
            void drawSurface(SDL_Surface* surface, int dstX, int dstY) override;

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
            void fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3) override;

            void _beginDraw() override;
            void _endDraw() override;

            bool pushClipArea(fcn::Rectangle area) override;
            void popClipArea() override;

            void setColor(fcn::Color const & color) override;
            fcn::Color const & getColor() const override;

        private:
            /** Current colour alpha combined with the Fifechan opacity. */
            uint8_t effectiveAlpha() const;

            RenderBackend* m_renderbackend;
            fcn::Color m_color;
            int32_t m_width;
            int32_t m_height;
    };
} // namespace FIFE

#endif
