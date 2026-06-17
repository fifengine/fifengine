// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <array>
#include <memory>
#include <string>
#include <vector>

// Platform specific includes
#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "video/renderbackend.h"
#include "view/rendererbase.h"
#include "view/renderers/pathrenderer.h"

namespace
{
    std::array<int, 5> dummy_inst_storage{};
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
            void init(std::string const & driver) override
            {
            }
            void clearBackBuffer() override
            {
            }
            void setLightingModel(uint32_t lighting) override
            {
            }
            uint32_t getLightingModel() const override
            {
                return 0;
            }
            void setLighting(float red, float green, float blue) override
            {
            }
            void resetLighting() override
            {
            }
            void resetStencilBuffer(uint8_t buffer) override
            {
            }
            void changeBlending(int32_t scr, int32_t dst) override
            {
            }
            void createMainScreen(std::string const & title, std::string const & icon) override
            {
            }
            std::unique_ptr<FIFE::Image> createImage(FIFE::IResourceLoader* loader) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(std::string const & name, FIFE::IResourceLoader* loader) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(uint8_t const * data, uint32_t width, uint32_t height) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(SDL_Surface* surface) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(std::string const & name, SDL_Surface* surface) override
            {
                return nullptr;
            }
            void renderVertexArrays() override
            {
            }
            void captureScreen(std::string const & filename) override
            {
            }
            void captureScreen(std::string const & filename, uint32_t width, uint32_t height) override
            {
            }
            bool putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
                return true;
            }
            void drawLine(
                FIFE::Point const & p1, FIFE::Point const & p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
            }
            void drawThickLine(
                FIFE::Point const & p1,
                FIFE::Point const & p2,
                uint8_t width,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a) override
            {
            }
            void drawPolyLine(
                std::vector<FIFE::Point> const & points, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
                override
            {
            }
            void drawBezier(
                std::vector<FIFE::Point> const & points,
                int32_t steps,
                uint8_t width,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a) override
            {
            }
            std::unique_ptr<FIFE::Image> createImage(
                std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) override
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
                FIFE::Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
            }
            void fillRectangle(
                FIFE::Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
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
            void drawVertex(FIFE::Point const & p, uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
            }
            void drawCircle(FIFE::Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
            }
            void drawFillCircle(
                FIFE::Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override
            {
            }
            void drawCircleSegment(
                FIFE::Point const & p,
                uint32_t radius,
                int32_t sangle,
                int32_t eangle,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a) override
            {
            }
            void drawFillCircleSegment(
                FIFE::Point const & p,
                uint32_t radius,
                int32_t sangle,
                int32_t eangle,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a) override
            {
            }
            void drawLightPrimitive(
                FIFE::Point const & p,
                uint8_t intensity,
                float radius,
                int32_t subdivisions,
                float xstretch,
                float ystretch,
                uint8_t red,
                uint8_t green,
                uint8_t blue) override
            {
            }
            void addImageToArray(
                uint32_t id, FIFE::Rect const & rec, float const * st, uint8_t alpha, uint8_t const * rgba) override
            {
            }
            void changeRenderInfos(
                FIFE::RenderDataType type,
                uint16_t elements,
                int32_t src,
                int32_t dst,
                bool light,
                bool stentest,
                uint8_t stenref,
                FIFE::GLConstants stenop,
                FIFE::GLConstants stenfunc,
                FIFE::OverlayType otype) override
            {
            }
            void renderGuiGeometry(
                std::vector<FIFE::GuiVertex> const & vertices,
                std::vector<int> const & indices,
                FIFE::DoublePoint const & translation,
                FIFE::ImagePtr texture) override
            {
            }
            void enableScissorTest() override
            {
            }
            void disableScissorTest() override
            {
            }
            void attachRenderTarget(FIFE::ImagePtr& img, bool discard) override
            {
            }
            void detachRenderTarget() override
            {
            }

        protected:
            void setClipArea(FIFE::Rect const & cliparea, bool clear) override
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

TEST_CASE("PathStyleConfig default values", "[pathrenderer]")
{
    FIFE::PathStyleConfig const cfg;
    CHECK((cfg.color.r) == (0));
    CHECK((cfg.color.g) == (100));
    CHECK((cfg.color.b) == (255));
    CHECK((cfg.color.a) == (255));
    CHECK((cfg.width) == (2.0F));
    CHECK((cfg.style) == (FIFE::PLS_SOLID));
    CHECK((cfg.dashLength) == (12.0F));
    CHECK((cfg.dashGap) == (6.0F));
    CHECK((cfg.drawArrows) == (false));
    CHECK((cfg.arrowSize) == (10.0F));
}

TEST_CASE("PathRenderer construction and basic properties", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    CHECK((pr.getName()) == ("PathRenderer"));
    CHECK(pr.isEnabled() == false);

    // Clone preserves name and disabled state
    std::unique_ptr<FIFE::RendererBase> cloned(pr.clone());
    CHECK((cloned->getName()) == ("PathRenderer"));
    CHECK(cloned->isEnabled() == false);
}

TEST_CASE("PathRenderer add/remove/clear paths", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    FIFE::Instance* const i1 = reinterpret_cast<FIFE::Instance*>(dummy_inst_storage.data());
    FIFE::Instance* const i2 = reinterpret_cast<FIFE::Instance*>(&dummy_inst_storage.at(1));

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

    FIFE::Instance* const dummy = reinterpret_cast<FIFE::Instance*>(&dummy_inst_storage.at(2));
    FIFE::PathStyleConfig const cfg;
    pr.setPathStyle(dummy, cfg);
}

TEST_CASE("PathRenderer removePath for non-existent instance no-ops", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    FIFE::Instance* const dummy = reinterpret_cast<FIFE::Instance*>(&dummy_inst_storage.at(3));
    pr.removePath(dummy);
}

TEST_CASE("PathRenderer getInstance", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);
    MockContainer mc;
    mc.pathRenderer = &pr;

    FIFE::PathRenderer const * retrieved = FIFE::PathRenderer::getInstance(&mc);
    CHECK((retrieved) == (&pr));
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
    CHECK((b.color.r) == (10));
    CHECK((b.color.g) == (20));
    CHECK((b.color.b) == (30));
    CHECK((b.color.a) == (40));
    CHECK((b.width) == (5.0F));
    CHECK((b.style) == (FIFE::PLS_ARROWED));
    CHECK((b.arrowSize) == (15.0F));
}

TEST_CASE("PathRenderer clone propagates configs", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    FIFE::Instance* const inst = reinterpret_cast<FIFE::Instance*>(&dummy_inst_storage.at(4));
    FIFE::PathStyleConfig cfg;
    cfg.color = SDL_Color{.r = 100, .g = 200, .b = 50, .a = 255};
    cfg.width = 3.0F;
    cfg.style = FIFE::PLS_DASHED;
    pr.addPath(inst, cfg);

    std::unique_ptr<FIFE::RendererBase> cloned(pr.clone());
    CHECK((cloned->getName()) == ("PathRenderer"));

    // The cloned renderer shuold also be disabled
    CHECK(cloned->isEnabled() == false);
}
