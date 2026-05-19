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

// ── Mock RenderBackend ──────────────────────────────────────────────────
namespace
{
    struct MockRenderBackend : FIFE::RenderBackend
    {
            MockRenderBackend() : FIFE::RenderBackend(SDL_Color{0, 0, 0, 255})
            {
            }

            std::string const & getName() const override
            {
                static std::string const n = "MockRenderBackend";
                return n;
            }
            void init(std::string const &) override
            {
            }
            void clearBackBuffer() override
            {
            }
            void setLightingModel(uint32_t) override
            {
            }
            uint32_t getLightingModel() const override
            {
                return 0;
            }
            void setLighting(float, float, float) override
            {
            }
            void resetLighting() override
            {
            }
            void resetStencilBuffer(uint8_t) override
            {
            }
            void changeBlending(int32_t, int32_t) override
            {
            }
            void createMainScreen(FIFE::ScreenMode const &, std::string const &, std::string const &) override
            {
            }
            void setScreenMode(FIFE::ScreenMode const &) override
            {
            }
            FIFE::Image* createImage(FIFE::IResourceLoader*) override
            {
                return nullptr;
            }
            FIFE::Image* createImage(std::string const &, FIFE::IResourceLoader*) override
            {
                return nullptr;
            }
            FIFE::Image* createImage(uint8_t const *, uint32_t, uint32_t) override
            {
                return nullptr;
            }
            FIFE::Image* createImage(SDL_Surface*) override
            {
                return nullptr;
            }
            FIFE::Image* createImage(std::string const &, SDL_Surface*) override
            {
                return nullptr;
            }
            void renderVertexArrays() override
            {
            }
            void captureScreen(std::string const &) override
            {
            }
            void captureScreen(std::string const &, uint32_t, uint32_t) override
            {
            }
            bool putPixel(int32_t, int32_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
                return true;
            }
            void drawLine(FIFE::Point const &, FIFE::Point const &, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            void drawThickLine(
                FIFE::Point const &, FIFE::Point const &, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            void drawPolyLine(std::vector<FIFE::Point> const &, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            void drawBezier(
                std::vector<FIFE::Point> const &, int32_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            FIFE::Image* createImage(std::string const &, uint8_t const *, uint32_t, uint32_t) override
            {
                return nullptr;
            }
            void drawTriangle(
                FIFE::Point const &, FIFE::Point const &, FIFE::Point const &, uint8_t, uint8_t, uint8_t, uint8_t)
                override
            {
            }
            void drawRectangle(FIFE::Point const &, uint16_t, uint16_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            void fillRectangle(FIFE::Point const &, uint16_t, uint16_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            void drawQuad(
                FIFE::Point const &,
                FIFE::Point const &,
                FIFE::Point const &,
                FIFE::Point const &,
                uint8_t,
                uint8_t,
                uint8_t,
                uint8_t) override
            {
            }
            void drawVertex(FIFE::Point const &, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            void drawCircle(FIFE::Point const &, uint32_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            void drawFillCircle(FIFE::Point const &, uint32_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            void drawCircleSegment(
                FIFE::Point const &, uint32_t, int32_t, int32_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            void drawFillCircleSegment(
                FIFE::Point const &, uint32_t, int32_t, int32_t, uint8_t, uint8_t, uint8_t, uint8_t) override
            {
            }
            void drawLightPrimitive(
                FIFE::Point const &, uint8_t, float, int32_t, float, float, uint8_t, uint8_t, uint8_t) override
            {
            }
            void addImageToArray(uint32_t, FIFE::Rect const &, float const *, uint8_t, uint8_t const *) override
            {
            }
            void changeRenderInfos(
                FIFE::RenderDataType,
                uint16_t,
                int32_t,
                int32_t,
                bool,
                bool,
                uint8_t,
                FIFE::GLConstants,
                FIFE::GLConstants,
                FIFE::OverlayType) override
            {
            }
            void renderGuiGeometry(
                std::vector<FIFE::GuiVertex> const &,
                std::vector<int> const &,
                FIFE::DoublePoint const &,
                FIFE::ImagePtr) override
            {
            }
            void enableScissorTest() override
            {
            }
            void disableScissorTest() override
            {
            }
            void attachRenderTarget(FIFE::ImagePtr&, bool) override
            {
            }
            void detachRenderTarget() override
            {
            }
            void setClipArea(FIFE::Rect const &, bool) override
            {
            }
    };

    struct MockContainer : FIFE::IRendererContainer
    {
            FIFE::PathRenderer* pathRenderer{nullptr};
            FIFE::RendererBase* getRenderer(std::string const & name) override
            {
                if (name == "PathRenderer")
                    return pathRenderer;
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
    FIFE::PathStyleConfig cfg;
    CHECK_EQ(cfg.color.r, 0);
    CHECK_EQ(cfg.color.g, 100);
    CHECK_EQ(cfg.color.b, 255);
    CHECK_EQ(cfg.color.a, 255);
    CHECK_EQ(cfg.width, 2.0f);
    CHECK_EQ(cfg.style, FIFE::PLS_SOLID);
    CHECK_EQ(cfg.dashLength, 12.0f);
    CHECK_EQ(cfg.dashGap, 6.0f);
    CHECK_EQ(cfg.drawArrows, false);
    CHECK_EQ(cfg.arrowSize, 10.0f);
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

    FIFE::Instance* const i1 = reinterpret_cast<FIFE::Instance*>(static_cast<uintptr_t>(0x100));
    FIFE::Instance* const i2 = reinterpret_cast<FIFE::Instance*>(static_cast<uintptr_t>(0x200));

    FIFE::PathStyleConfig cfg;
    cfg.color = {255, 0, 0, 255};
    cfg.width = 4.0f;

    pr.addPath(i1, cfg);
    pr.addPath(i2, cfg);

    // Overwrite existing path style via setPathStyle
    FIFE::PathStyleConfig cfg2;
    cfg2.color = {0, 255, 0, 255};
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

    FIFE::Instance* const dummy = reinterpret_cast<FIFE::Instance*>(static_cast<uintptr_t>(0xDEAD));
    FIFE::PathStyleConfig cfg;
    pr.setPathStyle(dummy, cfg);
}

TEST_CASE("PathRenderer removePath for non-existent instance no-ops", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    FIFE::Instance* const dummy = reinterpret_cast<FIFE::Instance*>(static_cast<uintptr_t>(0xBEEF));
    pr.removePath(dummy);
}

TEST_CASE("PathRenderer getInstance", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);
    MockContainer mc;
    mc.pathRenderer = &pr;

    FIFE::PathRenderer* retrieved = FIFE::PathRenderer::getInstance(&mc);
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
    a.color     = {10, 20, 30, 40};
    a.width     = 5.0f;
    a.style     = FIFE::PLS_ARROWED;
    a.arrowSize = 15.0f;

    FIFE::PathStyleConfig b = a;
    CHECK_EQ(b.color.r, 10);
    CHECK_EQ(b.color.g, 20);
    CHECK_EQ(b.color.b, 30);
    CHECK_EQ(b.color.a, 40);
    CHECK_EQ(b.width, 5.0f);
    CHECK_EQ(b.style, FIFE::PLS_ARROWED);
    CHECK_EQ(b.arrowSize, 15.0f);
}

TEST_CASE("PathRenderer clone propagates configs", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    FIFE::Instance* const inst = reinterpret_cast<FIFE::Instance*>(static_cast<uintptr_t>(0x100));
    FIFE::PathStyleConfig cfg;
    cfg.color = {100, 200, 50, 255};
    cfg.width = 3.0f;
    cfg.style = FIFE::PLS_DASHED;
    pr.addPath(inst, cfg);

    std::unique_ptr<FIFE::RendererBase> cloned(pr.clone());
    CHECK_EQ(cloned->getName(), "PathRenderer");

    // The cloned renderer shuold also be disabled
    CHECK(cloned->isEnabled() == false);
}
