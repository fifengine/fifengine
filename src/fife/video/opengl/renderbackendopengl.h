// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_RENDERBACKENSD_OPENGL_RENDERBACKENDOPENGL_H
#define FIFE_VIDEO_RENDERBACKENSD_OPENGL_RENDERBACKENDOPENGL_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <array>
#include <memory>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "fife_opengl.h"
#include "video/renderbackend.h"

namespace FIFE
{
    class ScreenMode;

    /** The main class of the OpenGL-based renderer.
     * @see RenderBackend
     */
    class FIFE_API RenderBackendOpenGL : public RenderBackend
    {
        public:
            explicit RenderBackendOpenGL(SDL_Color const & colorkey);
            virtual ~RenderBackendOpenGL() override;

            RenderBackendOpenGL(RenderBackendOpenGL const &)            = delete;
            RenderBackendOpenGL& operator=(RenderBackendOpenGL const &) = delete;

            virtual std::string const & getName() const override;
            virtual void endFrame() override;
            virtual void init(std::string const & driver) override;
            virtual void clearBackBuffer() override;
            virtual void setLightingModel(uint32_t lighting) override;
            virtual uint32_t getLightingModel() const override;
            virtual void setLighting(float red, float green, float blue) override;
            virtual void resetLighting() override;
            virtual void resetStencilBuffer(uint8_t buffer) override;
            virtual void changeBlending(int32_t src, int32_t dst) override;

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
            virtual void addImageToArray(
                Rect const & rect,
                uint32_t id1,
                float const * st1,
                uint32_t id2,
                float const * st2,
                uint8_t alpha,
                uint8_t const * rgba);

            virtual void addImageToArrayZ(
                uint32_t id, Rect const & rect, float vertexZ, float const * st, uint8_t alpha, uint8_t const * rgba);
            virtual void addImageToArrayZ(
                Rect const & rect,
                float vertexZ,
                uint32_t id1,
                float const * st1,
                uint32_t id2,
                float const * st2,
                uint8_t alpha,
                uint8_t const * rgba);

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

            void enableTextures(uint32_t texUnit);
            void disableTextures(uint32_t texUnit);
            void bindTexture(uint32_t texUnit, GLuint texId);
            void bindTexture(GLuint texId);

        protected:
            virtual void setClipArea(Rect const & cliparea, bool clear) override;

            void enableLighting();
            void disableLighting();
            void enableStencilTest();
            void disableStencilTest();
            void setStencilTest(uint8_t stencil_ref, GLenum stencil_op, GLenum stencil_func);
            uint8_t getStencilRef() const;
            void enableAlphaTest();
            void disableAlphaTest();
            void setAlphaTest(float ref_alpha);
            void enableDepthTest();
            void disableDepthTest();
            void enableColorArray();
            void disableColorArray();
            void setEnvironmentalColor(uint32_t texUnit, uint8_t const * rgba);
            void setVertexPointer(GLint size, GLsizei stride, GLvoid const * ptr);
            void setColorPointer(GLsizei stride, GLvoid const * ptr);
            void setTexCoordPointer(uint32_t texUnit, GLsizei stride, GLvoid const * ptr);

            GLuint m_maskOverlay;
            void prepareForOverlays();

            void renderWithoutZ();

            void renderWithZ();
            void renderWithZTest();
            void renderWithColorAndZ();
            void renderWithMultitextureAndZ();

            class RenderObject;

            struct FIFE_API RenderZObject
            {
                    GLuint texture_id;
                    // uint32_t elements;
            };

            // for regular primitives with color and alpha
            struct FIFE_API renderDataP
            {
                    std::array<GLfloat, 2> vertex;
                    std::array<GLubyte, 4> color;
            };

            // for regular textured quads
            struct FIFE_API renderDataT
            {
                    std::array<GLfloat, 2> vertex;
                    std::array<GLfloat, 2> texel;
            };

            // for textured quads with alpha
            struct FIFE_API renderDataTC
            {
                    std::array<GLfloat, 2> vertex;
                    std::array<GLfloat, 2> texel;
                    std::array<GLubyte, 4> color;
            };

            // for multitextured quads
            struct FIFE_API renderData2TC
            {
                    std::array<GLfloat, 2> vertex;
                    std::array<GLfloat, 2> texel;
                    std::array<GLfloat, 2> texel2;
                    std::array<GLubyte, 4> color;
            };

