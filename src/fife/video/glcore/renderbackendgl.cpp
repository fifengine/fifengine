// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Shell translation unit: renderbackend.h brings in GLEW, so all GL calls are
// delegated to glcore_draw.cpp, which uses only the profile's glad loader.
#include "renderbackendgl.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "util/base/exception.h"
#include "video/glcore/glcore_draw.h"
#include "video/glcore/glcoreimage.h"
#include "video/window/window.h"

namespace FIFE
{
    RenderBackendGL::RenderBackendGL(SDL_Color const & colorkey, GLContextConfig config, std::string name) :
        RenderBackendDummy(colorkey), m_name(std::move(name)), m_context(config)
    {
    }

    RenderBackendGL::~RenderBackendGL() = default;

    std::string RenderBackendGL::getRenderDescription() const
    {
        return glcore_draw::renderDescription();
    }

    void RenderBackendGL::init(std::string const &)
    {
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
            throw SDLException(SDL_GetError());
        }
    }

    void RenderBackendGL::createMainScreen(std::string const &, std::string const &)
    {
        SDL_Window* const window = getAttachedWindow();
        if (window == nullptr) {
            return;
        }
        if (m_window != nullptr && m_window != window) {
            // The Window object replaced its SDL window: all GL objects belonged
            // to the old context and must be recreated from scratch.
            m_fbo        = 0;
            m_fboTexture = 0;
            m_shaders.reset();
            glcore_draw::reset();
            m_context.destroy();
            m_state.reset();
        }
        m_window = window;
        m_context.applyAttributes();
        if (!m_context.isLoaded()) {
            if (!m_context.create(window)) {
                throw SDLException(SDL_GetError());
            }
            m_context.makeCurrent(window);
            glcore_draw::reset();
            m_shaders = std::make_unique<ShaderLoader>(m_context.config().profile);
        } else {
            m_context.makeCurrent(window);
        }

        uint32_t w = 0;
        uint32_t h = 0;
        glcoreGetDrawableSize(window, w, h);
        m_state.setViewport(0, 0, w, h);
        glcore_draw::setScissor(0, 0, static_cast<int32_t>(w), static_cast<int32_t>(h));
        m_state.setScissor(FIFE::Rect(0, 0, static_cast<int32_t>(w), static_cast<int32_t>(h)), true);
        m_state.setProjection(0.0F, static_cast<float>(w), static_cast<float>(h), 0.0F, -100.0F, 100.0F);
        glcore_draw::enableBlend();
        m_state.setBlend(0x0302U, 0x0303U); // GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
    }

    void RenderBackendGL::endFrame()
    {
        if (SDL_Window* const window = getAttachedWindow()) {
            m_context.swap(window);
        }
    }

    void RenderBackendGL::clearBackBuffer()
    {
        glcore_draw::clear();
    }

    void RenderBackendGL::setScalingMode(ScalingMode mode)
    {
        int width  = 0;
        int height = 0;
        if (m_windowObject != nullptr) {
            width  = m_windowObject->getWidthInPixels();
            height = m_windowObject->getHeightInPixels();
            if (mode == ScalingMode::Integer) {
                width  = m_windowObject->getWidthInPoints();
                height = m_windowObject->getHeightInPoints();
            }
        } else if (m_window != nullptr) {
            SDL_GetWindowSize(m_window, &width, &height);
        }
        if (width <= 0 || height <= 0) {
            return;
        }
        uint32_t drawableW = 0;
        uint32_t drawableH = 0;
        glcoreGetDrawableSize(m_window, drawableW, drawableH);
        m_state.setViewport(0, 0, drawableW, drawableH);
        m_state.setProjection(0.0F, static_cast<float>(width), static_cast<float>(height), 0.0F, -100.0F, 100.0F);
    }

    void RenderBackendGL::setLightingModel(uint32_t lighting)
    {
        m_lightingModel = lighting;
    }

    uint32_t RenderBackendGL::getLightingModel() const
    {
        return m_lightingModel;
    }

    void RenderBackendGL::setLighting(float, float, float)
    {
    }

    void RenderBackendGL::resetLighting()
    {
        m_lightingModel = 0;
    }

    void RenderBackendGL::resetStencilBuffer(uint8_t)
    {
        glcore_draw::clearStencil();
    }

    void RenderBackendGL::changeBlending(int32_t src, int32_t dst)
    {
        m_state.setBlend(static_cast<uint32_t>(src), static_cast<uint32_t>(dst));
    }

    void RenderBackendGL::addTriangle(PrimitiveVertex const & a, PrimitiveVertex const & b, PrimitiveVertex const & c)
    {
        m_primitiveVertices.push_back(a);
        m_primitiveVertices.push_back(b);
        m_primitiveVertices.push_back(c);
    }

    void RenderBackendGL::addQuad(
        PrimitiveVertex const & a, PrimitiveVertex const & b, PrimitiveVertex const & c, PrimitiveVertex const & d)
    {
        addTriangle(a, b, c);
        addTriangle(a, c, d);
    }

    void glcoreAddImageToArray(uint32_t id, Rect const & rect, float const * st, uint8_t alpha, uint8_t const * rgba)
    {
        if (RenderBackend::exists()) {
            RenderBackend::instance()->addImageToArray(id, rect, st, alpha, rgba);
        }
    }

    void glcoreAddImageToArrayOverlay(
        Rect const & rect,
        uint32_t id1,
        float const * st1,
        uint32_t id2,
        float const * st2,
        uint8_t alpha,
        uint8_t const * rgba)
    {
        if (RenderBackend::exists()) {
            RenderBackend::instance()->addImageToArray(rect, id1, st1, id2, st2, alpha, rgba);
        }
    }

    std::unique_ptr<Image> RenderBackendGL::createImage(IResourceLoader* loader)
    {
        return std::make_unique<GLCoreImage>(loader);
    }

    std::unique_ptr<Image> RenderBackendGL::createImage(std::string const & name, IResourceLoader* loader)
    {
        return std::make_unique<GLCoreImage>(name, loader);
    }

    std::unique_ptr<Image> RenderBackendGL::createImage(uint8_t const * data, uint32_t width, uint32_t height)
    {
        return std::make_unique<GLCoreImage>(data, width, height);
    }

    std::unique_ptr<Image> RenderBackendGL::createImage(
        std::string const & name, uint8_t const * data, uint32_t width, uint32_t height)
    {
        return std::make_unique<GLCoreImage>(name, data, width, height);
    }

    std::unique_ptr<Image> RenderBackendGL::createImage(SDL_Surface* surface)
    {
        return std::make_unique<GLCoreImage>(surface);
    }

    std::unique_ptr<Image> RenderBackendGL::createImage(std::string const & name, SDL_Surface* surface)
    {
        return std::make_unique<GLCoreImage>(name, surface);
    }

    void RenderBackendGL::renderVertexArrays()
    {
        if (!m_shaders) {
            return;
        }

        std::vector<float> primitiveData;
        primitiveData.reserve(m_primitiveVertices.size() * 6);
        for (PrimitiveVertex const & v : m_primitiveVertices) {
            primitiveData.push_back(v.x);
            primitiveData.push_back(v.y);
            primitiveData.push_back(v.r);
            primitiveData.push_back(v.g);
            primitiveData.push_back(v.b);
            primitiveData.push_back(v.a);
        }

        std::vector<glcore_draw::ImageBatchView> views;
        views.reserve(m_imageBatches.size());
        for (ImageBatch const & batch : m_imageBatches) {
            if (!batch.vertices.empty()) {
                views.push_back(
                    {batch.texture,
                     batch.secondTexture,
                     batch.overlayMode,
                     batch.vertices.data(),
                     batch.vertices.size()});
            }
        }

        uint32_t const primitiveProgram = m_shaders->get(GLCoreProgram::Primitive).program();
        uint32_t const spriteProgram    = m_shaders->get(GLCoreProgram::Sprite).program();
        uint32_t const overlayProgram   = m_shaders->get(GLCoreProgram::Overlay).program();
        glcore_draw::drawBatches(
            primitiveData.empty() ? nullptr : primitiveData.data(),
            primitiveData.size(),
            views,
            primitiveProgram,
            spriteProgram,
            overlayProgram,
            m_state.projection());

        m_primitiveVertices.clear();
        m_imageBatches.clear();
    }

    void RenderBackendGL::addImageToArray(
        Rect const & rect,
        uint32_t id1,
        float const *,
        uint32_t id2,
        float const *,
        uint8_t alpha,
        uint8_t const * rgba)
    {
        // Reuse the single-texture path to emit the quad geometry; then tag
        // the resulting batch with the overlay texture and mode.
        float const fullTexCoords[4]{0.0F, 0.0F, 1.0F, 1.0F};
        addImageToArray(id1, rect, fullTexCoords, alpha, rgba);
        if (!m_imageBatches.empty()) {
            m_imageBatches.back().secondTexture = id2;
            m_imageBatches.back().overlayMode   = static_cast<uint32_t>(m_overlayType);
        }
    }

    void RenderBackendGL::addImageToArray(
        uint32_t id, Rect const & rec, float const * st, uint8_t alpha, uint8_t const * rgba)
    {
        auto const found =
            std::find_if(m_imageBatches.begin(), m_imageBatches.end(), [id](ImageBatch const & candidate) {
                return candidate.texture == id;
            });
        ImageBatch* batch = found != m_imageBatches.end() ? &*found : nullptr;
        if (batch == nullptr) {
            ImageBatch newBatch;
            newBatch.texture = id;
            m_imageBatches.push_back(std::move(newBatch));
            batch = &m_imageBatches.back();
        }

        float const x0    = static_cast<float>(rec.x);
        float const y0    = static_cast<float>(rec.y);
        float const x1    = static_cast<float>(rec.x + rec.w);
        float const y1    = static_cast<float>(rec.y + rec.h);
        float const u0    = st[0];
        float const v0    = st[1];
        float const u1    = st[2];
        float const v1    = st[3];
        float const r     = rgba != nullptr ? static_cast<float>(rgba[0]) / 255.0F : 1.0F;
        float const g     = rgba != nullptr ? static_cast<float>(rgba[1]) / 255.0F : 1.0F;
        float const b     = rgba != nullptr ? static_cast<float>(rgba[2]) / 255.0F : 1.0F;
        float const a     = static_cast<float>(alpha) / 255.0F;
        auto const vertex = [&](float x, float y, float u, float v) {
            batch->vertices.push_back(x);
            batch->vertices.push_back(y);
            batch->vertices.push_back(u);
            batch->vertices.push_back(v);
            batch->vertices.push_back(r);
            batch->vertices.push_back(g);
            batch->vertices.push_back(b);
            batch->vertices.push_back(a);
        };
        vertex(x0, y0, u0, v0);
        vertex(x1, y0, u1, v0);
        vertex(x1, y1, u1, v1);
        vertex(x0, y1, u0, v1);
    }

    bool RenderBackendGL::putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        auto const color = [r, g, b, a](float px, float py) {
            return PrimitiveVertex{
                px,
                py,
                static_cast<float>(r) / 255.0F,
                static_cast<float>(g) / 255.0F,
                static_cast<float>(b) / 255.0F,
                static_cast<float>(a) / 255.0F};
        };
        float const px = static_cast<float>(x);
        float const py = static_cast<float>(y);
        addQuad(color(px, py), color(px + 1.0F, py), color(px + 1.0F, py + 1.0F), color(px, py + 1.0F));
        return true;
    }

    void RenderBackendGL::drawLine(Point const & p1, Point const & p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        drawThickLine(p1, p2, 1, r, g, b, a);
    }

    void RenderBackendGL::drawThickLine(
        Point const & p1, Point const & p2, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        float const dx  = static_cast<float>(p2.x - p1.x);
        float const dy  = static_cast<float>(p2.y - p1.y);
        float const len = std::sqrt(dx * dx + dy * dy);
        if (len <= 0.0F) {
            return;
        }
        float const nx   = -dy / len * (static_cast<float>(width) / 2.0F);
        float const ny   = dx / len * (static_cast<float>(width) / 2.0F);
        auto const color = [r, g, b, a](float x, float y) {
            return PrimitiveVertex{
                x,
                y,
                static_cast<float>(r) / 255.0F,
                static_cast<float>(g) / 255.0F,
                static_cast<float>(b) / 255.0F,
                static_cast<float>(a) / 255.0F};
        };
        addQuad(
            color(static_cast<float>(p1.x) + nx, static_cast<float>(p1.y) + ny),
            color(static_cast<float>(p2.x) + nx, static_cast<float>(p2.y) + ny),
            color(static_cast<float>(p2.x) - nx, static_cast<float>(p2.y) - ny),
            color(static_cast<float>(p1.x) - nx, static_cast<float>(p1.y) - ny));
    }

    void RenderBackendGL::drawPolyLine(
        std::vector<Point> const & points, [[maybe_unused]] uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        for (size_t i = 1; i < points.size(); ++i) {
            drawLine(points[i - 1], points[i], r, g, b, a);
        }
    }

    void RenderBackendGL::drawBezier(
        std::vector<Point> const & points, int32_t steps, uint8_t, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (points.size() < 2 || steps < 2) {
            return;
        }
        std::vector<Point> sampled;
        int32_t const count = steps * static_cast<int32_t>(points.size() - 1);
        for (int32_t i = 0; i <= count; ++i) {
            float const t = static_cast<float>(i) / static_cast<float>(count);
            sampled.push_back(getBezierPoint(points, static_cast<int32_t>(points.size()), t));
        }
        drawPolyLine(sampled, 1, r, g, b, a);
    }

    void RenderBackendGL::addTriangle(
        Point const & p1, Point const & p2, Point const & p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool filled)
    {
        auto const color = [r, g, b, a](Point const & p) {
            return PrimitiveVertex{
                static_cast<float>(p.x),
                static_cast<float>(p.y),
                static_cast<float>(r) / 255.0F,
                static_cast<float>(g) / 255.0F,
                static_cast<float>(b) / 255.0F,
                static_cast<float>(a) / 255.0F};
        };
        if (filled) {
            addTriangle(color(p1), color(p2), color(p3));
        } else {
            drawLine(p1, p2, r, g, b, a);
            drawLine(p2, p3, r, g, b, a);
            drawLine(p3, p1, r, g, b, a);
        }
    }

    void RenderBackendGL::drawTriangle(
        Point const & p1, Point const & p2, Point const & p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        addTriangle(p1, p2, p3, r, g, b, a, false);
    }

    void RenderBackendGL::fillTriangle(
        Point const & p1, Point const & p2, Point const & p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        addTriangle(p1, p2, p3, r, g, b, a, true);
    }

    void RenderBackendGL::drawRectangle(
        Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        Point const p2(p.x + w, p.y), p3(p.x + w, p.y + h), p4(p.x, p.y + h);
        drawLine(p, p2, r, g, b, a);
        drawLine(p2, p3, r, g, b, a);
        drawLine(p3, p4, r, g, b, a);
        drawLine(p4, p, r, g, b, a);
    }

    void RenderBackendGL::fillRectangle(
        Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        Point const p2(p.x + w, p.y), p3(p.x + w, p.y + h), p4(p.x, p.y + h);
        addQuad(
            PrimitiveVertex{
                static_cast<float>(p.x),
                static_cast<float>(p.y),
                static_cast<float>(r) / 255.0F,
                static_cast<float>(g) / 255.0F,
                static_cast<float>(b) / 255.0F,
                static_cast<float>(a) / 255.0F},
            PrimitiveVertex{
                static_cast<float>(p2.x),
                static_cast<float>(p2.y),
                static_cast<float>(r) / 255.0F,
                static_cast<float>(g) / 255.0F,
                static_cast<float>(b) / 255.0F,
                static_cast<float>(a) / 255.0F},
            PrimitiveVertex{
                static_cast<float>(p3.x),
                static_cast<float>(p3.y),
                static_cast<float>(r) / 255.0F,
                static_cast<float>(g) / 255.0F,
                static_cast<float>(b) / 255.0F,
                static_cast<float>(a) / 255.0F},
            PrimitiveVertex{
                static_cast<float>(p4.x),
                static_cast<float>(p4.y),
                static_cast<float>(r) / 255.0F,
                static_cast<float>(g) / 255.0F,
                static_cast<float>(b) / 255.0F,
                static_cast<float>(a) / 255.0F});
    }

    void RenderBackendGL::drawQuad(
        Point const & p1,
        Point const & p2,
        Point const & p3,
        Point const & p4,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a)
    {
        drawLine(p1, p2, r, g, b, a);
        drawLine(p2, p3, r, g, b, a);
        drawLine(p3, p4, r, g, b, a);
        drawLine(p4, p1, r, g, b, a);
    }

    void RenderBackendGL::drawVertex(Point const & p, uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        drawFillCircle(p, size, r, g, b, a);
    }

    void RenderBackendGL::drawCircle(Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        constexpr int segments = 24;
        float const rad        = static_cast<float>(radius);
        for (int i = 0; i < segments; ++i) {
            float const a1 = 2.0F * 3.14159265F * static_cast<float>(i) / static_cast<float>(segments);
            float const a2 = 2.0F * 3.14159265F * static_cast<float>(i + 1) / static_cast<float>(segments);
            drawLine(
                Point(
                    static_cast<int32_t>(static_cast<float>(p.x) + std::cos(a1) * rad),
                    static_cast<int32_t>(static_cast<float>(p.y) + std::sin(a1) * rad)),
                Point(
                    static_cast<int32_t>(static_cast<float>(p.x) + std::cos(a2) * rad),
                    static_cast<int32_t>(static_cast<float>(p.y) + std::sin(a2) * rad)),
                r,
                g,
                b,
                a);
        }
    }

    void RenderBackendGL::drawFillCircle(Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        constexpr int segments = 24;
        float const rad        = static_cast<float>(radius);
        auto const color       = [r, g, b, a]() {
            return PrimitiveVertex{
                0.0F,
                0.0F,
                static_cast<float>(r) / 255.0F,
                static_cast<float>(g) / 255.0F,
                static_cast<float>(b) / 255.0F,
                static_cast<float>(a) / 255.0F};
        };
        PrimitiveVertex const center{
            static_cast<float>(p.x),
            static_cast<float>(p.y),
            static_cast<float>(r) / 255.0F,
            static_cast<float>(g) / 255.0F,
            static_cast<float>(b) / 255.0F,
            static_cast<float>(a) / 255.0F};
        for (int i = 0; i < segments; ++i) {
            float const a1     = 2.0F * 3.14159265F * static_cast<float>(i) / static_cast<float>(segments);
            float const a2     = 2.0F * 3.14159265F * static_cast<float>(i + 1) / static_cast<float>(segments);
            PrimitiveVertex v1 = color();
            v1.x               = static_cast<float>(p.x) + std::cos(a1) * rad;
            v1.y               = static_cast<float>(p.y) + std::sin(a1) * rad;
            PrimitiveVertex v2 = color();
            v2.x               = static_cast<float>(p.x) + std::cos(a2) * rad;
            v2.y               = static_cast<float>(p.y) + std::sin(a2) * rad;
            addTriangle(center, v1, v2);
        }
    }

    void RenderBackendGL::drawCircleSegment(
        Point const & p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        int32_t const span = eangle - sangle;
        if (span <= 0) {
            return;
        }
        int const segments = 24;
        auto const toPoint = [&](int step) {
            double const angle =
                (static_cast<double>(sangle) + static_cast<double>(span) * step / segments) * 3.141592653589793 / 180.0;
            return Point(
                static_cast<int32_t>(static_cast<double>(p.x) + std::cos(angle) * radius),
                static_cast<int32_t>(static_cast<double>(p.y) + std::sin(angle) * radius));
        };
        for (int i = 0; i < segments; ++i) {
            drawLine(toPoint(i), toPoint(i + 1), r, g, b, a);
        }
    }

    void RenderBackendGL::drawFillCircleSegment(
        Point const & p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        int32_t const span = eangle - sangle;
        if (span <= 0) {
            return;
        }
        int const segments  = 24;
        auto const toVertex = [&](int step) {
            double const angle =
                (static_cast<double>(sangle) + static_cast<double>(span) * step / segments) * 3.141592653589793 / 180.0;
            return PrimitiveVertex{
                static_cast<float>(static_cast<double>(p.x) + std::cos(angle) * radius),
                static_cast<float>(static_cast<double>(p.y) + std::sin(angle) * radius),
                static_cast<float>(r) / 255.0F,
                static_cast<float>(g) / 255.0F,
                static_cast<float>(b) / 255.0F,
                static_cast<float>(a) / 255.0F};
        };
        PrimitiveVertex const center{
            static_cast<float>(p.x),
            static_cast<float>(p.y),
            static_cast<float>(r) / 255.0F,
            static_cast<float>(g) / 255.0F,
            static_cast<float>(b) / 255.0F,
            static_cast<float>(a) / 255.0F};
        for (int i = 0; i < segments; ++i) {
            addTriangle(center, toVertex(i), toVertex(i + 1));
        }
    }

    void RenderBackendGL::drawLightPrimitive(
        Point const & p,
        uint8_t intensity,
        float radius,
        int32_t,
        float xstretch,
        float ystretch,
        uint8_t red,
        uint8_t green,
        uint8_t blue)
    {
        float const scale = static_cast<float>(intensity) / 255.0F;
        auto const shade  = [scale](uint8_t c) {
            return static_cast<uint8_t>(static_cast<float>(c) * scale);
        };
        drawFillCircle(
            p,
            static_cast<uint32_t>(std::max(1.0F, radius * std::max(xstretch, ystretch))),
            shade(red),
            shade(green),
            shade(blue),
            255);
    }

    void RenderBackendGL::prepareForOverlays()
    {
        // Overlay mode is carried per batch by m_overlayType (set from
        // changeRenderInfos); no additional pipeline state is required.
    }

    void RenderBackendGL::changeRenderInfos(
        RenderDataType type,
        uint16_t,
        int32_t src,
        int32_t dst,
        bool light,
        bool stentest,
        uint8_t stenref,
        GLConstants stenop,
        GLConstants stenfunc,
        OverlayType otype)
    {
        m_renderDataType     = type;
        m_overlayType        = otype;
        m_lightEnabled       = light;
        m_stateDebugBlendSrc = static_cast<uint32_t>(src);
        m_stateDebugBlendDst = static_cast<uint32_t>(dst);
        m_stateDebugStencil  = stentest;
        m_state.setBlend(m_stateDebugBlendSrc, m_stateDebugBlendDst);
        m_state.setDepthTest(type != RENDER_DATA_WITHOUT_Z, 0x0203U); // GL_LEQUAL
        m_state.setStencil(stentest, stenref, static_cast<uint32_t>(stenop), static_cast<uint32_t>(stenfunc));
    }

    void RenderBackendGL::captureScreen(std::string const & filename)
    {
        int w = 0;
        int h = 0;
        if (m_window != nullptr) {
            SDL_GetWindowSize(m_window, &w, &h);
        }
        if (w <= 0 || h <= 0) {
            return;
        }
        SDL_Surface* surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
        if (surface == nullptr) {
            return;
        }
        std::vector<uint8_t> pixels(static_cast<size_t>(w * h * 4));
        glcore_draw::readRGBA(0, 0, w, h, pixels.data());
        for (int row = 0; row < h; ++row) {
            std::copy_n(
                pixels.data() + static_cast<size_t>((h - 1 - row) * w * 4),
                static_cast<size_t>(w * 4),
                static_cast<uint8_t*>(surface->pixels) +
                    static_cast<size_t>(row) * static_cast<size_t>(surface->pitch));
        }
        Image::saveAsPng(filename, *surface);
        SDL_DestroySurface(surface);
    }

    void RenderBackendGL::captureScreen(std::string const & filename, uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0) {
            return;
        }
        SDL_Surface* surface =
            SDL_CreateSurface(static_cast<int>(width), static_cast<int>(height), SDL_PIXELFORMAT_RGBA32);
        if (surface == nullptr) {
            return;
        }
        std::vector<uint8_t> pixels(static_cast<size_t>(width * height * 4));
        glcore_draw::readRGBA(0, 0, static_cast<int32_t>(width), static_cast<int32_t>(height), pixels.data());
        for (uint32_t row = 0; row < height; ++row) {
            std::copy_n(
                pixels.data() + static_cast<size_t>((height - 1 - row) * width * 4),
                static_cast<size_t>(width * 4),
                static_cast<uint8_t*>(surface->pixels) +
                    static_cast<size_t>(row) * static_cast<size_t>(surface->pitch));
        }
        Image::saveAsPng(filename, *surface);
        SDL_DestroySurface(surface);
    }

    void RenderBackendGL::enableScissorTest()
    {
        m_state.setScissorEnabled(true);
    }

    void RenderBackendGL::disableScissorTest()
    {
        m_state.setScissorEnabled(false);
    }

    void RenderBackendGL::attachRenderTarget(ImagePtr& img, bool discard)
    {
        if (!img) {
            detachRenderTarget();
            return;
        }
        int32_t const w = static_cast<int32_t>(img->getWidth());
        int32_t const h = static_cast<int32_t>(img->getHeight());
        if (m_fbo == 0 || discard || m_renderTarget.get() != img.get()) {
            if (m_fbo != 0) {
                glcore_draw::deleteFramebuffer(m_fbo, m_fboTexture);
            }
            m_fbo          = glcore_draw::createFramebuffer(w, h, m_fboTexture);
            m_renderTarget = img;
        }
        glcore_draw::bindFramebuffer(m_fbo, w, h);
        m_state.setProjection(0.0F, static_cast<float>(w), static_cast<float>(h), 0.0F, -100.0F, 100.0F);
    }

    void RenderBackendGL::detachRenderTarget()
    {
        if (m_fbo == 0) {
            return;
        }
        int32_t const w = static_cast<int32_t>(m_renderTarget ? m_renderTarget->getWidth() : 0);
        int32_t const h = static_cast<int32_t>(m_renderTarget ? m_renderTarget->getHeight() : 0);
        if (m_renderTarget && w > 0 && h > 0) {
            SDL_Surface* surface = m_renderTarget->getSurface();
            if (surface != nullptr && surface->pixels != nullptr) {
                std::vector<uint8_t> pixels(static_cast<size_t>(w * h * 4));
                glcore_draw::readRGBA(0, 0, w, h, pixels.data());
                for (int32_t row = 0; row < h; ++row) {
                    std::copy_n(
                        pixels.data() + static_cast<size_t>((h - 1 - row) * w * 4),
                        static_cast<size_t>(w * 4),
                        static_cast<uint8_t*>(surface->pixels) +
                            static_cast<size_t>(row) * static_cast<size_t>(surface->pitch));
                }
            }
        }
        glcore_draw::bindFramebuffer(0, 0, 0);
        if (m_window != nullptr) {
            uint32_t dw = 0;
            uint32_t dh = 0;
            glcoreGetDrawableSize(m_window, dw, dh);
            m_state.setViewport(0, 0, dw, dh);
            m_state.setProjection(0.0F, static_cast<float>(dw), static_cast<float>(dh), 0.0F, -100.0F, 100.0F);
        }
        m_renderTarget.reset();
    }

    void RenderBackendGL::renderGuiGeometry(
        std::vector<GuiVertex> const & vertices,
        std::vector<int> const & indices,
        DoublePoint const & translation,
        ImagePtr)
    {
        auto const convert = [&](int index) {
            GuiVertex const & v = vertices[static_cast<size_t>(index)];
            return PrimitiveVertex{
                static_cast<float>(v.position.x + translation.x),
                static_cast<float>(v.position.y + translation.y),
                static_cast<float>(v.color.getR()) / 255.0F,
                static_cast<float>(v.color.getG()) / 255.0F,
                static_cast<float>(v.color.getB()) / 255.0F,
                static_cast<float>(v.color.getAlpha()) / 255.0F};
        };
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            addTriangle(convert(indices[i]), convert(indices[i + 1]), convert(indices[i + 2]));
        }
    }

    void RenderBackendGL::setClipArea(Rect const & cliparea, bool)
    {
        m_state.setScissor(cliparea, true);
    }

    RenderBackendGLCore::RenderBackendGLCore(SDL_Color const & colorkey) :
        RenderBackendGL(colorkey, makeCore33Config(), "OpenGL_CORE")
    {
    }

    RenderBackendGLES2::RenderBackendGLES2(SDL_Color const & colorkey) :
        RenderBackendGL(colorkey, makeES2Config(), "GLES2")
    {
    }
} // namespace FIFE
