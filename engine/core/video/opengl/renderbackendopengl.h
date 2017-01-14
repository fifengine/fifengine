/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_VIDEO_RENDERBACKENSD_OPENGL_RENDERBACKENDOPENGL_H
#define FIFE_VIDEO_RENDERBACKENSD_OPENGL_RENDERBACKENDOPENGL_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"

#include "fife_opengl.h"

namespace FIFE {
	class ScreenMode;

	/** The main class of the OpenGL-based renderer.
	 * @see RenderBackend
	 */
	class RenderBackendOpenGL : public RenderBackend {
	public:
		RenderBackendOpenGL(const SDL_Color& colorkey);
		virtual ~RenderBackendOpenGL();
		virtual const std::string& getName() const;
		virtual void startFrame();
		virtual void endFrame();
		virtual void init(const std::string& driver);
		virtual void clearBackBuffer();
		virtual void setLightingModel(uint32_t lighting);
		virtual uint32_t getLightingModel() const;
		virtual void setLighting(float red, float green, float blue);
		virtual void resetLighting();
		virtual void resetStencilBuffer(uint8_t buffer);
		virtual void changeBlending(int32_t scr, int32_t dst);

		virtual void createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon);
		virtual void setScreenMode(const ScreenMode& mode);

		virtual Image* createImage(IResourceLoader* loader = 0);
		virtual Image* createImage(const std::string& name, IResourceLoader* loader = 0);
		virtual Image* createImage(const uint8_t* data, uint32_t width, uint32_t height);
		virtual Image* createImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height);
		virtual Image* createImage(SDL_Surface* surface);
		virtual Image* createImage(const std::string& name, SDL_Surface* surface);

		virtual void renderVertexArrays();
		virtual void addImageToArray(uint32_t id, const Rect& rec, float const* st, uint8_t alpha, uint8_t const* rgba);
		virtual void addImageToArray(const Rect& rect, uint32_t id1, float const* st1, uint32_t id2, float const* st2, uint8_t alpha, uint8_t const* rgba);

		virtual void addImageToArrayZ(uint32_t id, const Rect& rect, float vertexZ, float const* st, uint8_t alpha, uint8_t const* rgba);
		virtual void addImageToArrayZ(const Rect& rect, float vertexZ, uint32_t id1, float const* st1, uint32_t id2, float const* st2, uint8_t alpha, uint8_t const* rgba);

		virtual void changeRenderInfos(RenderDataType type, uint16_t elements, int32_t src, int32_t dst, bool light, bool stentest, uint8_t stenref, GLConstants stenop, GLConstants stenfunc, OverlayType otype = OVERLAY_TYPE_NONE);

		virtual void captureScreen(const std::string& filename);
		virtual void captureScreen(const std::string& filename, uint32_t width, uint32_t height);

		virtual bool putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawThickLine(const Point& p1, const Point& p2, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawPolyLine(const std::vector<Point>& points, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawBezier(const std::vector<Point>& points, int32_t steps, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawTriangle(const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawCircle(const Point& p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawFillCircle(const Point& p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawCircleSegment(const Point& p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawFillCircleSegment(const Point& p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue);

		virtual void enableScissorTest();
		virtual void disableScissorTest();
		
		virtual void attachRenderTarget(ImagePtr& img, bool discard);
		virtual void detachRenderTarget();

		virtual void renderGuiGeometry(const std::vector<GuiVertex>& vertices, const std::vector<int>& indices, const DoublePoint& translation, ImagePtr texture);
		
		void enableTextures(uint32_t texUnit);
		void disableTextures(uint32_t texUnit);
		void bindTexture(uint32_t texUnit, GLuint texId);
		void bindTexture(GLuint textId);

	protected:
		virtual void setClipArea(const Rect& cliparea, bool clear);

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
		void setEnvironmentalColor(uint32_t texUnit, const uint8_t* rgba);
		void setVertexPointer(GLint size, GLsizei stride, const GLvoid* ptr);
		void setColorPointer(GLsizei stride, const GLvoid* ptr);
		void setTexCoordPointer(uint32_t texUnit, GLsizei stride, const GLvoid* ptr);
		
		GLuint m_maskOverlay;
		void prepareForOverlays();

		void renderWithoutZ();

		void renderWithZ();
		void renderWithZTest();
		void renderWithColorAndZ();
		void renderWithMultitextureAndZ();

		class RenderObject;

		struct RenderZObject {
			GLuint texture_id;
			//uint32_t elements;
		};

		// for regular primitives with color and alpha
		struct renderDataP {
			GLfloat vertex[2];
			GLubyte color[4];
		};

		// for regular textured quads
		struct renderDataT {
			GLfloat vertex[2];
			GLfloat texel[2];
		};

		// for textured quads with alpha
		struct renderDataTC {
			GLfloat vertex[2];
			GLfloat texel[2];
			GLubyte color[4];
		};

		// for multitextured quads
		struct renderData2TC {
			GLfloat vertex[2];
			GLfloat texel[2];
			GLfloat texel2[2];
			GLubyte color[4];
		};

		// for regular textured quads with z
		struct renderDataZ {
			GLfloat vertex[3];
			GLfloat texel[2];
		};

		// for textured quads with alpha and z
		struct renderDataColorZ {
			GLfloat vertex[3];
			GLfloat texel[2];
			GLubyte color[4];
		};

		// for multitextured quads with z
		struct renderData2TCZ {
			GLfloat vertex[3];
			GLfloat texel[2];
			GLfloat texel2[2];
			GLubyte color[4];
		};

		struct RenderZObjectTest {
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
		// vertex data source for textured quads that dont use depth buffer and color/alpha - described by m_renderObjects
		std::vector<renderDataT> m_renderTextureDatas;
		// vertex data source for textured quads with color/alpha that dont use depth buffer - described by m_renderObjects
		std::vector<renderDataTC> m_renderTextureColorDatas;
		// // vertex data source for multitexture quads that dont use depth buffer - described by m_renderObjects
		std::vector<renderData2TC> m_renderMultitextureDatas;
		std::vector<RenderObject> m_renderObjects;

		// Now the vertex data that do use the depth buffer / z
		// vertex data source for textured quads that do use depth buffer but no color/alpha - described by m_renderTextureObjectsZ
		std::vector<renderDataZ> m_renderTextureDatasZ;
		std::vector<RenderZObject> m_renderTextureObjectsZ;

		// vertex data source for textured quads that do use depth buffer and color/alpha - described by m_renderTextureColorObjectsZ
		std::vector<renderDataColorZ> m_renderTextureColorDatasZ;
		std::vector<RenderZObject> m_renderTextureColorObjectsZ;

		// vertex data source for multitexture quads that do use depth buffer - described by m_renderMultitextureObjectsZ
		std::vector<renderData2TCZ> m_renderMultitextureDatasZ;
		std::vector<RenderObject> m_renderMultitextureObjectsZ;

		// vertex data source for other stuff, unlit quads like outlines and unlit demanded instances - described by m_renderForcedObjectsZ
		std::vector<renderDataZ> m_renderForcedDatasZ;
		std::vector<RenderObject> m_renderForcedObjectsZ;

		struct currentState	{
			// Textures
			bool tex_enabled[4];
			GLuint texture[4];
			uint32_t active_tex;
			uint32_t active_client_tex;
			float alpha_test;
			GLint vertex_pointer_size;

			// Pointers
			const void* vertex_pointer;
			const void* tex_pointer[4];
			const void* color_pointer;

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
			uint8_t env_color[4];
			GLenum blend_src;
			GLenum blend_dst;
			bool alpha_enabled;
			bool scissor_test;
			bool depth_enabled;
			bool color_enabled;
		} m_state;

		GLuint m_fbo_id;
		ImagePtr m_img_target;
		bool m_target_discard;
		SDL_GLContext m_context;
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
