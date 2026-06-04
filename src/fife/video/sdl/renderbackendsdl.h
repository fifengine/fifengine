// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_RENDERBACKENDS_SDL_RENDERBACKENDSDL_H
#define FIFE_VIDEO_RENDERBACKENDS_SDL_RENDERBACKENDSDL_H

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

    class ScreenMode;

    /** The main class of the SDL-based renderer.
     *
     * @see RenderBackend
     */
    class FIFE_API RenderBackendSDL : public RenderBackend
    {
        public:
            explicit RenderBackendSDL(SDL_Color const & colorkey);
            virtual ~RenderBackendSDL() override;

            RenderBackendSDL(RenderBackendSDL const &)            = delete;
            RenderBackendSDL& operator=(RenderBackendSDL const &) = delete;

            virtual std::string const & getName() const override;
            virtual void endFrame() override;
            virtual void init(std::string const & driver) override;
            virtual void clearBackBuffer() override;
            virtual void setLightingModel(uint32_t lighting) override;
            virtual uint32_t getLightingModel() const override;
            virtual void setLighting(float red, float green, float blue) override;
            virtual void resetLighting() override;
            virtual void resetStencilBuffer(uint8_t buffer) override;
            virtual void changeBlending(int32_t scr, int32_t dst) override;

            virtual void createMainScreen(
                ScreenMode const & mode, std::string const & title, std::string const & icon) override;
            virtual void setScreenMode(ScreenMode const & mode) override;

            virtual std::unique_ptr<Image> createImage(IResourceLoader* loader = nullptr) override;
            virtual std::unique_ptr<Image> createImage(
                std::string const & name, IResourceLoader* loader = nullptr) override;
            virtual std::unique_ptr<Image> createImage(uint8_t const * data, uint32_t width, uint32_t height) override;
            virtual std::unique_ptr<Image> createImage(
                std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) override;
            virtual std::unique_ptr<Image> createImage(SDL_Surface* surface) override;
            virtual std::unique_ptr<Image> createImage(std::string const & name, SDL_Surface* surface) override;

            virtual void renderVertexArrays() override;
            virtual void addImageToArray(
                uint32_t id, Rect const & rec, float const * st, uint8_t alpha, uint8_t const * rgba) override;
            virtual void changeRenderInfos(
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
            virtual void captureScreen(std::string const & filename) override;
            virtual void captureScreen(std::string const & filename, uint32_t width, uint32_t height) override;

            virtual bool putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            virtual void drawLine(
                Point const & p1, Point const & p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            virtual void drawThickLine(
                Point const & p1, Point const & p2, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
                override;
            virtual void drawPolyLine(
                std::vector<Point> const & points, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
                override;
            virtual void drawBezier(
                std::vector<Point> const & points,
                int32_t steps,
                uint8_t width,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) override;
            virtual void drawTriangle(
                Point const & p1, Point const & p2, Point const & p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
                override;
            virtual void drawRectangle(
                Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            virtual void fillRectangle(
                Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            virtual void drawQuad(
                Point const & p1,
                Point const & p2,
                Point const & p3,
                Point const & p4,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) override;
            virtual void drawVertex(
                Point const & p, uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            virtual void drawCircle(
                Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            virtual void drawFillCircle(
                Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            virtual void drawCircleSegment(
                Point const & p,
                uint32_t radius,
                int32_t sangle,
                int32_t eangle,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) override;
            virtual void drawFillCircleSegment(
                Point const & p,
                uint32_t radius,
                int32_t sangle,
                int32_t eangle,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) override;
            virtual void drawLightPrimitive(
                Point const & p,
                uint8_t intensity,
                float radius,
                int32_t subdivisions,
                float xstretch,
                float ystretch,
                uint8_t red,
                uint8_t green,
                uint8_t blue) override;

            virtual void enableScissorTest() override;
            virtual void disableScissorTest() override;

            virtual void attachRenderTarget(ImagePtr& img, bool discard) override;
            virtual void detachRenderTarget() override;

            virtual void renderGuiGeometry(
                std::vector<GuiVertex> const & vertices,
                std::vector<int> const & indices,
                DoublePoint const & translation,
                ImagePtr texture) override;

            SDL_Renderer* getRenderer()
            {
                return m_renderer;
            }

        protected:
            virtual void setClipArea(Rect const & cliparea, bool clear) override;

            SDL_Renderer* m_renderer;
    };

} // namespace FIFE

#endif
