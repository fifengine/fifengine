// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_DUMMY_RENDERBACKENDDUMMY_H
#define FIFE_VIDEO_DUMMY_RENDERBACKENDDUMMY_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <memory>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "video/renderbackend.h"

namespace FIFE
{

    /** Shared base for the headless render backends.
     *
     * It carries an explicit geometry and provides safe no-op implementations
     * of the RenderBackend interface, so derived backends do not need a window
     * or an SDL surface.
     *
     * @see RenderBackend
     */
    class FIFE_API RenderBackendDummy : public RenderBackend
    {
        public:
            explicit RenderBackendDummy(SDL_Color const & colorkey, uint32_t width = 800, uint32_t height = 600);
            ~RenderBackendDummy() override;

            RenderBackendDummy(RenderBackendDummy const &)            = delete;
            RenderBackendDummy& operator=(RenderBackendDummy const &) = delete;

            std::string const & getName() const override;
            std::string getRenderDescription() const override;
            uint32_t getWidth() const override;
            uint32_t getHeight() const override;
            Rect const & getArea() const override;

            void init(std::string const & driver) override;
            void clearBackBuffer() override;
            void setLightingModel(uint32_t lighting) override;
            uint32_t getLightingModel() const override;
            void setLighting(float red, float green, float blue) override;
            void resetLighting() override;
            void resetStencilBuffer(uint8_t buffer) override;
            void changeBlending(int32_t scr, int32_t dst) override;

            void createMainScreen(std::string const & title, std::string const & icon) override;

            std::unique_ptr<Image> createImage(IResourceLoader* loader = nullptr) override;
            std::unique_ptr<Image> createImage(std::string const & name, IResourceLoader* loader = nullptr) override;
            std::unique_ptr<Image> createImage(uint8_t const * data, uint32_t width, uint32_t height) override;
            std::unique_ptr<Image> createImage(
                std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) override;
            std::unique_ptr<Image> createImage(SDL_Surface* surface) override;
            std::unique_ptr<Image> createImage(std::string const & name, SDL_Surface* surface) override;

            void renderVertexArrays() override;
            void addImageToArray(
                uint32_t id, Rect const & rec, float const * st, uint8_t alpha, uint8_t const * rgba) override;
            void changeRenderInfos(
                RenderDataType type,
                uint16_t elements,
                int32_t src,
                int32_t dst,
                bool light,
                bool stentest,
                uint8_t stenref,
                GLConstants stenop,
                GLConstants stenfunc,
                OverlayType otype = OVERLAY_TYPE_NONE) override;
            void captureScreen(std::string const & filename) override;
            void captureScreen(std::string const & filename, uint32_t width, uint32_t height) override;

            bool putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            void drawLine(
                Point const & p1, Point const & p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            void drawThickLine(
                Point const & p1, Point const & p2, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
                override;
            void drawPolyLine(
                std::vector<Point> const & points, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
                override;
            void drawBezier(
                std::vector<Point> const & points,
                int32_t steps,
                uint8_t width,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) override;
            void drawTriangle(
                Point const & p1, Point const & p2, Point const & p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
                override;
            void fillTriangle(
                Point const & p1, Point const & p2, Point const & p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
                override;
            void drawRectangle(
                Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            void fillRectangle(
                Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            void drawQuad(
                Point const & p1,
                Point const & p2,
                Point const & p3,
                Point const & p4,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) override;
            void drawVertex(Point const & p, uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            void drawCircle(
                Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            void drawFillCircle(
                Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            void drawCircleSegment(
                Point const & p,
                uint32_t radius,
                int32_t sangle,
                int32_t eangle,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) override;
            void drawFillCircleSegment(
                Point const & p,
                uint32_t radius,
                int32_t sangle,
                int32_t eangle,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) override;
            void drawLightPrimitive(
                Point const & p,
                uint8_t intensity,
                float radius,
                int32_t subdivisions,
                float xstretch,
                float ystretch,
                uint8_t red,
                uint8_t green,
                uint8_t blue) override;

            void enableScissorTest() override;
            void disableScissorTest() override;

            void attachRenderTarget(ImagePtr& img, bool discard) override;
            void detachRenderTarget() override;

            void renderGuiGeometry(
                std::vector<GuiVertex> const & vertices,
                std::vector<int> const & indices,
                DoublePoint const & translation,
                ImagePtr texture) override;

        protected:
            void setClipArea(Rect const & cliparea, bool clear) override;

            uint32_t m_width;
            uint32_t m_height;
            Rect m_area;
    };

} // namespace FIFE

#endif
