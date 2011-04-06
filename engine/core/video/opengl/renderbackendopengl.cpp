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
//		SDL_Surface* testsurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 1, 1, 32,
//				RMASK, GMASK, BMASK ,AMASK);

//		m_rgba_format = *(testsurface->format);
//		SDL_FreeSurface(testsurface);

		m_lightmodel = 0;
		m_light_enabled = false;
		m_stencil_enabled = false;
		m_alpha_enabled = false;
		m_sten_ref = 0;
		m_sten_buf = 0;
		m_sten_op = 0;
		m_sten_func = 0;
		m_blend_src = GL_SRC_ALPHA;
		m_blend_dst = GL_ONE_MINUS_SRC_ALPHA;
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
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL); // temporary hack

	}

	void RenderBackendOpenGL::clearBackBuffer() {
		GLDisable flag(GL_SCISSOR_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Image* RenderBackendOpenGL::createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon){
		if(icon != "") {
			SDL_Surface *img = IMG_Load(icon.c_str());
			if(img != NULL) {
				SDL_WM_SetIcon(img, 0);
			}
		}

		Image *image = setScreenMode(mode);

		SDL_WM_SetCaption(title.c_str(), 0);

		return image;
	}

	Image* RenderBackendOpenGL::setScreenMode(const ScreenMode& mode) {
		uint16_t width = mode.getWidth();
		uint16_t height = mode.getHeight();
		uint16_t bitsPerPixel = mode.getBPP();
		bool fs = mode.isFullScreen();
		uint32_t flags = mode.getSDLFlags();

		SDL_Surface* screen = NULL;

		if (bitsPerPixel != 0) {
			uint16_t bpp = SDL_VideoModeOK(width, height, bitsPerPixel, flags);
			if (!bpp){
				throw SDLException("Selected video mode not supported!");
			}
		}

		screen = SDL_SetVideoMode(width, height, bitsPerPixel, flags);
		if( !screen ) {
			throw SDLException("Unable to set video mode selected!");
		}

		FL_LOG(_log, LMsg("RenderBackendOpenGL")
			<< "Videomode " << width << "x" << height
			<< " at " << int32_t(bitsPerPixel) << " bpp");

		m_rgba_format = *(screen->format);
		m_rgba_format.Rmask = RMASK;
		m_rgba_format.Gmask = GMASK;
		m_rgba_format.Bmask = BMASK;
		m_rgba_format.Amask = AMASK;		
		
		//update the screen mode with the actual flags used
		m_screenMode = ScreenMode(width,
		                          height,
		                          bitsPerPixel,
		                          screen->flags);


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
		
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);

		glPointSize(1.0);
		glLineWidth(1.0);
		delete m_screen;

		m_screen = new GLImage(screen);
		return m_screen;
	}


	void RenderBackendOpenGL::startFrame() {
	}

	void RenderBackendOpenGL::endFrame() {
		// last chance to render (needed for gui or if no map is loaded)
		renderVertexArrays();

		SDL_GL_SwapBuffers();
	}

	Image* RenderBackendOpenGL::createImage(IResourceLoader* loader) {
		return new GLImage(loader);
	}

	Image* RenderBackendOpenGL::createImage(const std::string& name, IResourceLoader* loader) {
		return new GLImage(name, loader);
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

		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, SDL_SWSURFACE | SDL_SRCALPHA);
		GLImage* image = new GLImage(conv);
		SDL_FreeSurface( surface );
		return image;
	}

	Image* RenderBackendOpenGL::createImage(const std::string& name, SDL_Surface* surface) {
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

			return new GLImage(name, surface);
		}

		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, SDL_SWSURFACE | SDL_SRCALPHA);
		GLImage* image = new GLImage(name, conv);
		SDL_FreeSurface( surface );
		return image;
	}

	Image* RenderBackendOpenGL::createImage(const uint8_t* data, uint32_t width, uint32_t height) {
		return new GLImage(data, width, height);
	}

	Image* RenderBackendOpenGL::createImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height) {
		return new GLImage(name, data, width, height);
	}

	void RenderBackendOpenGL::setLightingModel(uint32_t lighting) {
		if (m_lightmodel != lighting) {
			if (m_lightmodel == 1) {
				disableLighting();
				glDisable(GL_COLOR_MATERIAL);
			} else if (lighting == 1) {
				enableLighting();
				glEnable(GL_LIGHT0);
				glColorMaterial(GL_FRONT, GL_DIFFUSE);
				glEnable(GL_COLOR_MATERIAL);
			}
			m_lightmodel = lighting;
		}
	}

	uint32_t RenderBackendOpenGL::getLightingModel() const {
		return m_lightmodel;
	}

	void RenderBackendOpenGL::enableLighting() {
		if (m_lightmodel == 1 && !m_light_enabled) {
			glEnable(GL_LIGHTING);
			m_light_enabled = true;
		}
	}

	void RenderBackendOpenGL::disableLighting() {
		if (m_lightmodel == 1 && m_light_enabled) {
			glDisable(GL_LIGHTING);
			m_light_enabled = false;
		}
	}

	void RenderBackendOpenGL::setLighting(float red, float green, float blue, float alpha) {
		if (m_lightmodel == 1) {
			GLfloat lightDiffuse[] = {red, green, blue, alpha};
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		} else if(m_lightmodel == 2) {
			m_lred = red;
			m_lgreen = green;
			m_lblue = blue;
			m_lalpha = alpha;
		}
	}

	void RenderBackendOpenGL::resetLighting() {
		if (m_lightmodel == 1) {
			setLighting(1.0, 1.0, 1.0, 1.0);
		} else if (m_lightmodel == 2 && m_lalpha > 0.01) {
			uint16_t width = getScreenWidth();
			uint16_t height = getScreenHeight();
			Point p = Point(0,0);
			setStencilTest(0, 0, 5);
			fillRectangle(p, width, height, m_lred*255, m_lgreen*255, m_lblue*255, m_lalpha*255);
			disableStencilTest();
		}
	}

	void RenderBackendOpenGL::enableStencilTest() {
		if (!m_stencil_enabled) {
			glEnable(GL_STENCIL_TEST);
			m_stencil_enabled = true;
		}
	}

	void RenderBackendOpenGL::disableStencilTest() {
		if (m_stencil_enabled) {
			glDisable(GL_STENCIL_TEST);
			m_stencil_enabled = false;
		}
	}

	void RenderBackendOpenGL::setStencilTest(uint8_t stencil_ref, uint32_t stencil_op, uint32_t stencil_func) {
		enableStencilTest();
		if(m_sten_op != stencil_op) {
			GLenum op;
			m_sten_op = stencil_op;
			switch(stencil_op) {
				default :
				case 0  : op = GL_KEEP; break;
				case 1  : op = GL_ZERO; break;
				case 2  : op = GL_REPLACE; break;
				case 3  : op = GL_INCR; break;
				case 4  : op = GL_DECR; break;
				case 5  : op = GL_INVERT; break;
			}
			glStencilOp(GL_KEEP, GL_KEEP, op);
		}

		if(m_sten_ref != stencil_ref || m_sten_func != stencil_func) {
			GLenum func;
			m_sten_ref = stencil_ref;
			m_sten_func = stencil_func;
			switch(stencil_func) {
				default :
				case 0  : func = GL_NEVER; break;
				case 1  : func = GL_LESS; break;
				case 2  : func = GL_LEQUAL; break;
				case 3  : func = GL_GREATER; break;
				case 4  : func = GL_GEQUAL; break;
				case 5  : func = GL_EQUAL; break;
				case 6  : func = GL_NOTEQUAL; break;
				case 7  : func = GL_ALWAYS; break;
			}
			glStencilFunc(func, stencil_ref, 0xff);
		}
	}

	void RenderBackendOpenGL::resetStencilBuffer(uint8_t buffer) {
		if (buffer != m_sten_buf) {
			m_sten_buf = buffer;
			glClearStencil(buffer);
		}
		GLDisable flag(GL_SCISSOR_TEST);
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	uint8_t RenderBackendOpenGL::getStencilRef() const {
		return m_sten_ref;
	}

	void RenderBackendOpenGL::enableAlphaTest() {
		if (!m_alpha_enabled) {
			glEnable(GL_ALPHA_TEST);
			m_alpha_enabled = true;
		}
	}

	void RenderBackendOpenGL::disableAlphaTest() {
		if (m_alpha_enabled) {
			glDisable(GL_ALPHA_TEST);
			m_alpha_enabled = false;
		}
	}

	void RenderBackendOpenGL::setAlphaTest(float ref_alpha) {
		enableAlphaTest();
		glAlphaFunc(GL_GREATER, ref_alpha);
	}

	void RenderBackendOpenGL::changeBlending(int32_t src, int32_t dst) {
		GLenum src_fact;
		GLenum dst_fact;

		switch(src) {
			case 0  : src_fact = GL_ZERO; break;
			case 1  : src_fact = GL_ONE; break;
			case 2  : src_fact = GL_DST_COLOR; break;
			case 3  : src_fact = GL_ONE_MINUS_DST_COLOR; break;
			case 4  : src_fact = GL_SRC_ALPHA; break;
			case 5  : src_fact = GL_ONE_MINUS_SRC_ALPHA; break;
			case 6  : src_fact = GL_DST_ALPHA; break;
			case 7  : src_fact = GL_ONE_MINUS_DST_ALPHA; break;

			default : src_fact = GL_DST_COLOR; break;
		}

		switch(dst) {
			case 0  : dst_fact = GL_ZERO; break;
			case 1  : dst_fact = GL_ONE; break;
			case 2  : dst_fact = GL_SRC_COLOR; break;
			case 3  : dst_fact = GL_ONE_MINUS_SRC_COLOR; break;
			case 4  : dst_fact = GL_SRC_ALPHA; break;
			case 5  : dst_fact = GL_ONE_MINUS_SRC_ALPHA; break;
			case 6  : dst_fact = GL_DST_ALPHA; break;
			case 7  : dst_fact = GL_ONE_MINUS_DST_ALPHA; break;

			default : dst_fact = GL_SRC_ALPHA; break;
		}

		if (m_blend_src != src_fact || m_blend_dst != dst_fact) {
			m_blend_src = src_fact;
			m_blend_dst = dst_fact;
			glBlendFunc(src_fact, dst_fact);
		}
	}

	void RenderBackendOpenGL::renderVertexArrays() {
		// point check
		if (!m_points.empty()) {
			renderPrimitives(m_points, 0);
			m_points.clear();
		}
		// line check
		if (!m_lines.empty()) {
			renderPrimitives(m_lines, 1);
			m_lines.clear();
		}
		// loop line check
		if (!m_looplines.empty()) {
			renderPrimitives(m_looplines, 3);
			m_looplines.clear();
		}
		// triangle check
		if (!m_triangles.empty()) {
			renderPrimitives(m_triangles, 4);
			m_triangles.clear();
		}
		// quad check
		if (!m_quads.empty()) {
			renderPrimitives(m_quads, 7);
			m_quads.clear();
		}
		// texture quad check
		if (!m_texture_quads.empty()) {
			renderTextures();
			m_texture_quads.clear();
		}
	}

	void RenderBackendOpenGL::renderPrimitives(std::vector<colorVertex>& vertice, uint8_t type) {
		const uint32_t size = vertice.size();
		const uint32_t stride = sizeof(colorVertex);

		glVertexPointer(2, GL_FLOAT, stride, &vertice[0].vertex);
		glColorPointer(4, GL_UNSIGNED_BYTE, stride, &vertice[0].color);

		GLenum mode;
		switch(type) {
			case 0  : mode = GL_POINTS; break;
			case 1  : mode = GL_LINES; break;
			case 2  : mode = GL_LINE_STRIP; break;
			case 3  : mode = GL_LINE_LOOP; break;
			case 4  : mode = GL_TRIANGLES; break;
			case 5  : mode = GL_TRIANGLE_STRIP; break;
			case 6  : mode = GL_TRIANGLE_FAN; break;
			case 7  : mode = GL_QUADS; break;
			case 8  : mode = GL_QUAD_STRIP; break;
			case 9  : mode = GL_POLYGON; break;
			default : mode = GL_QUADS;
		}
		glDrawArrays(mode, 0, size);
	}

	void RenderBackendOpenGL::renderTextures() {
		const uint32_t stride = sizeof(textureVertex);

		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(2, GL_INT, stride, &m_texture_quads[0].vertex);
		glTexCoordPointer(2, GL_FLOAT, stride, &m_texture_quads[0].texel);
		glColorPointer(4, GL_UNSIGNED_BYTE, stride, &m_texture_quads[0].color);

		int32_t index = 0;
		uint32_t old_id = 0;
		uint32_t elements = 0;
		std::vector<textureVertex>::iterator iv = m_texture_quads.begin();
		while (iv != m_texture_quads.end()) {
			if (old_id != (*iv).id) {
				if (elements > 0) {
					glDrawArrays(GL_QUADS, index, elements);
					index += elements;
				}
				elements = 4;
				old_id = (*iv).id;
				glBindTexture(GL_TEXTURE_2D, (*iv).id);
			} else {
				elements += 4;
			}
			iv += 4;
		}
		glDrawArrays(GL_QUADS, index, elements);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	bool RenderBackendOpenGL::putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if ((x < 0) || (x >= (int32_t)getWidth()) || (y < 0) || (y >= (int32_t)getHeight())) {
			return false;
		}
		colorVertex v;
		v.vertex[0] = float(x);
		v.vertex[1] = float(y);
		v.color[0] = r;
		v.color[1] = g;
		v.color[2] = b;
		v.color[3] = a;
		m_points.push_back(v);

		return true;
	}

	void RenderBackendOpenGL::drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		colorVertex v;
		v.vertex[0] = p1.x;
		v.vertex[1] = p1.y;
		v.color[0] = r;
		v.color[1] = g;
		v.color[2] = b;
		v.color[3] = a;
		m_lines.push_back(v);

		v.vertex[0] = p2.x;
		v.vertex[1] = p2.y;
		m_lines.push_back(v);
	}

	void RenderBackendOpenGL::drawTriangle(const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		colorVertex v;
		v.vertex[0] = p1.x;
		v.vertex[1] = p1.y;
		v.color[0] = r;
		v.color[1] = g;
		v.color[2] = b;
		v.color[3] = a;
		m_triangles.push_back(v);

		v.vertex[0] = p2.x;
		v.vertex[1] = p2.y;
		m_triangles.push_back(v);

		v.vertex[0] = p3.x;
		v.vertex[1] = p3.y;
		m_triangles.push_back(v);
	}

	void RenderBackendOpenGL::drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		colorVertex v;
		v.vertex[0] = p.x;
		v.vertex[1]= p.y;
		v.color[0] = r;
		v.color[1] = g;
		v.color[2] = b;
		v.color[3] = a;
		m_looplines.push_back(v);

		v.vertex[0] = p.x+w;
		m_looplines.push_back(v);

		v.vertex[1] = p.y+h;
		m_looplines.push_back(v);

		v.vertex[0] = p.x;
		m_looplines.push_back(v);
	}

	void RenderBackendOpenGL::fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		colorVertex v;
		v.vertex[0] = p.x;
		v.vertex[1] = p.y;
		v.color[0] = r;
		v.color[1] = g;
		v.color[2] = b;
		v.color[3] = a;
		m_quads.push_back(v);

		v.vertex[0] = p.x+w;
		m_quads.push_back(v);

		v.vertex[1] = p.y+h;
		m_quads.push_back(v);

		v.vertex[0] = p.x;
		m_quads.push_back(v);
	}

	void RenderBackendOpenGL::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		colorVertex v;
		v.vertex[0] = p1.x;
		v.vertex[1] = p1.y;
		v.color[0] = r;
		v.color[1] = g;
		v.color[2] = b;
		v.color[3] = a;
		m_quads.push_back(v);

		v.vertex[0] = p2.x;
		v.vertex[1] = p2.y;
		m_quads.push_back(v);

		v.vertex[0] = p3.x;
		v.vertex[1] = p3.y;
		m_quads.push_back(v);

		v.vertex[0] = p4.x;
		v.vertex[1] = p4.y;
		m_quads.push_back(v);
	}

	void RenderBackendOpenGL::drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
		colorVertex v;
		v.vertex[0] = p.x-size;
		v.vertex[1] = p.y+size;
		v.color[0] = r;
		v.color[1] = g;
		v.color[2] = b;
		v.color[3] = a;
		m_looplines.push_back(v);

		v.vertex[0] = p.x+size;
		m_looplines.push_back(v);

		v.vertex[1] = p.y-size;
		m_looplines.push_back(v);

		v.vertex[0] = p.x-size;
		m_looplines.push_back(v);
	}

	void RenderBackendOpenGL::drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue) {
		uint8_t alpha = intensity;
		if (m_lightmodel != 2) {
			alpha = 255;
		}
		const float step = Mathf::twoPi()/subdivisions;
		colorVertex v;
		for(float angle=0; angle<=Mathf::twoPi(); angle+=step){
			v.vertex[0] = p.x;
			v.vertex[1] = p.y;
			v.color[0] = red;
			v.color[1] = green;
			v.color[2] = blue;
			v.color[3] = intensity;
			m_triangles.push_back(v);

			v.vertex[0] = radius*Mathf::Cos(angle)*xstretch + p.x;
			v.vertex[1] = radius*Mathf::Sin(angle)*ystretch + p.y;
			v.color[0] = 0;
			v.color[1] = 0;
			v.color[2] = 0;
			v.color[3] = alpha;
			m_triangles.push_back(v);

			v.vertex[0] = radius*Mathf::Cos(angle+step)*xstretch + p.x;
			v.vertex[1] = radius*Mathf::Sin(angle+step)*ystretch + p.y;
			m_triangles.push_back(v);
		}
	}

	void RenderBackendOpenGL::addImageToArray(uint32_t& id, Rect& rec, float& rt, float& ct, uint8_t& alpha) {
		textureVertex v;

		v.vertex[0] = rec.x;
		v.vertex[1] = rec.y;
		v.texel[0] = 0.0;
		v.texel[1] = 0.0;
		v.color[0] = 255;
		v.color[1] = 255;
		v.color[2] = 255;
		v.color[3] = alpha;
		v.id = id;
		m_texture_quads.push_back(v);
		
		v.vertex[0] = rec.x;
		v.vertex[1] = rec.y+rec.h;
		v.texel[1] = rt;
		m_texture_quads.push_back(v);
		
		v.vertex[0] = rec.x+rec.w;
		v.vertex[1] = rec.y+rec.h;
		v.texel[0] = ct;
		m_texture_quads.push_back(v);
		
		v.vertex[0] = rec.x+rec.w;
		v.vertex[1] = rec.y;
		v.texel[1] = 0.0;
		m_texture_quads.push_back(v);
	}
}
