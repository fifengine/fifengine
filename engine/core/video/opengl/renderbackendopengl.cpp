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

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes
#include <SDL.h>

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "video/devicecaps.h"

#include "glimage.h"
#include "renderbackendopengl.h"
#include "SDL_image.h"


namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIDEO);

	class RenderBackendOpenGL::RenderObject {
	public:
		RenderObject(GLenum m, uint16_t s, uint32_t t1=0, uint32_t t2=0):
			mode(m),
			size(s),
			texture_id(t1),
			overlay_id(t2),
			src(4),
			dst(5),
			light(true),
			stencil_test(false),
			overlay_type(OVERLAY_TYPE_NONE),
			stencil_ref(0),
			stencil_op(0),
			stencil_func(0) {}

		GLenum mode;
		uint16_t size;
		uint32_t texture_id;
		uint32_t overlay_id;
		int32_t src;
		int32_t dst;
		bool light;
		bool stencil_test;
		OverlayType overlay_type;
		uint8_t stencil_ref;
		GLenum stencil_op;
		GLenum stencil_func;
		uint8_t rgba[4];
	};

	RenderBackendOpenGL::RenderBackendOpenGL(const SDL_Color& colorkey)
		: RenderBackend(colorkey), m_maskOverlay(0){

		m_state.tex_enabled[0] = false;
		m_state.tex_enabled[1] = false;
		m_state.tex_enabled[2] = false;
		m_state.tex_enabled[3] = false;
		m_state.texture[0] = 0;
		m_state.texture[1] = 0;
		m_state.texture[2] = 0;
		m_state.texture[3] = 0;
		m_state.active_tex = 0;

		m_state.color_pointer = 0;
		m_state.tex_pointer[0] = 0;
		m_state.tex_pointer[1] = 0;
		m_state.tex_pointer[2] = 0;
		m_state.tex_pointer[3] = 0;
		m_state.vertex_pointer = 0;

		m_state.sten_enabled = false;
		m_state.sten_ref = 0;
		m_state.sten_buf = 0;
		m_state.sten_op = 0;
		m_state.sten_func = 0;

		m_state.lightmodel = 0;
		m_state.light_enabled = false;

		m_state.env_color[0] = 0;
		m_state.env_color[1] = 0;
		m_state.env_color[2] = 0;
		m_state.env_color[3] = 0;
		m_state.blend_src = GL_SRC_ALPHA;
		m_state.blend_dst = GL_ONE_MINUS_SRC_ALPHA;
		m_state.alpha_enabled = false;
		m_state.scissor_test = true;
	}

	RenderBackendOpenGL::~RenderBackendOpenGL() {
		glDeleteTextures(1, &m_maskOverlay);
		if(GLEE_EXT_framebuffer_object && m_useframebuffer) {
			glDeleteFramebuffers(1, &m_fbo_id);
		}
		deinit();
	}

	const std::string& RenderBackendOpenGL::getName() const {
		static std::string backend_name = "OpenGL";
		return backend_name;
	}

	void RenderBackendOpenGL::init(const std::string& driver) {
		// note: driver has no affect on the opengl renderer so do nothing with it here.
		Uint32 flags = SDL_INIT_VIDEO;
		if (SDL_InitSubSystem(flags) < 0)
			throw SDLException(SDL_GetError());
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL); // temporary hack
	}

	void RenderBackendOpenGL::clearBackBuffer() {
		disableScissorTest();
		glClear(GL_COLOR_BUFFER_BIT);
		enableScissorTest();
	}

	void RenderBackendOpenGL::createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon){
		if(icon != "") {
			SDL_Surface *img = IMG_Load(icon.c_str());
			if(img != NULL) {
				SDL_WM_SetIcon(img, 0);
				SDL_FreeSurface(img);
			}
		}
		
		SDL_WM_SetCaption(title.c_str(), 0);
		setScreenMode(mode);
	}

	void RenderBackendOpenGL::setScreenMode(const ScreenMode& mode) {
		uint16_t width = mode.getWidth();
		uint16_t height = mode.getHeight();
		uint16_t bitsPerPixel = mode.getBPP();
		uint32_t flags = mode.getSDLFlags();

		if (bitsPerPixel != 0) {
			uint16_t bpp = SDL_VideoModeOK(width, height, bitsPerPixel, flags);
			if (!bpp){
				throw SDLException("Selected video mode not supported!");
			}
		}

		if(m_screen) {
			SDL_FreeSurface(m_screen);
		}
		m_screen = SDL_SetVideoMode(width, height, bitsPerPixel, flags);
		if( !m_screen ) {
			throw SDLException("Unable to set video mode selected!");
		}
		m_target = m_screen;

		FL_LOG(_log, LMsg("RenderBackendOpenGL")
			<< "Videomode " << width << "x" << height
			<< " at " << int32_t(bitsPerPixel) << " bpp");

		m_rgba_format = *(m_screen->format);
		m_rgba_format.Rmask = RMASK;
		m_rgba_format.Gmask = GMASK;
		m_rgba_format.Bmask = BMASK;
		m_rgba_format.Amask = AMASK;

		//update the screen mode with the actual flags used
		m_screenMode = ScreenMode(width,
		                          height,
		                          bitsPerPixel,
		                          m_screen->flags);

		if (!m_screen) {
			throw SDLException(SDL_GetError());
		}

		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_SCISSOR_TEST);

		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);

		prepareForOverlays();

		glPointSize(1.0);
		glLineWidth(1.0);

		if(GLEE_EXT_framebuffer_object && m_useframebuffer) {
			glGenFramebuffers(1, &m_fbo_id);
		}
		
		if (m_textureFilter == TEXTURE_FILTER_ANISOTROPIC) {
			if (GLEE_EXT_texture_filter_anisotropic) {
				GLint largest = 0;
				glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
				m_maxAnisotropy = static_cast<int32_t>(largest);
			} else {
				// if not available use trilinear filter
				m_maxAnisotropy = 0;
				m_textureFilter = TEXTURE_FILTER_TRILINEAR;
			}
		}
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
		if (32 == surface->format->BitsPerPixel
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

		uint8_t bpp = m_rgba_format.BitsPerPixel;
		m_rgba_format.BitsPerPixel = 32;
		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, SDL_SWSURFACE | SDL_SRCALPHA);
		m_rgba_format.BitsPerPixel = bpp;
		GLImage* image = new GLImage(conv);

		SDL_FreeSurface(surface);
		return image;
	}

	Image* RenderBackendOpenGL::createImage(const std::string& name, SDL_Surface* surface) {
		// Given an abritary surface, we must convert it to the format GLImage will understand.
		// It's easiest to let SDL do this for us.

		// Uh. Gotta love this :-)
		// Check for colorkey too?
		// Leave out the loss/shift checks?
		if (32 == surface->format->BitsPerPixel
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

		uint8_t bpp = m_rgba_format.BitsPerPixel;
		m_rgba_format.BitsPerPixel = 32;
		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, SDL_SWSURFACE | SDL_SRCALPHA);
		m_rgba_format.BitsPerPixel = bpp;
		GLImage* image = new GLImage(name, conv);

		SDL_FreeSurface(surface);
		return image;
	}

	Image* RenderBackendOpenGL::createImage(const uint8_t* data, uint32_t width, uint32_t height) {
		return new GLImage(data, width, height);
	}

	Image* RenderBackendOpenGL::createImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height) {
		return new GLImage(name, data, width, height);
	}

	void RenderBackendOpenGL::setLightingModel(uint32_t lighting) {
		if (m_state.lightmodel != lighting) {
			if (m_state.lightmodel != 0) {
				disableLighting();
				glDisable(GL_COLOR_MATERIAL);
			} else if (lighting != 0) {
				enableLighting();
				glEnable(GL_LIGHT0);
				glColorMaterial(GL_FRONT, GL_DIFFUSE);
				glEnable(GL_COLOR_MATERIAL);
			}
			m_state.lightmodel = lighting;
		}
	}

	uint32_t RenderBackendOpenGL::getLightingModel() const {
		return m_state.lightmodel;
	}

	void RenderBackendOpenGL::enableTextures(uint32_t texUnit) {
		if(m_state.tex_enabled[texUnit] == false) {
			if(m_state.active_tex != texUnit) {
				m_state.active_tex = texUnit;
				glActiveTexture(GL_TEXTURE0 + texUnit);
			}
			if (m_state.active_client_tex != texUnit) {
				m_state.active_client_tex = texUnit;
				glClientActiveTexture(GL_TEXTURE0 + texUnit);
			}
			m_state.tex_enabled[texUnit] = true;

			glEnable(GL_TEXTURE_2D);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}

	void RenderBackendOpenGL::disableTextures(uint32_t texUnit) {
		if(m_state.tex_enabled[texUnit] == true) {
			if(m_state.active_tex != texUnit) {
				m_state.active_tex = texUnit;
				glActiveTexture(GL_TEXTURE0 + texUnit);
			}
			if (m_state.active_client_tex != texUnit) {
				m_state.active_client_tex = texUnit;
				glClientActiveTexture(GL_TEXTURE0 + texUnit);
			}
			m_state.tex_enabled[texUnit] = false;

			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}

	void RenderBackendOpenGL::bindTexture(uint32_t texUnit, GLuint texId) {
		enableTextures(texUnit);

		if(m_state.texture[texUnit] != texId) {
			if(m_state.active_tex != texUnit) {
				m_state.active_tex = texUnit;
				glActiveTexture(GL_TEXTURE0 + texUnit);
			}
			if (m_state.active_client_tex != texUnit) {
				m_state.active_client_tex = texUnit;
				glClientActiveTexture(GL_TEXTURE0 + texUnit);
			}
			m_state.texture[texUnit] = texId;
			glBindTexture(GL_TEXTURE_2D, texId);
		}
	}

	void RenderBackendOpenGL::bindTexture(GLuint texId) {
		if(m_state.texture[m_state.active_tex] != texId) {
			m_state.texture[m_state.active_tex] = texId;
			glBindTexture(GL_TEXTURE_2D, texId);
		}
	}

	void RenderBackendOpenGL::enableLighting() {
		if (m_state.lightmodel != 0 && !m_state.light_enabled) {
			glEnable(GL_LIGHTING);
			m_state.light_enabled = true;
		}
	}

	void RenderBackendOpenGL::disableLighting() {
		if (m_state.lightmodel != 0 && m_state.light_enabled) {
			glDisable(GL_LIGHTING);
			m_state.light_enabled = false;
		}
	}

	void RenderBackendOpenGL::setLighting(float red, float green, float blue) {
		if (m_state.lightmodel != 0) {
			GLfloat lightDiffuse[] = {red, green, blue, 1.0f};
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		}
	}

	void RenderBackendOpenGL::resetLighting() {
		if (m_state.lightmodel != 0) {
			setLighting(1.0, 1.0, 1.0);
		}
	}

	void RenderBackendOpenGL::enableStencilTest() {
		if (!m_state.sten_enabled) {
			glEnable(GL_STENCIL_TEST);
			m_state.sten_enabled = true;
		}
	}

	void RenderBackendOpenGL::disableStencilTest() {
		if (m_state.sten_enabled) {
			glDisable(GL_STENCIL_TEST);
			m_state.sten_enabled = false;
		}
	}

	void RenderBackendOpenGL::setStencilTest(uint8_t stencil_ref, GLenum stencil_op, GLenum stencil_func) {
		enableStencilTest();
		if(m_state.sten_op != stencil_op) {
			m_state.sten_op = stencil_op;
			glStencilOp(GL_KEEP, GL_KEEP, m_state.sten_op);
		}

		if(m_state.sten_ref != stencil_ref || m_state.sten_func != stencil_func) {
			m_state.sten_ref = stencil_ref;
			m_state.sten_func = stencil_func;
			glStencilFunc(m_state.sten_func, stencil_ref, 0xff);
		}
	}

	void RenderBackendOpenGL::resetStencilBuffer(uint8_t buffer) {
		if (buffer != m_state.sten_buf) {
			m_state.sten_buf = buffer;
			glClearStencil(buffer);
		}
		disableScissorTest();
		glClear(GL_STENCIL_BUFFER_BIT);
		enableScissorTest();
	}

	uint8_t RenderBackendOpenGL::getStencilRef() const {
		return m_state.sten_ref;
	}

	void RenderBackendOpenGL::enableAlphaTest() {
		if (!m_state.alpha_enabled) {
			glEnable(GL_ALPHA_TEST);
			m_state.alpha_enabled = true;
		}
	}

	void RenderBackendOpenGL::disableAlphaTest() {
		if (m_state.alpha_enabled) {
			glDisable(GL_ALPHA_TEST);
			m_state.alpha_enabled = false;
		}
	}

	void RenderBackendOpenGL::setAlphaTest(float ref_alpha) {
		enableAlphaTest();
		glAlphaFunc(GL_GREATER, ref_alpha);
	}

	void RenderBackendOpenGL::setEnvironmentalColor(uint32_t texUnit, const uint8_t* rgba) {
		if (memcmp(m_state.env_color, rgba, sizeof(uint8_t) * 4) || m_state.active_tex != texUnit) {

			memcpy(m_state.env_color, rgba, sizeof(uint8_t) * 4);
			GLfloat rgbaf[4] = {
				static_cast<float>(m_state.env_color[0]) / 255.0f,
				static_cast<float>(m_state.env_color[1]) / 255.0f,
				static_cast<float>(m_state.env_color[2]) / 255.0f,
				static_cast<float>(m_state.env_color[3]) / 255.0f};

			if(m_state.active_tex != texUnit) {
				m_state.active_tex = texUnit;
				glActiveTexture(GL_TEXTURE0 + texUnit);
				m_state.active_client_tex = texUnit;
				glClientActiveTexture(GL_TEXTURE0 + texUnit);
			}

			glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, rgbaf);
		}
	}

	void RenderBackendOpenGL::setVertexPointer(GLsizei stride, const GLvoid* ptr) {
		if(m_state.vertex_pointer != ptr)	{
			m_state.vertex_pointer = ptr;
			glVertexPointer(2, GL_FLOAT, stride, ptr);
		}
	}

	void RenderBackendOpenGL::setColorPointer(GLsizei stride, const GLvoid* ptr) {
		if(m_state.color_pointer != ptr) {
			m_state.color_pointer = ptr;
			glColorPointer(4, GL_UNSIGNED_BYTE, stride, ptr);
		}
	}

	void RenderBackendOpenGL::setTexCoordPointer(uint32_t texUnit, GLsizei stride, const GLvoid* ptr) {
		if(m_state.tex_pointer[texUnit] != ptr) {
			if(m_state.active_tex != texUnit) {
				m_state.active_tex = texUnit;
				glActiveTexture(GL_TEXTURE0 + texUnit);
			}
			if (m_state.active_client_tex != texUnit) {
				m_state.active_client_tex = texUnit;
				glClientActiveTexture(GL_TEXTURE0 + texUnit);
			}
			m_state.tex_pointer[texUnit] = ptr;
			glTexCoordPointer(2, GL_FLOAT, stride, ptr);
		}
	}

	void RenderBackendOpenGL::enableScissorTest() {
		if(m_state.scissor_test == false) {
			m_state.scissor_test = true;
			glEnable(GL_SCISSOR_TEST);
		}
	}

	void RenderBackendOpenGL::disableScissorTest() {
		if(m_state.scissor_test == true)	{
			m_state.scissor_test = false;
			glDisable(GL_SCISSOR_TEST);
		}
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

		if (m_state.blend_src != src_fact || m_state.blend_dst != dst_fact) {
			m_state.blend_src = src_fact;
			m_state.blend_dst = dst_fact;
			glBlendFunc(src_fact, dst_fact);
		}
	}

	void RenderBackendOpenGL::changeRenderInfos(uint16_t elements, int32_t src, int32_t dst, bool light,
		bool stentest, uint8_t stenref, GLConstants stenop, GLConstants stenfunc, OverlayType otype) {

		uint16_t count = 0;
		uint32_t size = m_render_objects.size();
		while (count != elements) {
			++count;
			RenderObject& r = m_render_objects.at(size-count);

			r.src = src;
			r.dst = dst;
			r.light = light;
			r.overlay_type = otype;
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
			bool mt = false;

			//stride
			const uint32_t stride = sizeof(renderData);
			const uint32_t stride2T = sizeof(renderData2T);

			if (m_render_objects[0].overlay_type == OVERLAY_TYPE_NONE) {
				//set pointer
				setVertexPointer(stride, &m_render_datas[0].vertex);
				setTexCoordPointer(0, stride, &m_render_datas[0].texel);
				setColorPointer(stride, &m_render_datas[0].color);
			}

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
			// overlay type
			OverlayType overlay_type = OVERLAY_TYPE_NONE;
			// overlay color
			uint8_t color[4] = {0};

			int32_t index2T = 0;
			uint32_t elements2T = 0;

			int32_t* currentIndex = &index;
			uint32_t* currentElements = &elements;

			for(std::vector<RenderObject>::iterator ir = m_render_objects.begin(); ir != m_render_objects.end(); ++ir) {
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
				if (m_state.lightmodel != 0) {
					if (ro.src != src || ro.dst != dst) {
						blending = true;
						render = true;
					}
					if (ro.light != m_state.light_enabled) {
						light = true;
						render = true;
					}
					if (ro.stencil_test != m_state.sten_enabled) {
						stencil = true;
						render = true;
					} else if (ro.stencil_test) {
						if (ro.stencil_ref != m_state.sten_ref ||
							ro.stencil_op != m_state.sten_op ||
							ro.stencil_func != m_state.sten_func) {
							stencil = true;
							render = true;
						}
					}
				}
				if (ro.overlay_type != overlay_type ||
					(ro.overlay_type != OVERLAY_TYPE_NONE && memcmp(color, ro.rgba, sizeof(uint8_t) * 4))) {
					mt = true;
					render = true;
				}

				// if changes then we render all previously elements
				if (render) {
					if (*currentElements > 0) {
						//render
						glDrawArrays(mode, *currentIndex, *currentElements);
						*currentIndex += *currentElements;
					}
					// switch mode
					if (type) {
						mode = ro.mode;
						type = false;
					}
					// multitexturing
					if(mt) {
						switch (ro.overlay_type) {
						case OVERLAY_TYPE_NONE:
							disableTextures(3);
							disableTextures(2);
							disableTextures(1);
							enableTextures(0);

							// set pointers
							setVertexPointer(stride, &m_render_datas[0].vertex);
							setTexCoordPointer(0, stride, &m_render_datas[0].texel);
							setColorPointer(stride, &m_render_datas[0].color);

							currentIndex = &index;
							currentElements = &elements;
							break;
						case OVERLAY_TYPE_COLOR:
							disableTextures(3);
							disableTextures(2);
							bindTexture(1, m_maskOverlay);
							setEnvironmentalColor(1, ro.rgba);
							enableTextures(0);

							// set pointers
							setVertexPointer(stride, &m_render_datas[0].vertex);
							setColorPointer(stride, &m_render_datas[0].color);
							setTexCoordPointer(1, stride, &m_render_datas[0].texel);
							setTexCoordPointer(0, stride, &m_render_datas[0].texel);

							memcpy(color, ro.rgba, sizeof(uint8_t) * 4);
							currentElements = &elements;
							currentIndex = &index;
							break;
						case OVERLAY_TYPE_COLOR_AND_TEXTURE:
							disableTextures(3);
							disableTextures(1);
							bindTexture(2, ro.overlay_id);
							setEnvironmentalColor(2, ro.rgba);
							enableTextures(0);

							// set pointers
							setVertexPointer(stride2T, &m_render_datas2T[0].vertex);
							setColorPointer(stride2T, &m_render_datas2T[0].color);
							setTexCoordPointer(2, stride2T, &m_render_datas2T[0].texel2);
							setTexCoordPointer(0, stride2T, &m_render_datas2T[0].texel);

							memcpy(color, ro.rgba, sizeof(uint8_t) * 4);
							currentElements = &elements2T;
							currentIndex = &index2T;
							break;
						case OVERLAY_TYPE_TEXTURES_AND_FACTOR:
							disableTextures(2);
							disableTextures(1);
							bindTexture(3, ro.overlay_id);
							setEnvironmentalColor(3, ro.rgba);
							enableTextures(0);

							// set pointers
							setVertexPointer(stride2T, &m_render_datas2T[0].vertex);
							setColorPointer(stride2T, &m_render_datas2T[0].color);
							setTexCoordPointer(3, stride2T, &m_render_datas2T[0].texel2);
							setTexCoordPointer(0, stride2T, &m_render_datas2T[0].texel);

							memcpy(color, ro.rgba, sizeof(uint8_t) * 4);
							currentElements = &elements2T;
							currentIndex = &index2T;
							break;
						}
						overlay_type = ro.overlay_type;
						mt = false;
					}

					// switch texturing
					if (texture) {
						if (ro.texture_id != 0) {
							bindTexture(0, ro.texture_id);
							texture_id = ro.texture_id;
						} else {
							disableTextures(0);
							texture_id = 0;
						}
						texture = false;
					}

					// set element to current size
					*currentElements = ro.size;

					// if lighting is enabled we have to consider a few more values
					if (m_state.lightmodel != 0) {
						// change blending
						if (blending) {
							src = ro.src;
							dst = ro.dst;
							changeBlending(ro.src, ro.dst);
							blending = false;
						}
						// change light
						if (light) {
							if (ro.light && !m_state.light_enabled) {
								enableLighting();
							} else if (!ro.light && m_state.light_enabled) {
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
					*currentElements += ro.size;
				}
			}
			// render
			glDrawArrays(mode, *currentIndex, *currentElements);

			//reset all states
			if (overlay_type != OVERLAY_TYPE_NONE) {
				disableTextures(3);
				disableTextures(2);
				disableTextures(1);
			}
			disableTextures(0);

			if (m_state.lightmodel != 0) {
				changeBlending(4, 5);
				disableLighting();
				disableStencilTest();
				disableAlphaTest();
			}

			m_render_objects.clear();
			m_render_datas.clear();
			m_render_datas2T.clear();
		}
	}

	bool RenderBackendOpenGL::putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if ((x < 0) || (x >= (int32_t)m_target->w) ||
			(y < 0) || (y >= (int32_t)m_target->h)) {
			return false;
		}
		renderData rd;
		rd.vertex[0] = static_cast<float>(x)+0.375;
		rd.vertex[1] = static_cast<float>(y)+0.375;
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
		rd.vertex[0] = static_cast<float>(p1.x)-0.375;
		rd.vertex[1] = static_cast<float>(p1.y)-0.375;
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p2.x)-0.375;
		rd.vertex[1] = static_cast<float>(p2.y)-0.375;
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

		rd.vertex[1] = static_cast<float>(p.y+h);
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x+w);
		m_render_datas.push_back(rd);

		rd.vertex[1] = static_cast<float>(p.y);
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

	void RenderBackendOpenGL::drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
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

			rd.vertex[0] = radius*Mathf::Cos(angle+step)*xstretch + p.x;
			rd.vertex[1] = radius*Mathf::Sin(angle+step)*ystretch + p.y;
			rd.color[0] = 0;
			rd.color[1] = 0;
			rd.color[2] = 0;
			rd.color[3] = 255;
			m_render_datas.push_back(rd);

			rd.vertex[0] = radius*Mathf::Cos(angle)*xstretch + p.x;
			rd.vertex[1] = radius*Mathf::Sin(angle)*ystretch + p.y;
			m_render_datas.push_back(rd);

			RenderObject ro(GL_TRIANGLES, 3);
			m_render_objects.push_back(ro);
		}
	}

	void RenderBackendOpenGL::addImageToArray(uint32_t id, const Rect& rect, float const* st, uint8_t alpha, uint8_t const* rgba) {
		renderData rd;
		rd.vertex[0] = static_cast<float>(rect.x);
		rd.vertex[1] = static_cast<float>(rect.y);
		rd.texel[0] = st[0];
		rd.texel[1] = st[1];
		rd.color[0] = 255;
		rd.color[1] = 255;
		rd.color[2] = 255;
		rd.color[3] = alpha;
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(rect.x);
		rd.vertex[1] = static_cast<float>(rect.y+rect.h);
		rd.texel[1] = st[3];
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(rect.x+rect.w);
		rd.vertex[1] = static_cast<float>(rect.y+rect.h);
		rd.texel[0] = st[2];
		m_render_datas.push_back(rd);

		rd.vertex[0] = static_cast<float>(rect.x+rect.w);
		rd.vertex[1] = static_cast<float>(rect.y);
		rd.texel[1] = st[1];
		m_render_datas.push_back(rd);

		RenderObject ro(GL_QUADS, 4, id);
		if (rgba) {
			ro.overlay_type = OVERLAY_TYPE_COLOR;
			ro.rgba[0] = rgba[0];
			ro.rgba[1] = rgba[1];
			ro.rgba[2] = rgba[2];
			ro.rgba[3] = rgba[3];
		}
		m_render_objects.push_back(ro);
	}

	void RenderBackendOpenGL::addImageToArray(const Rect& rect, uint32_t id1, float const* st1, uint32_t id2, float const* st2, uint8_t alpha, uint8_t const* rgba) {
		if (rgba) {
			renderData2T rd;
			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.texel[0] = st1[0];
			rd.texel[1] = st1[1];
			rd.texel2[0] = st2[0];
			rd.texel2[1] = st2[1];
			rd.color[0] = 255;
			rd.color[1] = 255;
			rd.color[2] = 255;
			rd.color[3] = alpha;
			m_render_datas2T.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[1] = st1[3];
			rd.texel2[1] = st2[3];
			m_render_datas2T.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[0] = st1[2];
			rd.texel2[0] = st2[2];
			m_render_datas2T.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.texel[1] = st1[1];
			rd.texel2[1] = st2[1];
			m_render_datas2T.push_back(rd);

			RenderObject ro(GL_QUADS, 4, id1, id2);
			ro.overlay_type = OVERLAY_TYPE_TEXTURES_AND_FACTOR;
			ro.rgba[0] = rgba[0];
			ro.rgba[1] = rgba[1];
			ro.rgba[2] = rgba[2];
			ro.rgba[3] = rgba[3];
			m_render_objects.push_back(ro);
		}
	}

	void RenderBackendOpenGL::prepareForOverlays() {
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);

		if(m_maskOverlay == 0) {
			// Constant texture - can be constant across every tilesets
			glGenTextures(1, &m_maskOverlay);

			uint8_t dummydata[3] = {127, 127, 127};
			glBindTexture(GL_TEXTURE_2D, m_maskOverlay);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0,
				GL_RGB, GL_UNSIGNED_BYTE, dummydata);
		} else {
			glBindTexture(GL_TEXTURE_2D, m_maskOverlay);
		}

		m_state.texture[1] = m_maskOverlay;

		// Texture Unit 1
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

		// Arg0
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE0);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE0);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

		// The alpha component is taken only from 0th tex unit which is
		// Arg0 in our case, therefore we doesn't need to set operands
		// and sources for the rest of arguments

		// Arg1
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

		// Arg2
		// uses alpha part of environmental color as interpolation factor
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);

		glDisable(GL_TEXTURE_2D);
		
		// Texture Unit 2
		glClientActiveTexture(GL_TEXTURE2);
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

		// Arg0
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE0);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE2);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

		// The alpha component is taken only from 0th tex unit which is
		// Arg0 in our case, therefore we doesn't need to set operands
		// and sources for the rest of arguments

		// Arg1
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

		// Arg2
		// uses alpha part of environmental color as interpolation factor
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);

		glDisable(GL_TEXTURE_2D);

		// Texture Unit 3
		glClientActiveTexture(GL_TEXTURE3);
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

		// Arg0
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE0);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE3);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

		// The alpha component is taken only from 3th tex unit which is
		// Arg0 in our case, therefore we doesn't need to set operands
		// and sources for the rest of arguments

		// Arg1
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE3);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

		// Arg2
		// uses alpha part of environmental color as interpolation factor
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);

		// Return to normal sampling mode
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		m_state.active_tex = 0;
		glClientActiveTexture(GL_TEXTURE0);
		m_state.active_client_tex = 0;

		// For now it's unneecessary - Only needed if we intend to use the next texture unit in different case
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	void RenderBackendOpenGL::captureScreen(const std::string& filename) {
		const uint32_t swidth = getWidth();
		const uint32_t sheight = getHeight();

		uint8_t *pixels;
		SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, swidth, sheight, 24,
			RMASK, GMASK, BMASK, NULLMASK);

		if (!surface) {
			return;
		}

		SDL_LockSurface(surface);
		pixels = new uint8_t[swidth * sheight * 3];
		glReadPixels(0, 0, swidth, sheight, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(pixels));
		uint8_t *imagepixels = reinterpret_cast<uint8_t*>(surface->pixels);
		// Copy the "reversed_image" memory to the "image" memory
		for (int32_t y = (sheight - 1); y >= 0; --y) {
			uint8_t *rowbegin = pixels + y * swidth * 3;
			uint8_t *rowend = rowbegin + swidth * 3;

			std::copy(rowbegin, rowend, imagepixels);

			// Advance a row in the output surface.
			imagepixels += surface->pitch;
		}

		SDL_UnlockSurface(surface);
		Image::saveAsPng(filename, *surface);

		SDL_FreeSurface(surface);
		delete[] pixels;
	}

	void RenderBackendOpenGL::captureScreen(const std::string& filename, uint32_t width, uint32_t height) {
		const uint32_t swidth = getWidth();
		const uint32_t sheight = getHeight();
		const bool same_size = (width == swidth && height == sheight);

		if (width < 1 || height < 1) {
			return;
		}

		if (same_size) {
			captureScreen(filename);
			return;
		}

		uint8_t *pixels;
		// create source surface
		SDL_Surface* src = SDL_CreateRGBSurface(SDL_SWSURFACE, swidth, sheight, 32,
			RMASK, GMASK, BMASK, AMASK);

		if (!src) {
			return;
		}

		if (SDL_MUSTLOCK(src)) {
			SDL_LockSurface(src);
		}
		pixels = new uint8_t[swidth * sheight * 4];
		glReadPixels(0, 0, swidth, sheight, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(pixels));

		uint8_t* imagepixels = reinterpret_cast<uint8_t*>(src->pixels);
		// Copy the "reversed_image" memory to the "image" memory
		for (int32_t y = (sheight - 1); y >= 0; --y) {
			uint8_t *rowbegin = pixels + y * swidth * 4;
			uint8_t *rowend = rowbegin + swidth * 4;

			std::copy(rowbegin, rowend, imagepixels);

			// Advance a row in the output surface.
			imagepixels += src->pitch;
		}

		// create destination surface
		SDL_Surface* dst = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
			RMASK, GMASK, BMASK, AMASK);

		uint32_t* src_pointer = static_cast<uint32_t*>(src->pixels);
		uint32_t* src_help_pointer = src_pointer;
		uint32_t* dst_pointer = static_cast<uint32_t*>(dst->pixels);

		int32_t x, y, *sx_ca, *sy_ca;
		int32_t sx = static_cast<int32_t>(0xffff * src->w / dst->w);
		int32_t sy = static_cast<int32_t>(0xffff * src->h / dst->h);
		int32_t sx_c = 0;
		int32_t sy_c = 0;

		// Allocates memory and calculates row wide&height
		int32_t* sx_a = new int32_t[dst->w + 1];
		sx_ca = sx_a;
		for (x = 0; x <= dst->w; x++) {
			*sx_ca = sx_c;
			sx_ca++;
			sx_c &= 0xffff;
			sx_c += sx;
		}

		int32_t* sy_a = new int32_t[dst->h + 1];
		sy_ca = sy_a;
		for (y = 0; y <= dst->h; y++) {
			*sy_ca = sy_c;
			sy_ca++;
			sy_c &= 0xffff;
			sy_c += sy;
		}
		sy_ca = sy_a;

		// Transfers the image data

		if (SDL_MUSTLOCK(dst)) {
			SDL_LockSurface(dst);
		}

		for (y = 0; y < dst->h; y++) {
			src_pointer = src_help_pointer;
			sx_ca = sx_a;
			for (x = 0; x < dst->w; x++) {
				*dst_pointer = *src_pointer;
				sx_ca++;
				src_pointer += (*sx_ca >> 16);
				dst_pointer++;
			}
			sy_ca++;
			src_help_pointer = (uint32_t*)((uint8_t*)src_help_pointer + (*sy_ca >> 16) * src->pitch);
		}

		if (SDL_MUSTLOCK(dst)) {
			SDL_UnlockSurface(dst);
		}
		if (SDL_MUSTLOCK(src)) {
			SDL_UnlockSurface(src);
		}

		Image::saveAsPng(filename, *dst);

		// Free memory
		SDL_FreeSurface(src);
		SDL_FreeSurface(dst);
		delete[] sx_a;
		delete[] sy_a;
		delete[] pixels;
	}

	void RenderBackendOpenGL::setClipArea(const Rect& cliparea, bool clear) {
		glScissor(cliparea.x, getHeight() - cliparea.y - cliparea.h, cliparea.w, cliparea.h);
		if (clear) {
			if (m_isbackgroundcolor) {
				float red = float(m_backgroundcolor.r/255.0);
				float green = float(m_backgroundcolor.g/255.0);
				float blue = float(m_backgroundcolor.b/255.0);
				glClearColor(red, green, blue, 0.0);
				m_isbackgroundcolor = false;
			}
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}

	void RenderBackendOpenGL::attachRenderTarget(ImagePtr& img, bool discard) {
		// flush down what we batched for the old target
		renderVertexArrays();

		m_img_target = img;
		m_target_discard = discard;

		// to render on something, we need to make sure its loaded already in gpu memory
		m_img_target->forceLoadInternal();
		m_target = m_img_target->getSurface();

		GLImage* glimage = static_cast<GLImage*>(m_img_target.get());

		GLuint targetid = glimage->getTexId();
		uint32_t w = m_img_target->getWidth();
		uint32_t h = m_img_target->getHeight();

		// quick & dirty hack for attaching compressed texture
		if(glimage->isCompressed()) {
			bindTexture(targetid);
			GLubyte* pixels = new GLubyte[w*h*4];
			// here we get decompressed pixels
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			delete [] pixels;
			glimage->setCompressed(false);
		}

		// can we use fbo?
		if (GLEE_EXT_framebuffer_object && m_useframebuffer) {
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo_id);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
				GL_TEXTURE_2D, targetid, 0);
		}

		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		// invert top with bottom
		glOrtho(0, w, 0, h, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		// because of inversion 2 lines above we need to also invert culling faces
		glCullFace(GL_FRONT);

		if (m_target_discard) {
			glClear(GL_COLOR_BUFFER_BIT);
		} else if (!m_target_discard && (!GLEE_EXT_framebuffer_object || !m_useframebuffer)) {
			// if we wanna just add something to render target, we need to first render previous contents
			addImageToArray(targetid, m_img_target->getArea(),
				static_cast<GLImage*>(m_img_target.get())->getTexCoords(), 255, 0);
		}
	}

	void RenderBackendOpenGL::detachRenderTarget() {
		assert(m_target != m_screen);

		// flush down what we batched
		renderVertexArrays();

		if (GLEE_EXT_framebuffer_object && m_useframebuffer) {
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		} else {
			bindTexture(0, static_cast<GLImage*>(m_img_target.get())->getTexId());
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0,
				m_img_target->getWidth(), m_img_target->getHeight(), 0);
		}

		m_target = m_screen;
		glViewport(0, 0, m_screen->w, m_screen->h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, m_screen->w, m_screen->h, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glCullFace(GL_BACK);
	}

	void RenderBackendOpenGL::renderGuiGeometry(const std::vector<GuiVertex>& vertices, const std::vector<int>& indices, const DoublePoint& translation, ImagePtr texture) {
	
		glPushMatrix();
		glTranslatef(translation.x, translation.y, 0);
		
		glVertexPointer(2, GL_DOUBLE, sizeof(GuiVertex), &vertices[0].position);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(GuiVertex), &vertices[0].color);
		
		GLuint texId = 0;

		GLImage* glImage = dynamic_cast<GLImage*>(texture.get());
		if(glImage) {
			glImage->forceLoadInternal();
			texId = glImage->getTexId();
		}
		
		if(texId == 0) {
			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		} else {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texId);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_DOUBLE, sizeof(GuiVertex), &vertices[0].texCoords);
		}
		
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, &indices[0]);
		
		glPopMatrix();
	}
}