            // for regular textured quads with z
            struct FIFE_API renderDataZ
            {
                    std::array<GLfloat, 3> vertex;
                    std::array<GLfloat, 2> texel;
            };

            // for textured quads with alpha and z
            struct FIFE_API renderDataColorZ
            {
                    std::array<GLfloat, 3> vertex;
                    std::array<GLfloat, 2> texel;
                    std::array<GLubyte, 4> color;
            };

            // for multitextured quads with z
            struct FIFE_API renderData2TCZ
            {
                    std::array<GLfloat, 3> vertex;
                    std::array<GLfloat, 2> texel;
                    std::array<GLfloat, 2> texel2;
                    std::array<GLubyte, 4> color;
            };

            struct FIFE_API RenderZObjectTest
            {
                    GLuint texture_id;
                    uint32_t elements;
                    uint32_t index;
                    uint32_t max_size;
            };
            RenderZObjectTest* getRenderBufferObject(GLuint texture_id);
            std::vector<renderDataZ> m_renderZ_datas;
            std::vector<RenderZObjectTest> m_renderZ_objects;

            // Vertex data structs that dont use the depth buffer / z
            // vertex data source for primitives that dont use depth buffer and texture - described by m_renderObjects
            std::vector<renderDataP> m_renderPrimitiveDatas;
            // vertex data source for textured quads that dont use depth buffer and color/alpha - described by
            // m_renderObjects
            std::vector<renderDataT> m_renderTextureDatas;
            // vertex data source for textured quads with color/alpha that dont use depth buffer - described by
            // m_renderObjects
            std::vector<renderDataTC> m_renderTextureColorDatas;
            // // vertex data source for multitexture quads that dont use depth buffer - described by m_renderObjects
            std::vector<renderData2TC> m_renderMultitextureDatas;
            std::vector<RenderObject> m_renderObjects;

            // Dynamic index buffers
            std::vector<uint32_t> m_pIndices;
            std::vector<uint32_t> m_tIndices;
            std::vector<uint32_t> m_tcIndices;
            std::vector<uint32_t> m_tc2Indices;

            // Now the vertex data that do use the depth buffer / z
            // vertex data source for textured quads that do use depth buffer but no color/alpha - described by
            // m_renderTextureObjectsZ
            std::vector<renderDataZ> m_renderTextureDatasZ;
            std::vector<RenderZObject> m_renderTextureObjectsZ;

            // vertex data source for textured quads that do use depth buffer and color/alpha - described by
            // m_renderTextureColorObjectsZ
            std::vector<renderDataColorZ> m_renderTextureColorDatasZ;
            std::vector<RenderZObject> m_renderTextureColorObjectsZ;

            // vertex data source for multitexture quads that do use depth buffer - described by
            // m_renderMultitextureObjectsZ
            std::vector<renderData2TCZ> m_renderMultitextureDatasZ;
            std::vector<RenderObject> m_renderMultitextureObjectsZ;

            // vertex data source for other stuff, unlit quads like outlines and unlit demanded instances - described by
            // m_renderForcedObjectsZ
            std::vector<renderDataZ> m_renderForcedDatasZ;
            std::vector<RenderObject> m_renderForcedObjectsZ;

            struct FIFE_API currentState
            {
                    // Textures
                    std::array<bool, 4> tex_enabled{};
                    std::array<GLuint, 4> texture{};
                    uint32_t active_tex;
                    uint32_t active_client_tex;
                    float alpha_test;
                    GLint vertex_pointer_size;

                    // Pointers
                    void const * vertex_pointer;
                    std::array<void const *, 4> tex_pointer{};
                    void const * color_pointer;

                    // Stencil
                    bool sten_enabled;
                    uint8_t sten_ref;
                    GLint sten_buf;
                    GLenum sten_op;
                    GLenum sten_func;

                    // Light
                    uint32_t lightmodel;
                    bool light_enabled;

                    // The rest
                    std::array<uint8_t, 4> env_color{};
                    GLenum blend_src;
                    GLenum blend_dst;
                    bool alpha_enabled;
                    bool scissor_test;
                    bool depth_enabled;
                    bool color_enabled;
            } m_state;

            GLuint m_fbo_id;
            GLuint m_indicebufferId;
            //! static indices for vertex data with z
            std::vector<uint32_t> m_indices;
            ImagePtr m_img_target;
            bool m_target_discard;
            SDL_GLContext m_context;
    };

} // namespace FIFE

#endif
