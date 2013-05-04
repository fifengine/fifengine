/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

#ifndef FIFE_VIDEO_RENDERBACKENSD_OPENGL_RENDERBACKENDOPENGLE_H
#define FIFE_VIDEO_RENDERBACKENSD_OPENGL_RENDERBACKENDOPENGLE_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "video/opengl/fife_opengl.h"

namespace FIFE {
	class ScreenMode;

	/** The main class of the OpenGL-based experimental renderer.
	 * @see RenderBackend
	 */
	class RenderBackendOpenGLe : public RenderBackend {
	public:
		RenderBackendOpenGLe(const SDL_Color& colorkey);
		virtual ~RenderBackendOpenGLe();
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
		virtual void addImageToArrayZ(uint32_t id, const Rect& rec, float vertexZ, float const* st, uint8_t alpha, bool forceNewBatch, uint8_t const* rgba);
		virtual void changeRenderInfos(uint16_t elements, int32_t src, int32_t dst, bool light, bool stentest, uint8_t stenref, GLConstants stenop, GLConstants stenfunc, OverlayType otype = OVERLAY_TYPE_NONE);
		virtual void captureScreen(const std::string& filename);
		virtual void captureScreen(const std::string& filename, uint32_t width, uint32_t height);

		virtual bool putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawTriangle(const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
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

		static const float zfar;
		static const float znear;

	protected:
		virtual void setClipArea(const Rect& cliparea, bool clear);

		void enableLighting();
		void disableLighting();
		void enableStencilTest();
		void disableStencilTest();
		void setStencilTest(uint8_t stencil_ref, GLenum stencil_op, GLenum stencil_func);
		void enableAlphaTest();
		void disableAlphaTest();
		void setAlphaTest(float ref_alpha);
		void enableDepthTest();
		void disableDepthTest();
		void setEnvironmentalColor(const uint8_t* rgba);
		void setVertexPointer(GLsizei stride, const GLvoid* ptr);
		void setColorPointer(GLsizei stride, const GLvoid* ptr);
		void setTexCoordPointer(uint32_t texUnit, GLsizei stride, const GLvoid* ptr);

		void renderWithZ();
		void renderWithoutZ();

		GLuint m_mask_overlays;
		void prepareForOverlays();
		
		// only for textured quads
		struct RenderZData {
			GLfloat vertex[3];
			GLfloat texel[2];
		};

		// structure for colored overlay
		struct RenderZData2T {
			GLfloat vertex[3];
			GLfloat texel[2];
			GLfloat texel2[2];
			GLubyte color[4];
		};

		// rest (data for objects that don't utilise depth buffer)
		struct RenderData {
			GLfloat vertex[2];
			GLfloat texel[2];
			GLubyte color[4];
		};

		// describe chunk of batched (by texture) quads 
		struct RenderZObject {
			GLuint texture_id;
			uint32_t elements;
			uint32_t index;
			uint32_t max_size;
		};
		class RenderObject;

		RenderZObject* getRenderBufferObject(GLuint texture_id, bool unlit = false);

		// main source of vertex data - described by m_renderZ_objects
		std::vector<RenderZData> m_renderZ_datas;
		std::vector<RenderZObject> m_renderZ_objects;

		// vertex data source for transparent primitives - described by m_render_trans_objects
		std::vector<RenderZData2T> m_render_trans_datas;
		std::vector<RenderObject> m_render_trans_objects;

		// vertex data source for colored overlays - described by m_render_objects2T
		std::vector<RenderZData2T> m_render_datas2T;
		std::vector<RenderObject> m_render_objects2T;

		// note: vertex data comes from m_renderZ_datas
		std::vector<RenderZObject> m_renderZ_objects_forced;

		// vertex data source for primitives that dont use depth buffer - described by m_render_objects
		std::vector<RenderData> m_render_datas;
		std::vector<RenderObject> m_render_objects;

		struct currentState	{
			// Textures
			bool tex_enabled[2];
			GLuint texture[2];
			uint32_t active_tex;
			uint32_t active_client_tex;

			// Light
			uint32_t lightmodel;
			bool light_enabled;

			// Stencil
			bool sten_enabled;
			uint8_t sten_ref;
			GLint sten_buf;
			GLenum sten_op;
			GLenum sten_func;

			// The rest
			uint8_t env_color[4];
			GLenum blend_src;
			GLenum blend_dst;
			bool alpha_enabled;
			bool depth_enabled;
			bool scissor_test;
		} m_state;

		GLuint m_fbo_id;
		ImagePtr m_img_target;
		bool m_target_discard;
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
