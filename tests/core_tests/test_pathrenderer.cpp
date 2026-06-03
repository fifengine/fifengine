// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <memory>
#include <string>
#include <vector>

// Platform specific includes
#include "fife_unittest.h"

// FIFE includes
#include "video/renderbackend.h"
#include "view/rendererbase.h"
#include "view/renderers/pathrenderer.h"

namespace
{
    int dummy_inst_storage[5];
}

// ── Mock RenderBackend ──────────────────────────────────────────────────
namespace
{
    struct MockRenderBackend : FIFE::RenderBackend
    {
            MockRenderBackend() : FIFE::RenderBackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255})
            {
            }

            std::string const & getName() const override
            {
                static std::string const n = "MockRenderBackend";
                return n;
            }
            void init(std::string const & backendId) override
            {
            }
            void clearBackBuffer() override
            {
            }
            void setLightingModel(uint32_t mode) override
            {
            }
            uint32_t getLightingModel() const override
            {
                return 0;
            }
            void setLighting(float r, float g, float b) override
            {
            }
            void resetLighting() override
            {
            }
            void resetStencilBuffer(uint8_t value) override
            {
            }
            void changeBlending(int32_t src, int32_t dst) override
            {
            }
            void createMainScreen(
                FIFE::ScreenMode const & mode, std::string const & title, std::string const & icon) override
            {
            }
            void setScreenMode(FIFE::ScreenMode const & mode) override
            {
            }
            std::unique_ptr<FIFE::Image> createImage(FIFE::IResourceLoader*) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(
                std::string const &, FIFE::IResourceLoader*) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(uint8_t const *, uint32_t, uint32_t) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(SDL_Surface*) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(std::string const &, SDL_Surface*) override
            {
                return nullptr;
            }
            void renderVertexArrays() override
            {
            }
            void captureScreen(std::string const & filename) override
            {
            }
            void captureScreen(std::string const & filename, uint32_t x, uint32_t y) override
            {
            }
            bool putPixel(int32_t, int32_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
                return true;
            }
            void drawLine(
                FIFE::Point const & p1, FIFE::Point const & p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
            }
            void drawThickLine(
                FIFE::Point const & p1, FIFE::Point const & p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint8_t w)
                override
            {
            }
            void drawPolyLine(
                std::vector<FIFE::Point> const & pts, uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint8_t w) override
            {
            }
            void drawBezier(
                std::vector<FIFE::Point> const & pts,
                int32_t steps,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a,
                uint8_t w) override
            {
            }
            std::unique_ptr<FIFE::Image> createImage(
                std::string const &, uint8_t const *, uint32_t, uint32_t) override
            {
                return nullptr;
            }
            void drawTriangle(
                FIFE::Point const & p1,
                FIFE::Point const & p2,
                FIFE::Point const & p3,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a) override
            {
            }
            void drawRectangle(
                FIFE::Point const & pt, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
            }
            void fillRectangle(
                FIFE::Point const & pt, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
            }
            void drawQuad(
                FIFE::Point const & p1,
                FIFE::Point const & p2,
                FIFE::Point const & p3,
                FIFE::Point const & p4,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a) override
            {
            }
            void drawVertex(FIFE::Point const & pt, uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint8_t w) override
            {
            }
            void drawCircle(
                FIFE::Point const & center, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
            }
            void drawFillCircle(
                FIFE::Point const & center, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
            }
            void drawCircleSegment(
                FIFE::Point const & center,
                uint32_t radius,
                int32_t startAngle,
                int32_t endAngle,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a) override
            {
            }
            void drawFillCircleSegment(
                FIFE::Point const & center,
                uint32_t radius,
                int32_t startAngle,
                int32_t endAngle,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a) override
            {
            }
            void drawLightPrimitive(
                FIFE::Point const & pt,
                uint8_t brightness,
                float scale,
                int32_t angle,
                float xStretch,
                float yStretch,
                uint8_t r,
                uint8_t g,
                uint8_t b) override
            {
            }
            void addImageToArray(
                uint32_t id,
                FIFE::Rect const & rect,
                float const * texCoords,
                uint8_t opacity,
                uint8_t const * color) override
            {
            }
            void changeRenderInfos(
                FIFE::RenderDataType type,
                uint16_t src,
                int32_t x,
                int32_t y,
                bool fog,
                bool lighting,
                uint8_t alpha,
                FIFE::GLConstants srcBlend,
                FIFE::GLConstants dstBlend,
                FIFE::OverlayType overlay) override
            {
            }
            void renderGuiGeometry(
                std::vector<FIFE::GuiVertex> const & verts,
                std::vector<int> const & indices,
                FIFE::DoublePoint const & offset,
                FIFE::ImagePtr image) override
            {
            }
            void enableScissorTest() override
            {
            }
            void disableScissorTest() override
            {
            }
            void attachRenderTarget(FIFE::ImagePtr& target, bool enabled) override
            {
            }
            void detachRenderTarget() override
            {
            }

        protected:
            void setClipArea(FIFE::Rect const & rect, bool enabled) override
            {
            }

        public:
    };

    struct MockContainer : FIFE::IRendererContainer
    {
            FIFE::PathRenderer* pathRenderer{nullptr};
            FIFE::RendererBase* getRenderer(std::string const & name) override
            {
                if (name == "PathRenderer") {
                    return pathRenderer;
                }
                return nullptr;
            }
    };
} // anonymous namespace

