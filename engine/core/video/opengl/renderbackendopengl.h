/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
	class RenderObject;

	/** The main class of the OpenGL-based renderer.
	 * @see RenderBackend
	 */
	class RenderBackendOpenGL : public RenderBackend {
	public:
		RenderBackendOpenGL(const SDL_Color& colorkey);
		virtual ~RenderBackendOpenGL();
		const std::string& getName() const;
		void startFrame();
		void endFrame();
		void init(const std::string& driver);
		void clearBackBuffer();
		void setLightingModel(uint32_t lighting);
		uint32_t getLightingModel() const;
		void setLighting(float red, float green, float blue, float alpha);
		void resetLighting();
		void resetStencilBuffer(uint8_t buffer);
		void changeBlending(int32_t scr, int32_t dst);

		Image* createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon);
		Image* setScreenMode(const ScreenMode& mode);
		Image* createImage(IResourceLoader* loader = 0);
		Image* createImage(const std::string& name, IResourceLoader* loader = 0);
		Image* createImage(const uint8_t* data, uint32_t width, uint32_t height);
		Image* createImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height);
		Image* createImage(SDL_Surface* surface);
		Image* createImage(const std::string& name, SDL_Surface* surface);
		bool putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void drawTriangle(const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue);

		void renderVertexArrays();
		void addImageToArray(uint32_t& id, Rect& rec, float& rt, float& ct, uint8_t& alpha);
		void changeRenderInfos(uint16_t elements, int32_t src, int32_t dst, bool light, bool stentest, uint8_t stenref, GLConstants stenop, GLConstants stenfunc);

	private:
		void enableLighting();
		void disableLighting();
		void enableStencilTest();
		void disableStencilTest();
		void setStencilTest(uint8_t stencil_ref, GLenum stencil_op, GLenum stencil_func);
		uint8_t getStencilRef() const;
		void enableAlphaTest();
		void disableAlphaTest();
		void setAlphaTest(float ref_alpha);

		struct renderData {
			GLfloat vertex[2];
			GLfloat texel[2];
			GLubyte color[4];
		};
		std::vector<renderData> m_render_datas;
		std::vector<RenderObject> m_render_objects;

		uint32_t m_lightmodel;
		float m_lred;
		float m_lgreen;
		float m_lblue;
		float m_lalpha;
		bool m_light_enabled;
		bool m_stencil_enabled;
		bool m_alpha_enabled;
		uint8_t m_sten_ref;
		GLint m_sten_buf;
		GLenum m_sten_op;
		GLenum m_sten_func;
		GLenum m_blend_src;
		GLenum m_blend_dst;
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
