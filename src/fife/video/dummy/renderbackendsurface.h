// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_DUMMY_RENDERBACKENDSURFACE_H
#define FIFE_VIDEO_DUMMY_RENDERBACKENDSURFACE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <memory>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "video/dummy/renderbackenddummy.h"

namespace FIFE
{

    /** Deterministic, windowless render backend backed by a software renderer.
     *
     * It creates an offscreen RGBA8888 SDL_Surface and an SDL_Renderer with
     * SDL_CreateSoftwareRenderer. No display, GL context or window is needed,
     * and the SDL video subsystem is not initialized by this backend (the
     * software renderer works without it in the pinned SDL3 version).
     *
     * All output is committed to the surface by endFrame(), which flushes the
     * renderer but never presents anything.
     *
     * @see RenderBackendDummy
     */
    class FIFE_API RenderBackendSurface : public RenderBackendDummy
    {
        public:
            explicit RenderBackendSurface(SDL_Color const & colorkey, uint32_t width = 800, uint32_t height = 600);
            ~RenderBackendSurface() override;

            RenderBackendSurface(RenderBackendSurface const &)            = delete;
            RenderBackendSurface& operator=(RenderBackendSurface const &) = delete;

            std::string const & getName() const override;

            void createMainScreen(std::string const & title, std::string const & icon) override;
            void endFrame() override;

            std::unique_ptr<Image> createImage(IResourceLoader* loader = nullptr) override;
            std::unique_ptr<Image> createImage(std::string const & name, IResourceLoader* loader = nullptr) override;
            std::unique_ptr<Image> createImage(uint8_t const * data, uint32_t width, uint32_t height) override;
            std::unique_ptr<Image> createImage(
                std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) override;
            std::unique_ptr<Image> createImage(SDL_Surface* surface) override;
            std::unique_ptr<Image> createImage(std::string const & name, SDL_Surface* surface) override;

            void drawImageToTarget(Image const & image, Rect const & rect, uint8_t alpha, uint8_t const * rgb) override;

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
            void fillTriangle(
                Point const & p1, Point const & p2, Point const & p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
                override;
            void drawRectangle(
                Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            void fillRectangle(
                Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
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

            void attachRenderTarget(ImagePtr& img, bool discard) override;
            void detachRenderTarget() override;

        protected:
            void setClipArea(Rect const & cliparea, bool clear) override;

        private:
            /** Read the current renderer target into a new surface. */
            SDL_Surface* readTarget() const;

            SDL_Renderer* m_renderer;
            SDL_Texture* m_targetTexture;
    };

} // namespace FIFE

#endif