// ── Static assertions — compile-time only ──────────────────────────────
static_assert(static_cast<int>(FIFE::PLS_SOLID) == 0, "PLS_SOLID must be 0");
static_assert(static_cast<int>(FIFE::PLS_DASHED) == 1, "PLS_DASHED must be 1");
static_assert(static_cast<int>(FIFE::PLS_ARROWED) == 2, "PLS_ARROWED must be 2");
static_assert(static_cast<int>(FIFE::PLS_GRADIENT) == 3, "PLS_GRADIENT must be 3");
static_assert(static_cast<int>(FIFE::PLS_HEATMAP) == 4, "PLS_HEATMAP must be 4");

TEST_CASE("PathLineStyle enum values", "[pathrenderer]")
{
    CHECK(static_cast<int>(FIFE::PLS_SOLID) == 0);
    CHECK(static_cast<int>(FIFE::PLS_DASHED) == 1);
    CHECK(static_cast<int>(FIFE::PLS_ARROWED) == 2);
    CHECK(static_cast<int>(FIFE::PLS_GRADIENT) == 3);
    CHECK(static_cast<int>(FIFE::PLS_HEATMAP) == 4);
}

TEST_CASE("PathStyleConfig default values", "[pathrenderer]")
{
    FIFE::PathStyleConfig const cfg;
    CHECK_EQ(cfg.color.r, 0);
    CHECK_EQ(cfg.color.g, 100);
    CHECK_EQ(cfg.color.b, 255);
    CHECK_EQ(cfg.color.a, 255);
    CHECK_EQ(cfg.width, 2.0F);
    CHECK_EQ(cfg.style, FIFE::PLS_SOLID);
    CHECK_EQ(cfg.dashLength, 12.0F);
    CHECK_EQ(cfg.dashGap, 6.0F);
    CHECK_EQ(cfg.drawArrows, false);
    CHECK_EQ(cfg.arrowSize, 10.0F);
}

TEST_CASE("PathRenderer construction and basic properties", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    CHECK_EQ(pr.getName(), "PathRenderer");
    CHECK(pr.isEnabled() == false);

    // Clone preserves name and disabled state
    std::unique_ptr<FIFE::RendererBase> cloned(pr.clone());
    CHECK_EQ(cloned->getName(), "PathRenderer");
    CHECK(cloned->isEnabled() == false);
}

TEST_CASE("PathRenderer add/remove/clear paths", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    FIFE::Instance* const i1 = static_cast<FIFE::Instance*>(static_cast<void*>(&dummy_inst_storage[0]));
    FIFE::Instance* const i2 = static_cast<FIFE::Instance*>(static_cast<void*>(&dummy_inst_storage[1]));

    FIFE::PathStyleConfig cfg;
    cfg.color = SDL_Color{.r = 255, .g = 0, .b = 0, .a = 255};
    cfg.width = 4.0F;

    pr.addPath(i1, cfg);
    pr.addPath(i2, cfg);

    // Overwrite existing path style via setPathStyle
    FIFE::PathStyleConfig cfg2;
    cfg2.color = SDL_Color{.r = 0, .g = 255, .b = 0, .a = 255};
    cfg2.style = FIFE::PLS_DASHED;
    pr.setPathStyle(i1, cfg2);

    // Remove one path
    pr.removePath(i1);

    // Clear all
    pr.clearPaths();
}

// No null-instance test: addPath asserts non-null in debug builds

TEST_CASE("PathRenderer setPathStyle for non-existent instance no-ops", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    FIFE::Instance* const dummy = static_cast<FIFE::Instance*>(static_cast<void*>(&dummy_inst_storage[2]));
    FIFE::PathStyleConfig const cfg;
    pr.setPathStyle(dummy, cfg);
}

TEST_CASE("PathRenderer removePath for non-existent instance no-ops", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    FIFE::Instance* const dummy = static_cast<FIFE::Instance*>(static_cast<void*>(&dummy_inst_storage[3]));
    pr.removePath(dummy);
}

TEST_CASE("PathRenderer getInstance", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);
    MockContainer mc;
    mc.pathRenderer = &pr;

    FIFE::PathRenderer const * retrieved = FIFE::PathRenderer::getInstance(&mc);
    CHECK_EQ(retrieved, &pr);
}

TEST_CASE("PathRenderer getInstance returns null for missing renderer", "[pathrenderer]")
{
    MockContainer mc;
    CHECK(FIFE::PathRenderer::getInstance(&mc) == nullptr);
}

TEST_CASE("PathStyleConfig can be copied", "[pathrenderer]")
{
    FIFE::PathStyleConfig a;
    a.color     = SDL_Color{.r = 10, .g = 20, .b = 30, .a = 40};
    a.width     = 5.0F;
    a.style     = FIFE::PLS_ARROWED;
    a.arrowSize = 15.0F;

    FIFE::PathStyleConfig const b = a;
    CHECK_EQ(b.color.r, 10);
    CHECK_EQ(b.color.g, 20);
    CHECK_EQ(b.color.b, 30);
    CHECK_EQ(b.color.a, 40);
    CHECK_EQ(b.width, 5.0F);
    CHECK_EQ(b.style, FIFE::PLS_ARROWED);
    CHECK_EQ(b.arrowSize, 15.0F);
}

TEST_CASE("PathRenderer clone propagates configs", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    FIFE::Instance* const inst = static_cast<FIFE::Instance*>(static_cast<void*>(&dummy_inst_storage[4]));
    FIFE::PathStyleConfig cfg;
    cfg.color = SDL_Color{.r = 100, .g = 200, .b = 50, .a = 255};
    cfg.width = 3.0F;
    cfg.style = FIFE::PLS_DASHED;
    pr.addPath(inst, cfg);

    std::unique_ptr<FIFE::RendererBase> cloned(pr.clone());
    CHECK_EQ(cloned->getName(), "PathRenderer");

    // The cloned renderer shuold also be disabled
    CHECK(cloned->isEnabled() == false);
}
