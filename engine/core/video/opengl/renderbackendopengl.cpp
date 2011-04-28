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

	class RenderObject {
	public:
		RenderObject(GLenum m, uint16_t s, uint32_t t=0):
			mode(m),
			size(s),
			texture_id(t),
			src(4),
			dst(5),
			light(true),
			stencil_test(false),
			stencil_ref(0),
			stencil_op(0),
			stencil_func(0) {}

		GLenum mode;
		uint16_t size;
		uint32_t texture_id;
		int32_t src;
		int32_t dst;
		bool light;
		bool stencil_test;
		uint8_t stencil_ref;
		GLenum stencil_op;
		GLenum stencil_func;
	};

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
		RenderBackend::startFrame();
	}

	void RenderBackendOpenGL::endFrame() {
		SDL_GL_SwapBuffers();
		RenderBackend::endFrame();
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
			fillRectangle(p, width, height,
				static_cast<uint8_t>(m_lred*255.0f),
				static_cast<uint8_t>(m_lgreen*255.0f),
				static_cast<uint8_t>(m_lblue*255.0f),
				static_cast<uint8_t>(m_lalpha*255.0f));
			changeRenderInfos(1, 4, 5, false, true, 0, KEEP, EQUAL);
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

	void RenderBackendOpenGL::setStencilTest(uint8_t stencil_ref, GLenum stencil_op, GLenum stencil_func) {
		enableStencilTest();
		if(m_sten_op != stencil_op) {
			m_sten_op = stencil_op;
			glStencilOp(GL_KEEP, GL_KEEP, m_sten_op);
		}

		if(m_sten_ref != stencil_ref || m_sten_func != stencil_func) {
			m_sten_ref = stencil_ref;
			m_sten_func = stencil_func;
			glStencilFunc(m_sten_func, stencil_ref, 0xff);
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

	void RenderBackendOpenGL::changeRenderInfos(uint16_t elements, int32_t src, int32_t dst, bool light,
		bool stentest, uint8_t stenref, GLConstants stenop, GLConstants stenfunc) {

		uint16_t count = 0;
		uint32_t size = m_render_objects.size();
		while (count != elements) {
			++count;
			RenderObject& r = m_render_objects.at(size-count);

			r.src = src;
			r.dst = dst;
			r.light = light;
			if (stentest) {
				r.stencil_test = stentest;
				r.stencil_ref = stenref;
				r.stencil_op = stenop;
				r.stencil_func = stenfunc;
			}
		}
	}

	void RenderBackendOpenGL::renderVertexArrays() {
		if (!m_render_objects.empty()) {
			//bools to indicate changes
			bool type = false;
			bool texture = false;
			bool blending = false;
			bool light = false;
			bool stencil = false;
			bool render = false;

			//stride
			const uint32_t stride = sizeof(renderData);
			
			//set pointer
			glVertexPointer(2, GL_FLOAT, stride, &m_render_datas[0].vertex);
			glTexCoordPointer(2, GL_FLOAT, stride, &m_render_datas[0].texel);
			glColorPointer(4, GL_UNSIGNED_BYTE, stride, &m_render_datas[0].color);

			// array index
			int32_t index = 0;
			// elements to render
			uint32_t elements = 0;
			// render mode
			GLenum mode = GL_QUADS;
			// texture id
			uint32_t texture_id = 0;
			// src blending mode
			int32_t src = 4;
			// dst blending mode
			int32_t dst = 5;

			for(std::vector<RenderObject>::iterator ir = m_render_objects.begin(); ir != m_render_objects.end(); ir++) {
				RenderObject& ro = (*ir);

				//first we look for changes
				if (ro.mode != mode) {
					type = true;
					render = true;
				}
				if (ro.texture_id != texture_id) {
					texture = true;
					render = true;
				}
				if (m_lightmodel != 0) {
					if (ro.src != src || ro.dst != dst) {
						blending = true;
						render = true;
					}
					if (ro.light != m_light_enabled && m_lightmodel == 1) {
						light = true;
						render = true;
					}
					if (ro.stencil_test != m_stencil_enabled) {
						stencil = true;
						render = true;
					} else if (ro.stencil_test) {
						if (ro.stencil_ref != m_sten_ref || ro.stencil_op != m_sten_op || ro.stencil_func != m_sten_func) {
							stencil = true;
							render = true;
						}
					}
				}

				// if changes then we render all previously elements
				if (render) {
					if (elements > 0) {
						//render
						glDrawArrays(mode, index, elements);
						index += elements;
					}
					// switch mode
					if (type) {
						mode = ro.mode;
						type = false;
					}
					// set element to current size
					elements = ro.size;

					// switch texturing
					if (texture) {
						if (ro.texture_id != 0) {
							glEnable(GL_TEXTURE_2D);
							glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							glBindTexture(GL_TEXTURE_2D, ro.texture_id);
							texture_id = ro.texture_id;
						} else {
							glDisableClientState(GL_TEXTURE_COORD_ARRAY);
							glDisable(GL_TEXTURE_2D);
							texture_id = 0;
						}
						texture = false;
					}

					// if lighting is enabled we have to consider a few more values
					if (m_lightmodel != 0) {
						// change blending
						if (blending) {
							src = ro.src;
							dst = ro.dst;
							changeBlending(ro.src, ro.dst);
							blending = false;
						}
						// change light
						if (light) {
							if (ro.light && !m_light_enabled) {
								enableLighting();
							} else if (!ro.light && m_light_enabled) {
								disableLighting();
							}
							light = false;
						}
						// change stencil
						if (stencil) {
							if (ro.stencil_test) {
								setStencilTest(ro.stencil_ref, ro.stencil_op, ro.stencil_func);
								setAlphaTest(0.0);
							} else {
								disableAlphaTest();
								disableStencilTest();
							}
							stencil = false;
						}
					}
					render = false;
				} else {
					// else add the element
					elements += ro.size;
				}
			}
			// render
			glDrawArrays(mode, index, elements);

			//reset all states
			if (texture_id != 0) {
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisable(GL_TEXTURE_2D);
			}
			if (m_lightmodel != 0) {
				changeBlending(4, 5);
				disableLighting();
				disableStencilTest();
				disableAlphaTest();
			}

			m_render_objects.clear();
			m_render_datas.clear();
		}
	}

	bool RenderBackendOpenGL::putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if ((x < 0) || (x >= (int32_t)getWidth()) || (y < 0) || (y >= (int32_t)getHeight())) {
			return false;
		}
		renderData rd;
		rd.vertex[0] = static_cast<float>(x);
		rd.vertex[1] = static_cast<float>(y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_render_datas.push_back(rd);

		RenderObject ro(GL_POINTS, 1);
		m_render_objects.push_back(ro);

		return true;
	}

	void RenderBackendOpenGL::drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderData rd;
		rd.vertex[0] = static_cast<float>(p1.x);
		rd.vertex[1] = static_cast<float>(p1.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p2.x);
		rd.vertex[1] = static_cast<float>(p2.y);
		m_render_datas.push_back(rd);

		RenderObject ro(GL_LINES, 2);
		m_render_objects.push_back(ro);
	}

	void RenderBackendOpenGL::drawTriangle(const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderData rd;
		rd.vertex[0] = static_cast<float>(p1.x);
		rd.vertex[1] = static_cast<float>(p1.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p2.x);
		rd.vertex[1] = static_cast<float>(p2.y);
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p3.x);
		rd.vertex[1] = static_cast<float>(p3.y);
		m_render_datas.push_back(rd);

		RenderObject ro(GL_TRIANGLES, 3);
		m_render_objects.push_back(ro);
	}

	void RenderBackendOpenGL::drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderData rd;
		rd.vertex[0] = static_cast<float>(p.x);
		rd.vertex[1] = static_cast<float>(p.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x+w);
		m_render_datas.push_back(rd);

		rd.vertex[1] = static_cast<float>(p.y+h);
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x);
		m_render_datas.push_back(rd);

		RenderObject ro(GL_LINE_LOOP, 4);
		m_render_objects.push_back(ro);
	}

	void RenderBackendOpenGL::fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderData rd;
		rd.vertex[0] = static_cast<float>(p.x);
		rd.vertex[1] = static_cast<float>(p.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x+w);
		m_render_datas.push_back(rd);

		rd.vertex[1] = static_cast<float>(p.y+h);
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x);
		m_render_datas.push_back(rd);

		RenderObject ro(GL_QUADS, 4);
		m_render_objects.push_back(ro);
	}

	void RenderBackendOpenGL::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderData rd;
		rd.vertex[0] = static_cast<float>(p1.x);
		rd.vertex[1] = static_cast<float>(p1.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p2.x);
		rd.vertex[1] = static_cast<float>(p2.y);
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p3.x);
		rd.vertex[1] = static_cast<float>(p3.y);
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p4.x);
		rd.vertex[1] = static_cast<float>(p4.y);
		m_render_datas.push_back(rd);

		RenderObject ro(GL_QUADS, 4);
		m_render_objects.push_back(ro);
	}

	void RenderBackendOpenGL::drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
		renderData rd;
		rd.vertex[0] = static_cast<float>(p.x-size);
		rd.vertex[1] = static_cast<float>(p.y+size);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x+size);
		m_render_datas.push_back(rd);

		rd.vertex[1] = static_cast<float>(p.y-size);
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x-size);
		m_render_datas.push_back(rd);

		RenderObject ro(GL_LINE_LOOP, 4);
		m_render_objects.push_back(ro);
	}

	void RenderBackendOpenGL::drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue) {
		uint8_t alpha = intensity;
		if (m_lightmodel != 2) {
			alpha = 255;
		}
		const float step = Mathf::twoPi()/subdivisions;
		renderData rd;;
		for(float angle=0; angle<=Mathf::twoPi(); angle+=step){
			rd.vertex[0] = static_cast<float>(p.x);
			rd.vertex[1] = static_cast<float>(p.y);
			rd.color[0] = red;
			rd.color[1] = green;
			rd.color[2] = blue;
			rd.color[3] = intensity;
			m_render_datas.push_back(rd);

			rd.vertex[0] = radius*Mathf::Cos(angle)*xstretch + p.x;
			rd.vertex[1] = radius*Mathf::Sin(angle)*ystretch + p.y;
			rd.color[0] = 0;
			rd.color[1] = 0;
			rd.color[2] = 0;
			rd.color[3] = alpha;
			m_render_datas.push_back(rd);

			rd.vertex[0] = radius*Mathf::Cos(angle+step)*xstretch + p.x;
			rd.vertex[1] = radius*Mathf::Sin(angle+step)*ystretch + p.y;
			m_render_datas.push_back(rd);

			RenderObject ro(GL_TRIANGLES, 3);
			m_render_objects.push_back(ro);
		}
	}

	void RenderBackendOpenGL::addImageToArray(uint32_t& id, Rect& rec, float& rt, float& ct, uint8_t& alpha) {
		renderData rd;
		rd.vertex[0] = static_cast<float>(rec.x);
		rd.vertex[1] = static_cast<float>(rec.y);
		rd.texel[0] = 0.0;
		rd.texel[1] = 0.0;
		rd.color[0] = 255;
		rd.color[1] = 255;
		rd.color[2] = 255;
		rd.color[3] = alpha;
		m_render_datas.push_back(rd);
		
		rd.vertex[0] = static_cast<float>(rec.x);
		rd.vertex[1] = static_cast<float>(rec.y+rec.h);
		rd.texel[1] = rt;
		m_render_datas.push_back(rd);
		
		rd.vertex[0] = static_cast<float>(rec.x+rec.w);
		rd.vertex[1] = static_cast<float>(rec.y+rec.h);
		rd.texel[0] = ct;
		m_render_datas.push_back(rd);
		
		rd.vertex[0] = static_cast<float>(rec.x+rec.w);
		rd.vertex[1] = static_cast<float>(rec.y);
		rd.texel[1] = 0.0;
		m_render_datas.push_back(rd);

		RenderObject ro(GL_QUADS, 4, id);
		m_render_objects.push_back(ro);
	}
}
