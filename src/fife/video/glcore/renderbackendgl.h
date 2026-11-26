// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_RENDERBACKENDGL_H
#define FIFE_RENDERBACKENDGL_H

#include <memory>
#include <string>
#include <vector>

#include "platform.h"
#include "video/dummy/renderbackenddummy.h"
#include "video/glcore/glcontext.h"
#include "video/glcore/glcorestate.h"
#include "video/glcore/shaderloader.h"

namespace FIFE
{
    /** Shared shader-based backend shell. Inherits the safe no-op defaults of
     * RenderBackendDummy for primitives that need no special handling yet.
     */
    class FIFE_API RenderBackendGL : public RenderBackendDummy
    {
        public:
            RenderBackendGL(SDL_Color const & colorkey, GLContextConfig config, std::string name);
            ~RenderBackendGL() override;

            std::string const & getName() const override
            {
                return m_name;
            }

            std::string getRenderDescription() const override;

            void init(std::string const & driver) override;
            void createMainScreen(std::string const & title, std::string const & icon) override;
            void endFrame() override;
            void clearBackBuffer() override;
            void setScalingMode(ScalingMode mode) override;
            void setLightingModel(uint32_t lighting) override;
            uint32_t getLightingModel() const override;
            void setLighting(float red, float green, float blue) override;
            void resetLighting() override;
            void resetStencilBuffer(uint8_t buffer) override;
            void changeBlending(int32_t src, int32_t dst) override;

            std::unique_ptr<Image> createImage(IResourceLoader* loader = nullptr) override;
            std::unique_ptr<Image> createImage(std::string const & name, IResourceLoader* loader = nullptr) override;
            std::unique_ptr<Image> createImage(uint8_t const * data, uint32_t width, uint32_t height) override;
            std::unique_ptr<Image> createImage(
                std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) override;
            std::unique_ptr<Image> createImage(SDL_Surface* surface) override;
            std::unique_ptr<Image> createImage(std::string const & name, SDL_Surface* surface) override;

            void renderVertexArrays() override;
            void addImageToArray(
                Rect const & rect,
                uint32_t id1,
                float const * st1,
                uint32_t id2,
                float const * st2,
                uint8_t alpha,
                uint8_t const * rgba) override;
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

            void enableScissorTest() override;
            void disableScissorTest() override;

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

            uint32_t debugBlendSrc() const
            {
                return m_stateDebugBlendSrc;
            }
            uint32_t debugBlendDst() const
            {
                return m_stateDebugBlendDst;
            }
            bool debugStencilEnabled() const
            {
                return m_stateDebugStencil;
            }
            OverlayType debugOverlayType() const
            {
                return m_overlayType;
            }

            float const * debugProjection() const
            {
                return m_state.projection();
            }

            size_t bufferedPrimitiveVertices() const
            {
                return m_primitiveVertices.size();
            }

            void prepareForOverlays();

            void attachRenderTarget(ImagePtr& img, bool discard) override;
            void detachRenderTarget() override;
            void renderGuiGeometry(
                std::vector<GuiVertex> const & vertices,
                std::vector<int> const & indices,
                DoublePoint const & translation,
                ImagePtr texture) override;

        protected:
            void setClipArea(Rect const & cliparea, bool clear) override;

            std::string m_name;
            SDL_Window* m_window{nullptr};
            GLContext m_context;
            GLCoreState m_state;
            std::unique_ptr<ShaderLoader> m_shaders;
            uint32_t m_lightingModel{0};
            RenderDataType m_renderDataType{RENDER_DATA_WITHOUT_Z};
            OverlayType m_overlayType{OVERLAY_TYPE_NONE};
            bool m_lightEnabled{false};
            uint32_t m_stateDebugBlendSrc{0};
            uint32_t m_stateDebugBlendDst{0};
            bool m_stateDebugStencil{false};
            uint32_t m_fbo{0};
            uint32_t m_fboTexture{0};
            ImagePtr m_renderTarget;

            struct PrimitiveVertex
            {
                    float x;
                    float y;
                    float r;
                    float g;
                    float b;
                    float a;
            };
            struct ImageBatch
            {
                    uint32_t texture{0};
                    uint32_t secondTexture{0};
                    uint32_t overlayMode{0};
                    std::vector<float> vertices;
            };
            std::vector<ImageBatch> m_imageBatches;
            std::vector<PrimitiveVertex> m_primitiveVertices;

            void addTriangle(PrimitiveVertex const & a, PrimitiveVertex const & b, PrimitiveVertex const & c);
            void addQuad(
                PrimitiveVertex const & a,
                PrimitiveVertex const & b,
                PrimitiveVertex const & c,
                PrimitiveVertex const & d);
            void addTriangle(
                Point const & p1,
                Point const & p2,
                Point const & p3,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a,
                bool filled);
    };

    class FIFE_API RenderBackendGLCore final : public RenderBackendGL
    {
        public:
            explicit RenderBackendGLCore(SDL_Color const & colorkey);
    };

    class FIFE_API RenderBackendGLES2 final : public RenderBackendGL
    {
        public:
            explicit RenderBackendGLES2(SDL_Color const & colorkey);
    };
} // namespace FIFE

#endif
