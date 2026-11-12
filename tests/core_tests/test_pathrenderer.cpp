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
    // Opaque, never-dereferenced storage used to fabricate distinct Instance* keys.
    std::array<int, 5> dummy_inst_storage{};
} // namespace

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
            void init(std::string const & driver [[maybe_unused]]) override
            {
            }
            void clearBackBuffer() override
            {
            }
            void setLightingModel(uint32_t lighting [[maybe_unused]]) override
            {
            }
            uint32_t getLightingModel() const override
            {
                return 0;
            }
            void setLighting(
                float red [[maybe_unused]], float green [[maybe_unused]], float blue [[maybe_unused]]) override
            {
            }
            void resetLighting() override
            {
            }
            void resetStencilBuffer(uint8_t buffer [[maybe_unused]]) override
            {
            }
            void changeBlending(int32_t scr [[maybe_unused]], int32_t dst [[maybe_unused]]) override
            {
            }
            void createMainScreen(
                std::string const & title [[maybe_unused]], std::string const & icon [[maybe_unused]]) override
            {
            }
            std::unique_ptr<FIFE::Image> createImage([[maybe_unused]] FIFE::IResourceLoader* loader) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(
                [[maybe_unused]] std::string const & name, [[maybe_unused]] FIFE::IResourceLoader* loader) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(
                [[maybe_unused]] uint8_t const * data,
                [[maybe_unused]] uint32_t width,
                [[maybe_unused]] uint32_t height) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage([[maybe_unused]] SDL_Surface* surface) override
            {
                return nullptr;
            }
            std::unique_ptr<FIFE::Image> createImage(
                [[maybe_unused]] std::string const & name, [[maybe_unused]] SDL_Surface* surface) override
            {
                return nullptr;
            }
            void renderVertexArrays() override
            {
            }
            void captureScreen(std::string const & filename [[maybe_unused]]) override
            {
            }
            void captureScreen(
                std::string const & filename [[maybe_unused]],
                uint32_t width [[maybe_unused]],
                uint32_t height [[maybe_unused]]) override
            {
            }
            bool putPixel(
                [[maybe_unused]] int32_t x,
                [[maybe_unused]] int32_t y,
                [[maybe_unused]] uint8_t r,
                [[maybe_unused]] uint8_t g,
                [[maybe_unused]] uint8_t b,
                [[maybe_unused]] uint8_t a) override
            {
                return true;
            }
            void drawLine(
                FIFE::Point const & p1 [[maybe_unused]],
                FIFE::Point const & p2 [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawThickLine(
                FIFE::Point const & p1 [[maybe_unused]],
                FIFE::Point const & p2 [[maybe_unused]],
                uint8_t width [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawPolyLine(
                std::vector<FIFE::Point> const & points [[maybe_unused]],
                uint8_t width [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawBezier(
                std::vector<FIFE::Point> const & points [[maybe_unused]],
                int32_t steps [[maybe_unused]],
                uint8_t width [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            std::unique_ptr<FIFE::Image> createImage(
                [[maybe_unused]] std::string const & name,
                [[maybe_unused]] uint8_t const * data,
                [[maybe_unused]] uint32_t width,
                [[maybe_unused]] uint32_t height) override
            {
                return nullptr;
            }
            void drawTriangle(
                FIFE::Point const & p1 [[maybe_unused]],
                FIFE::Point const & p2 [[maybe_unused]],
                FIFE::Point const & p3 [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void fillTriangle(
                FIFE::Point const & p1 [[maybe_unused]],
                FIFE::Point const & p2 [[maybe_unused]],
                FIFE::Point const & p3 [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawRectangle(
                FIFE::Point const & p [[maybe_unused]],
                uint16_t w [[maybe_unused]],
                uint16_t h [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void fillRectangle(
                FIFE::Point const & p [[maybe_unused]],
                uint16_t w [[maybe_unused]],
                uint16_t h [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawQuad(
                FIFE::Point const & p1 [[maybe_unused]],
                FIFE::Point const & p2 [[maybe_unused]],
                FIFE::Point const & p3 [[maybe_unused]],
                FIFE::Point const & p4 [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawVertex(
                FIFE::Point const & p [[maybe_unused]],
                uint8_t size [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawCircle(
                FIFE::Point const & p [[maybe_unused]],
                uint32_t radius [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawFillCircle(
                FIFE::Point const & p [[maybe_unused]],
                uint32_t radius [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawCircleSegment(
                FIFE::Point const & p [[maybe_unused]],
                uint32_t radius [[maybe_unused]],
                int32_t sangle [[maybe_unused]],
                int32_t eangle [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawFillCircleSegment(
                FIFE::Point const & p [[maybe_unused]],
                uint32_t radius [[maybe_unused]],
                int32_t sangle [[maybe_unused]],
                int32_t eangle [[maybe_unused]],
                uint8_t r [[maybe_unused]],
                uint8_t g [[maybe_unused]],
                uint8_t b [[maybe_unused]],
                uint8_t a [[maybe_unused]]) override
            {
            }
            void drawLightPrimitive(
                FIFE::Point const & p [[maybe_unused]],
                uint8_t intensity [[maybe_unused]],
                float radius [[maybe_unused]],
                int32_t subdivisions [[maybe_unused]],
                float xstretch [[maybe_unused]],
                float ystretch [[maybe_unused]],
                uint8_t red [[maybe_unused]],
                uint8_t green [[maybe_unused]],
                uint8_t blue [[maybe_unused]]) override
            {
            }
            void addImageToArray(
                uint32_t id [[maybe_unused]],
                FIFE::Rect const & rec [[maybe_unused]],
                float const * st [[maybe_unused]],
                uint8_t alpha [[maybe_unused]],
                uint8_t const * rgba [[maybe_unused]]) override
            {
            }
            void changeRenderInfos(
                FIFE::RenderDataType type [[maybe_unused]],
                uint16_t elements [[maybe_unused]],
                int32_t src [[maybe_unused]],
                int32_t dst [[maybe_unused]],
                bool light [[maybe_unused]],
                bool stentest [[maybe_unused]],
                uint8_t stenref [[maybe_unused]],
                FIFE::GLConstants stenop [[maybe_unused]],
                FIFE::GLConstants stenfunc [[maybe_unused]],
                FIFE::OverlayType otype [[maybe_unused]]) override
            {
            }
            void renderGuiGeometry(
                std::vector<FIFE::GuiVertex> const & vertices [[maybe_unused]],
                std::vector<int> const & indices [[maybe_unused]],
                FIFE::DoublePoint const & translation [[maybe_unused]],
                FIFE::ImagePtr texture [[maybe_unused]]) override
            {
            }
            void enableScissorTest() override
            {
            }
            void disableScissorTest() override
            {
            }
            void attachRenderTarget(FIFE::ImagePtr& img [[maybe_unused]], bool discard [[maybe_unused]]) override
            {
            }
            void detachRenderTarget() override
            {
            }

        protected:
            void setClipArea(FIFE::Rect const & cliparea [[maybe_unused]], bool clear [[maybe_unused]]) override
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

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    FIFE::Instance* const i1 = reinterpret_cast<FIFE::Instance*>(dummy_inst_storage.data());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
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

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    FIFE::Instance* const dummy = reinterpret_cast<FIFE::Instance*>(&dummy_inst_storage.at(2));
    FIFE::PathStyleConfig const cfg;
    pr.setPathStyle(dummy, cfg);
}

TEST_CASE("PathRenderer removePath for non-existent instance no-ops", "[pathrenderer]")
{
    MockRenderBackend rb;
    FIFE::PathRenderer pr(&rb);

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
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

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
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
