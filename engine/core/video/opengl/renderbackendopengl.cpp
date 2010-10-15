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

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes
#include <SDL.h>

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "video/devicecaps.h"

#include "fife_opengl.h"
#include "glimage.h"
#include "renderbackendopengl.h"
#include "SDL_image.h"

namespace FIFE {
	static Logger _log(LM_VIDEO);

	RenderBackendOpenGL::RenderBackendOpenGL(const SDL_Color& colorkey) : RenderBackend(colorkey) {
		// Get the pixelformat we want.
		SDL_Surface* testsurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 1, 1, 32,
				RMASK, GMASK, BMASK ,AMASK);

		m_rgba_format = *(testsurface->format);
		SDL_FreeSurface(testsurface);
		m_clear = false;
	}

	const std::string& RenderBackendOpenGL::getName() const {
		static std::string backend_name = "OpenGL";
		return backend_name;
	}

	RenderBackendOpenGL::~RenderBackendOpenGL() {
	}


	void RenderBackendOpenGL::init(const std::string& driver) {

		//note: driver has no affect on the opengl renderer so do nothing with it here.

		Uint32 flags = SDL_INIT_VIDEO;
		if (SDL_InitSubSystem(flags) < 0)
			throw SDLException(SDL_GetError());
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL); // temporary hack
	}

	void RenderBackendOpenGL::clearBackBuffer() {
		GLDisable flag(GL_SCISSOR_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Image* RenderBackendOpenGL::createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon){
		m_screenMode = mode;
		unsigned int width = mode.getWidth();
		unsigned int height = mode.getHeight();
		unsigned char bitsPerPixel = mode.getBPP();
		bool fs = mode.isFullScreen();
		Uint32 flags = mode.getSDLFlags();

		delete m_screen;
		m_screen = 0;

		if(icon != "") {
			SDL_Surface *img = IMG_Load(icon.c_str());
			if(img != NULL) {
				SDL_WM_SetIcon(img, 0);
			}
		}

		SDL_Surface* screen = NULL;

		if( 0 == bitsPerPixel ) {
			/// autodetect best mode
			unsigned char possibleBitsPerPixel[] = {32, 24, 16, 0};
			int i = 0;
			while( true ) {
				bitsPerPixel = possibleBitsPerPixel[i];
				if( !bitsPerPixel ) {
					throw SDLException("Videomode not available");
				}

				if ( SDL_VideoModeOK(width, height, bitsPerPixel, flags) ) {
					screen = SDL_SetVideoMode(width, height, bitsPerPixel, flags);
					if( screen ) {
						break;
					}
				}
				++i;
			}
		} else {
			if ( !SDL_VideoModeOK(width, height, bitsPerPixel, flags) ) {
				throw SDLException("Videomode not available");
			}
			screen = SDL_SetVideoMode(width, height, bitsPerPixel, flags);
		}

		FL_LOG(_log, LMsg("RenderBackendOpenGL")
			<< "Videomode " << width << "x" << height
			<< " at " << int(bitsPerPixel) << " bpp");

		//update the screen mode with the actual flags used

		m_screenMode = ScreenMode(m_screenMode.getWidth(),
		                          m_screenMode.getHeight(),
		                          m_screenMode.getBPP(),
		                          screen->flags);

		SDL_WM_SetCaption(title.c_str(), 0);

		if (!screen) {
			throw SDLException(SDL_GetError());
		}

		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0, width, height, 0);
		glMatrixMode(GL_MODELVIEW);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_SCISSOR_TEST);

		glPointSize(1.0);
		glLineWidth(1.0);

		m_screen = new GLImage(screen);
		return m_screen;
	}

	void RenderBackendOpenGL::startFrame() {
		if(m_clear) {
			clearBackBuffer();
		}
	}

	void RenderBackendOpenGL::endFrame() {
		SDL_GL_SwapBuffers();
	}

	Image* RenderBackendOpenGL::createImage(SDL_Surface* surface) {
		// Given an abritary surface, we must convert it to the format GLImage will understand.
		// It's easiest to let SDL do this for us.

		// Uh. Gotta love this :-)
		// Check for colorkey too?
		// Leave out the loss/shift checks?
		if(    m_rgba_format.BitsPerPixel == surface->format->BitsPerPixel
			&& m_rgba_format.Rmask == surface->format->Rmask
			&& m_rgba_format.Gmask == surface->format->Gmask
			&& m_rgba_format.Bmask == surface->format->Bmask
			&& m_rgba_format.Amask == surface->format->Amask
			&& m_rgba_format.Rshift == surface->format->Rshift
			&& m_rgba_format.Gshift == surface->format->Gshift
			&& m_rgba_format.Bshift == surface->format->Bshift
			&& m_rgba_format.Ashift == surface->format->Ashift
			&& m_rgba_format.Rloss == surface->format->Rloss
			&& m_rgba_format.Gloss == surface->format->Gloss
			&& m_rgba_format.Bloss == surface->format->Bloss
			&& m_rgba_format.Aloss == surface->format->Aloss
			&& surface->flags & SDL_SRCALPHA   ) {

			return new GLImage(surface);
		}

		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, SDL_SWSURFACE|SDL_SRCALPHA);
		GLImage* image = new GLImage(conv);
		SDL_FreeSurface( surface );
		return image;
	}

	Image* RenderBackendOpenGL::createImage(const uint8_t* data, unsigned int width, unsigned int height) {
		return new GLImage(data, width, height);
	}

	bool RenderBackendOpenGL::putPixel(int x, int y, int r, int g, int b, int a) {
		if ((x < 0) || (x >= (int)getWidth()) || (y < 0) || (y >= (int)getHeight())) {
			return false;
		}

		glColor4ub(r, g, b, a);

		glBegin(GL_POINTS);
		glVertex2i(x, y);
		glEnd();
		return true;
	}

	void RenderBackendOpenGL::drawLine(const Point& p1, const Point& p2, int r, int g, int b, int a) {
		glColor4ub(r, g, b, a);

		glBegin(GL_LINES);
		glVertex2f(p1.x+0.5f, p1.y+0.5f);
		glVertex2f(p2.x+0.5f, p2.y+0.5f);
		glEnd();

		glBegin(GL_POINTS);
		glVertex2f(p2.x+0.5f, p2.y+0.5f);
		glEnd();
	}

	void RenderBackendOpenGL::drawTriangle(const Point& p1, const Point& p2, const Point& p3, int r, int g, int b, int a) {
		glColor4ub(r, g, b, a);

		glBegin(GL_TRIANGLES);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glVertex2f(p3.x, p3.y);
		glEnd();
	}

	void RenderBackendOpenGL::drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		glColor4ub(r, g, b, a);

		glBegin(GL_LINE_LOOP);
		glVertex2f(p.x, p.y);
		glVertex2f(p.x+w, p.y);
		glVertex2f(p.x+w, p.y+h);
		glVertex2f(p.x, p.y+h);
		glEnd();
	}

	void RenderBackendOpenGL::fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		glColor4ub(r, g, b, a);

		glBegin(GL_QUADS);
		glVertex2f(p.x, p.y);
		glVertex2f(p.x+w, p.y);
		glVertex2f(p.x+w, p.y+h);
		glVertex2f(p.x, p.y+h);
		glEnd();
	}

	void RenderBackendOpenGL::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b, int a) {
		glColor4ub(r, g, b, a);

		glBegin(GL_QUADS);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glVertex2f(p3.x, p3.y);
		glVertex2f(p4.x, p4.y);
		glEnd();
	}

	void RenderBackendOpenGL::drawVertex(const Point& p, const uint8_t size, int r, int g, int b, int a){
		GLfloat width;
		glGetFloatv(GL_LINE_WIDTH, &width);
		glLineWidth(1.0);

		glColor4ub(r, g, b, a);

		glBegin(GL_LINE_LOOP);
		glVertex2f(p.x-size, p.y+size);
		glVertex2f(p.x+size, p.y+size);
		glVertex2f(p.x+size, p.y-size);
		glVertex2f(p.x-size, p.y-size);
		glEnd();

		glLineWidth(width);
	}
}
