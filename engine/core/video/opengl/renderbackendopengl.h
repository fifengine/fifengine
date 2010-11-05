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

namespace FIFE {

	class ScreenMode;

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
		void setLightingModel(unsigned int lighting);
		unsigned int getLightingModel() const;
		void enableLighting();
		void disableLighting();
		void setLighting(float red, float green, float blue, float alpha);
		void resetLighting();
		void enableStencilTest();
		void disableStencilTest();
		void setStencilTest(uint8_t stencil_ref, unsigned int stencil_op, unsigned int stencil_func);
		void resetStencilBuffer(uint8_t buffer);
		uint8_t getStencilRef() const;
		void enableAlphaTest();
		void disableAlphaTest();
		void setAlphaTest(float ref_alpha);
		void changeBlending(int scr, int dst);

		Image* createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon);
		Image*  setScreenMode(const ScreenMode& mode);
		Image* createImage(const uint8_t* data, unsigned int width, unsigned int height);
		Image* createImage(SDL_Surface* surface);
 		bool putPixel(int x, int y, int r, int g, int b, int a = 255);
		void drawLine(const Point& p1, const Point& p2, int r, int g, int b, int a = 255);
		void drawTriangle(const Point& p1, const Point& p2, const Point& p3, int r, int g, int b, int a = 255);
		void drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b, int a = 255);
		void drawVertex(const Point& p, const uint8_t size, int r, int g, int b, int a = 255);
		void drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue);

	private:
		SDL_PixelFormat m_rgba_format;

		unsigned int m_lightmodel;
		float m_lred;
		float m_lgreen;
		float m_lblue;
		float m_lalpha;
		bool m_light_enabled;
		bool m_stencil_enabled;
		bool m_alpha_enabled;
		uint8_t m_sten_ref;
		GLint m_sten_buf;
		unsigned int m_sten_op;
		unsigned int m_sten_func;
		GLenum m_blend_src;
		GLenum m_blend_dst;
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
