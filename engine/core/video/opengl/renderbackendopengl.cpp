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
			color(true),
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
		bool color;
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
		m_state.alpha_test = 0.0;
		m_state.vertex_pointer_size = 2;
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
		m_state.alpha_enabled = true;
		m_state.scissor_test = true;
		m_state.depth_enabled = true;
		m_state.color_enabled = true;
	}

	RenderBackendOpenGL::~RenderBackendOpenGL() {
		glDeleteTextures(1, &m_maskOverlay);
		if(GLEE_EXT_framebuffer_object && m_useframebuffer) {
			glDeleteFramebuffers(1, &m_fbo_id);
		}
		SDL_GL_DeleteContext(m_context);
		SDL_DestroyWindow(m_window);
		deinit();
	}

	const std::string& RenderBackendOpenGL::getName() const {
		static std::string backend_name = "OpenGL";
		return backend_name;
	}

	void RenderBackendOpenGL::init(const std::string& driver) {
		Uint32 flags = SDL_INIT_VIDEO;
		if (SDL_InitSubSystem(flags) < 0) {
			throw SDLException(SDL_GetError());
		}
		if (driver != "") {
			if (SDL_VideoInit(driver.c_str()) < 0) {
				throw SDLException(SDL_GetError());
			}
		}
		// defines buffer sizes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

		// sync swaping with refresh rate if VSync is enabled
		SDL_GL_SetSwapInterval(static_cast<uint8_t>(m_vSync));
	}

	void RenderBackendOpenGL::clearBackBuffer() {
		disableScissorTest();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		enableScissorTest();
	}

	void RenderBackendOpenGL::createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon){
		setScreenMode(mode);
		if (m_window) {
			if (icon != "") {
				SDL_Surface *img = IMG_Load(icon.c_str());
				if (img != NULL) {
					SDL_SetWindowIcon(m_window, img);
					SDL_FreeSurface(img);
				}
			}
			SDL_SetWindowTitle(m_window, title.c_str());
		}
	}

	void RenderBackendOpenGL::setScreenMode(const ScreenMode& mode) {
		uint16_t width = mode.getWidth();
		uint16_t height = mode.getHeight();
		uint16_t bitsPerPixel = mode.getBPP();
		uint32_t flags = mode.getSDLFlags();
		// in case of recreating
		if (m_window) {
			SDL_GL_DeleteContext(m_context);
			SDL_DestroyWindow(m_window);
			m_screen = NULL;
		}
		// create window
		uint8_t displayIndex = mode.getDisplay();
		if (mode.isFullScreen()) {
			m_window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED_DISPLAY(displayIndex), SDL_WINDOWPOS_UNDEFINED_DISPLAY(displayIndex), width, height, flags | SDL_WINDOW_SHOWN);
		} else {
			m_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex), SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex), width, height, flags | SDL_WINDOW_SHOWN);
		}

		if (!m_window) {
			throw SDLException(SDL_GetError());
		}
		// make sure the window have the right settings
		SDL_DisplayMode displayMode;
		displayMode.format = mode.getFormat();
		displayMode.w = width;
		displayMode.h = height;
		displayMode.refresh_rate = mode.getRefreshRate();
		if (SDL_SetWindowDisplayMode(m_window, &displayMode) != 0) {
			throw SDLException(SDL_GetError());
		}

		// create render context
		m_context = SDL_GL_CreateContext(m_window);
		// set the window surface as main surface, not really needed anymore
		m_screen = SDL_GetWindowSurface(m_window);
		m_target = m_screen;
		if (!m_screen) {
			throw SDLException(SDL_GetError());
		}

		FL_LOG(_log, LMsg("RenderBackendOpenGL")
			<< "Videomode " << width << "x" << height
			<< " at " << int32_t(bitsPerPixel) << " bpp with " << displayMode.refresh_rate << " Hz");
		
		// this is needed, otherwise we would have screen pixel formats which will not work with
		// our texture generation. 32 bit surfaces to BitsPerPixel texturen.
		m_rgba_format = *(m_screen->format);
		if (bitsPerPixel != 16) {
			m_rgba_format.format = SDL_PIXELFORMAT_RGBA8888;
			m_rgba_format.BitsPerPixel = 32;
		} else {
			m_rgba_format.format = SDL_PIXELFORMAT_RGBA4444;
			m_rgba_format.BitsPerPixel = 16;
		}
		m_rgba_format.Rmask = RMASK;
		m_rgba_format.Gmask = GMASK;
		m_rgba_format.Bmask = BMASK;
		m_rgba_format.Amask = AMASK;

		//update the screen mode with the actual flags used
		m_screenMode = mode;

		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, -100, 100);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClearStencil(0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		setAlphaTest(m_alphaValue);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

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

		// currently unused, 1000 objects x 400 textures x 4 renderDataZ
		//m_renderZ_datas.resize(1600000);
	}

	void RenderBackendOpenGL::startFrame() {
		RenderBackend::startFrame();
	}

	void RenderBackendOpenGL::endFrame() {
		if (m_window) {
			SDL_GL_SwapWindow(m_window);
		}
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
			&& m_rgba_format.Aloss == surface->format->Aloss) {

			return new GLImage(surface);
		}

		uint8_t bpp = m_rgba_format.BitsPerPixel;
		m_rgba_format.BitsPerPixel = 32;
		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, 0);
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
			&& m_rgba_format.Aloss == surface->format->Aloss) {

			return new GLImage(name, surface);
		}

		uint8_t bpp = m_rgba_format.BitsPerPixel;
		m_rgba_format.BitsPerPixel = 32;
		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, 0);
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
		if (!Mathf::Equal(m_state.alpha_test, ref_alpha)) {
			m_state.alpha_test = ref_alpha;
			glAlphaFunc(GL_GREATER, ref_alpha);
		}
	}

	void RenderBackendOpenGL::enableDepthTest() {
		if (!m_state.depth_enabled) {
			glEnable(GL_DEPTH_TEST);
			m_state.depth_enabled = true;
		}
	}

	void RenderBackendOpenGL::disableDepthTest() {
		if (m_state.depth_enabled) {
			glDisable(GL_DEPTH_TEST);
			m_state.depth_enabled = false;
		}
	}

	void RenderBackendOpenGL::enableColorArray() {
		if (!m_state.color_enabled) {
			glEnableClientState(GL_COLOR_ARRAY);
			m_state.color_enabled = true;
		}
	}

	void RenderBackendOpenGL::disableColorArray() {
		if (m_state.color_enabled) {
			glDisableClientState(GL_COLOR_ARRAY);
			glColor4ub(255,255,255,255);
			m_state.color_enabled = false;
		}
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

	void RenderBackendOpenGL::setVertexPointer(GLint size, GLsizei stride, const GLvoid* ptr) {
		if(m_state.vertex_pointer != ptr || m_state.vertex_pointer_size != size)	{
			m_state.vertex_pointer = ptr;
			m_state.vertex_pointer_size = size;
			glVertexPointer(size, GL_FLOAT, stride, ptr);
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

	void RenderBackendOpenGL::changeRenderInfos(RenderDataType type, uint16_t elements, int32_t src, int32_t dst, bool light,
		bool stentest, uint8_t stenref, GLConstants stenop, GLConstants stenfunc, OverlayType otype) {

		uint16_t count = 0;
		switch (type) {
			case RENDER_DATA_WITHOUT_Z: {
				uint32_t size = m_renderObjects.size();
				while (count != elements) {
					++count;
					RenderObject& r = m_renderObjects.at(size-count);

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
			} break;
			case RENDER_DATA_TEXTURE_Z: {
				// not needed currently
			} break;
			case RENDER_DATA_TEXCOLOR_Z : {
				// not needed currently
			} break;
			case RENDER_DATA_MULTITEXTURE_Z: {
				uint32_t size = m_renderMultitextureObjectsZ.size();
				while (count != elements) {
					++count;
					RenderObject& r = m_renderMultitextureObjectsZ.at(size-count);
					// only overlay_type is important
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
			} break;
			default: {
				// nothing
			} break;
		}
	}

	void RenderBackendOpenGL::renderWithoutZ() {
		// bools to indicate changes
		bool type = false;
		bool texture = false;
		bool blending = false;
		bool light = false;
		bool stencil = false;
		bool color = false;
		bool mt = false;
		bool render = false;

		// render mode
		GLenum mode = GL_QUADS;
		// texture id
		uint32_t texture_id = 0;
		uint32_t texture_id2 = 0;
		// src blending mode
		int32_t src = 4;
		// dst blending mode
		int32_t dst = 5;
		// overlay type
		OverlayType overlay_type = OVERLAY_TYPE_NONE;
		// overlay color
		uint8_t rgba[4] = {0};

		// array indices
		int32_t indexP = 0;
		int32_t indexT = 0;
		int32_t indexTC = 0;
		int32_t index2TC = 0;
		// elements to render
		uint32_t elementsP = 0;
		uint32_t elementsT = 0;
		uint32_t elementsTC = 0;
		uint32_t elements2TC = 0;

		int32_t* currentIndex = 0;
		uint32_t* currentElements = 0;

		//stride
		const uint32_t strideP = sizeof(renderDataP);
		const uint32_t strideT = sizeof(renderDataT);
		const uint32_t strideTC = sizeof(renderDataTC);
		const uint32_t stride2TC = sizeof(renderData2TC);

		// disable alpha and depth tests
		disableAlphaTest();
		disableDepthTest();

		if (m_renderObjects[0].overlay_type == OVERLAY_TYPE_NONE) {
			// texture without color/alpha
			if (!m_renderObjects[0].color) {
				// set pointer
				disableColorArray();
				setVertexPointer(2, strideT, &m_renderTextureDatas[0].vertex);
				setTexCoordPointer(0, strideT, &m_renderTextureDatas[0].texel);
				currentIndex = &indexT;
				currentElements = &elementsT;
			// texture with color/alpha
			} else if (m_renderObjects[0].texture_id != 0){
				// set pointer
				enableColorArray();
				setVertexPointer(2, strideTC, &m_renderTextureColorDatas[0].vertex);
				setTexCoordPointer(0, strideTC, &m_renderTextureColorDatas[0].texel);
				setColorPointer(strideTC, &m_renderTextureColorDatas[0].color);
				currentIndex = &indexTC;
				currentElements = &elementsTC;
			// primitive
			} else {
				// set pointer
				enableColorArray();
				setVertexPointer(2, strideP, &m_renderPrimitiveDatas[0].vertex);
				setColorPointer(strideP, &m_renderPrimitiveDatas[0].color);
				currentIndex = &indexP;
				currentElements = &elementsP;
			}
		// multitexture overlay
		} else {
			// set pointer
			setVertexPointer(2, stride2TC, &m_renderMultitextureDatas[0].vertex);
			setColorPointer(stride2TC, &m_renderMultitextureDatas[0].color);
			setTexCoordPointer(0, stride2TC, &m_renderMultitextureDatas[0].texel);
			currentIndex = &index2TC;
			currentElements = &elements2TC;
		}

		for(std::vector<RenderObject>::iterator ir = m_renderObjects.begin(); ir != m_renderObjects.end(); ++ir) {
			RenderObject& ro = (*ir);

			// first we look for changes
			if (ro.mode != mode) {
				type = true;
				render = true;
			} else if (ro.mode == GL_LINE_STRIP || ro.mode == GL_LINE_LOOP || ro.mode == GL_TRIANGLE_FAN ) {
				// do not batch line strips, loops or triangle fans to avoid side effects
				render = true;
			}
			if (ro.texture_id != texture_id) {
				texture = true;
				render = true;
			}
			if (ro.color != m_state.color_enabled) {
				color = true;
				render = true;
			}
			if (ro.overlay_type != overlay_type || 
				(ro.overlay_type != OVERLAY_TYPE_NONE && (memcmp(rgba, ro.rgba, sizeof(uint8_t) * 4) || ro.overlay_id != texture_id2))) {
				mt = true;
				render = true;
			}
			if (m_state.lightmodel) {
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
				// switch coloring
				if (color) {
					if (ro.color && !m_state.color_enabled) {
						enableColorArray();
						if (ro.overlay_type == OVERLAY_TYPE_NONE) {
							if (ro.texture_id != 0) {
								setVertexPointer(2, strideTC, &m_renderTextureColorDatas[0].vertex);
								setTexCoordPointer(0, strideTC, &m_renderTextureColorDatas[0].texel);
								setColorPointer(strideTC, &m_renderTextureColorDatas[0].color);
								currentElements = &elementsTC;
								currentIndex = &indexTC;
							} else {
								setVertexPointer(2, strideP, &m_renderPrimitiveDatas[0].vertex);
								setColorPointer(strideP, &m_renderPrimitiveDatas[0].color);
								currentElements = &elementsP;
								currentIndex = &indexP;
							}
						}
					} else if (!ro.color && m_state.color_enabled) {
						disableColorArray();
						if (ro.overlay_type == OVERLAY_TYPE_NONE) {
							setVertexPointer(2, strideT, &m_renderTextureDatas[0].vertex);
							setTexCoordPointer(0, strideT, &m_renderTextureDatas[0].texel);
							currentElements = &elementsT;
							currentIndex = &indexT;
						}
					}
					color = false;
				}
				// multitexturing
				if (mt) {
					switch (ro.overlay_type) {
					case OVERLAY_TYPE_NONE:
						disableTextures(3);
						disableTextures(2);
						disableTextures(1);
						
						if (ro.texture_id != 0) {
							enableTextures(0);
							if (m_state.color_enabled) {
								setVertexPointer(2, strideTC, &m_renderTextureColorDatas[0].vertex);
								setTexCoordPointer(0, strideTC, &m_renderTextureColorDatas[0].texel);
								setColorPointer(strideTC, &m_renderTextureColorDatas[0].color);
								currentElements = &elementsTC;
								currentIndex = &indexTC;
							} else {
								setVertexPointer(2, strideT, &m_renderTextureDatas[0].vertex);
								setTexCoordPointer(0, strideT, &m_renderTextureDatas[0].texel);
								currentIndex = &indexT;
								currentElements = &elementsT;
							}
						} else {
							setVertexPointer(2, strideP, &m_renderPrimitiveDatas[0].vertex);
							setColorPointer(strideP, &m_renderPrimitiveDatas[0].color);
							currentElements = &elementsP;
							currentIndex = &indexP;
						}
						texture_id2 = 0;
						break;
					case OVERLAY_TYPE_COLOR:
						disableTextures(3);
						disableTextures(2);
						bindTexture(1, m_maskOverlay);
						setEnvironmentalColor(1, ro.rgba);
						enableTextures(0);

						// set pointer
						setVertexPointer(2, stride2TC, &m_renderMultitextureDatas[0].vertex);
						setColorPointer(stride2TC, &m_renderMultitextureDatas[0].color);
						setTexCoordPointer(1, stride2TC, &m_renderMultitextureDatas[0].texel2);
						setTexCoordPointer(0, stride2TC, &m_renderMultitextureDatas[0].texel);

						texture_id2 = m_maskOverlay;
						currentElements = &elements2TC;
						currentIndex = &index2TC;
						break;
					case OVERLAY_TYPE_COLOR_AND_TEXTURE:
						disableTextures(3);
						disableTextures(1);
						bindTexture(2, ro.overlay_id);
						setEnvironmentalColor(2, ro.rgba);
						enableTextures(0);

						// set pointer
						setVertexPointer(2, stride2TC, &m_renderMultitextureDatas[0].vertex);
						setColorPointer(stride2TC, &m_renderMultitextureDatas[0].color);
						setTexCoordPointer(2, stride2TC, &m_renderMultitextureDatas[0].texel2);
						setTexCoordPointer(0, stride2TC, &m_renderMultitextureDatas[0].texel);

						texture_id2 = ro.overlay_id;
						currentElements = &elements2TC;
						currentIndex = &index2TC;
						break;
					case OVERLAY_TYPE_TEXTURES_AND_FACTOR:
						disableTextures(2);
						disableTextures(1);
						bindTexture(3, ro.overlay_id);
						setEnvironmentalColor(3, ro.rgba);
						enableTextures(0);

						// set pointer
						setVertexPointer(2, stride2TC, &m_renderMultitextureDatas[0].vertex);
						setColorPointer(stride2TC, &m_renderMultitextureDatas[0].color);
						setTexCoordPointer(3, stride2TC, &m_renderMultitextureDatas[0].texel2);
						setTexCoordPointer(0, stride2TC, &m_renderMultitextureDatas[0].texel);

						texture_id2 = ro.overlay_id;
						currentElements = &elements2TC;
						currentIndex = &index2TC;
						break;
					}
					memcpy(rgba, ro.rgba, sizeof(uint8_t) * 4);
					overlay_type = ro.overlay_type;
					mt = false;
				}
				// switch texturing
				if (texture) {
					if (ro.texture_id != 0) {
						bindTexture(0, ro.texture_id);
						texture_id = ro.texture_id;
						if (ro.overlay_type == OVERLAY_TYPE_NONE) {
							if (m_state.color_enabled) {
								setVertexPointer(2, strideTC, &m_renderTextureColorDatas[0].vertex);
								setTexCoordPointer(0, strideTC, &m_renderTextureColorDatas[0].texel);
								setColorPointer(strideTC, &m_renderTextureColorDatas[0].color);
								currentElements = &elementsTC;
								currentIndex = &indexTC;
							} else {
								setVertexPointer(2, strideT, &m_renderTextureDatas[0].vertex);
								setTexCoordPointer(0, strideT, &m_renderTextureDatas[0].texel);
								currentElements = &elementsT;
								currentIndex = &indexT;
							}
						}
					} else {
						disableTextures(0);
						texture_id = 0;
						if (ro.overlay_type == OVERLAY_TYPE_NONE) {
							setVertexPointer(2, strideP, &m_renderPrimitiveDatas[0].vertex);
							setColorPointer(strideP, &m_renderPrimitiveDatas[0].color);
							currentElements = &elementsP;
							currentIndex = &indexP;
						}
					}
					texture = false;
				}
				
				// set element to current size
				*currentElements = ro.size;

				// if lighting is enabled we have to consider a few more values
				if (m_state.lightmodel) {
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

		// reset all states
		if (overlay_type != OVERLAY_TYPE_NONE) {
			disableTextures(3);
			disableTextures(2);
			disableTextures(1);
		}
		disableTextures(0);
		enableColorArray();

		if (m_state.lightmodel != 0) {
			changeBlending(4, 5);
			disableLighting();
			disableStencilTest();
			disableAlphaTest();
		}

		m_renderPrimitiveDatas.clear();
		m_renderTextureDatas.clear();
		m_renderTextureColorDatas.clear();
		m_renderMultitextureDatas.clear();
		m_renderObjects.clear();
	}

	void RenderBackendOpenGL::renderWithZ() {
		// stride
		const uint32_t stride = sizeof(renderDataZ);

		// set pointer
		setVertexPointer(3, stride, &m_renderTextureDatasZ[0].vertex);
		setTexCoordPointer(0, stride, &m_renderTextureDatasZ[0].texel);

		// array index
		int32_t index = 0;
		// elements to render
		uint32_t elements = 0;
		// texture id
		uint32_t texture_id = 0;

		int32_t* currentIndex = &index;
		uint32_t* currentElements = &elements;

		enableAlphaTest();
		enableDepthTest();
		enableTextures(0);
		enableLighting();
		disableColorArray();

		for(std::vector<RenderZObject>::iterator ir = m_renderTextureObjectsZ.begin(); ir != m_renderTextureObjectsZ.end(); ++ir) {
			RenderZObject& ro = (*ir);
			// if changes then we render all previously elements
			if (ro.texture_id != texture_id) {
				if (*currentElements > 0) {
					//render
					glDrawArrays(GL_QUADS, *currentIndex, *currentElements);
					*currentIndex += *currentElements;
				}

				if (ro.texture_id != 0) {
					bindTexture(0, ro.texture_id);
					texture_id = ro.texture_id;
				} else {
					disableTextures(0);
					texture_id = 0;
				}

				// set element to current size
				*currentElements = 4;
			} else {
				// else add the element
				*currentElements += 4;
			}
		}

		// render
		glDrawArrays(GL_QUADS, *currentIndex, *currentElements);

		//reset all states
		disableLighting();
		disableTextures(0);
		disableAlphaTest();
		disableDepthTest();
		enableColorArray();

		m_renderTextureDatasZ.clear();
		m_renderTextureObjectsZ.clear();
	}

	void RenderBackendOpenGL::renderWithZTest() {
		// stride
		const uint32_t stride = sizeof(renderDataZ);

		// set pointer
		setVertexPointer(3, stride, &m_renderZ_datas[0].vertex);
		setTexCoordPointer(0, stride, &m_renderZ_datas[0].texel);

		enableAlphaTest();
		enableDepthTest();
		enableTextures(0);
		enableLighting();
		disableColorArray();

		std::vector<RenderZObjectTest>::iterator iter = m_renderZ_objects.begin();
		for ( ; iter != m_renderZ_objects.end(); ++iter) {
			bindTexture(iter->texture_id);
			glDrawArrays(GL_QUADS, iter->index, iter->elements);
		}
		m_renderZ_objects.clear();

		//reset all states
		disableLighting();
		disableTextures(0);
		disableAlphaTest();
		disableDepthTest();
		enableColorArray();
	}

	void RenderBackendOpenGL::renderWithColorAndZ() {
		// stride
		const uint32_t stride = sizeof(renderDataColorZ);

		// set pointer
		setVertexPointer(3, stride, &m_renderTextureColorDatasZ[0].vertex);
		setTexCoordPointer(0, stride, &m_renderTextureColorDatasZ[0].texel);
		setColorPointer(stride, &m_renderTextureColorDatasZ[0].color);

		// array index
		int32_t index = 0;
		// elements to render
		uint32_t elements = 0;
		// texture id
		uint32_t texture_id = 0;

		int32_t* currentIndex = &index;
		uint32_t* currentElements = &elements;

		enableDepthTest();
		// use own value, other option would be to disable it
		setAlphaTest(0.008);
		enableTextures(0);
		enableLighting();

		for(std::vector<RenderZObject>::iterator ir = m_renderTextureColorObjectsZ.begin(); ir != m_renderTextureColorObjectsZ.end(); ++ir) {
			RenderZObject& ro = (*ir);
			// if changes then we render all previously elements
			if (ro.texture_id != texture_id) {
				if (*currentElements > 0) {
					//render
					glDrawArrays(GL_QUADS, *currentIndex, *currentElements);
					*currentIndex += *currentElements;
				}

				if (ro.texture_id != 0) {
					bindTexture(0, ro.texture_id);
					texture_id = ro.texture_id;
				} else {
					disableTextures(0);
					texture_id = 0;
				}

				// set element to current size
				*currentElements = 4;
			} else {
				// else add the element
				*currentElements += 4;
			}
		}

		// render
		glDrawArrays(GL_QUADS, *currentIndex, *currentElements);

		//reset all states
		disableLighting();
		disableTextures(0);
		setAlphaTest(m_alphaValue);
		disableAlphaTest();
		disableDepthTest();

		m_renderTextureColorDatasZ.clear();
		m_renderTextureColorObjectsZ.clear();
	}

	void RenderBackendOpenGL::renderWithMultitextureAndZ() {
		//bools to indicate changes
		bool texture = false;
		bool render = false;
		bool mt = false;

		// stride
		const uint32_t stride = sizeof(renderData2TCZ);

		// set pointer
		setVertexPointer(3, stride, &m_renderMultitextureDatasZ[0].vertex);
		setTexCoordPointer(0, stride, &m_renderMultitextureDatasZ[0].texel);
		setTexCoordPointer(1, stride, &m_renderMultitextureDatasZ[0].texel2);
		setTexCoordPointer(2, stride, &m_renderMultitextureDatasZ[0].texel2);
		setTexCoordPointer(3, stride, &m_renderMultitextureDatasZ[0].texel2);
		setColorPointer(stride, &m_renderMultitextureDatasZ[0].color);

		// array index
		int32_t index = 0;
		// elements to render
		uint32_t elements = 0;
		// texture id
		uint32_t texture_id = 0;
		uint32_t texture_id2 = 0;
		// overlay type
		OverlayType overlay_type = OVERLAY_TYPE_NONE;
		// overlay color
		uint8_t color[4] = {0};

		int32_t* currentIndex = &index;
		uint32_t* currentElements = &elements;

		enableDepthTest();
		enableAlphaTest();
		enableTextures(0);
		enableLighting();

		for(std::vector<RenderObject>::iterator ir = m_renderMultitextureObjectsZ.begin(); ir != m_renderMultitextureObjectsZ.end(); ++ir) {
			RenderObject& ro = (*ir);

			// first we look for changes
			if (ro.texture_id != texture_id) {
				texture = true;
				render = true;
			}
			if (ro.overlay_type != overlay_type || 
				(ro.overlay_type != OVERLAY_TYPE_NONE && (memcmp(color, ro.rgba, sizeof(uint8_t) * 4) || ro.overlay_id != texture_id2))) {
				mt = true;
				render = true;
			}

			// if changes then we render all previously elements
			if (render) {
				if (*currentElements > 0) {
					//render
					glDrawArrays(GL_QUADS, *currentIndex, *currentElements);
					*currentIndex += *currentElements;
				}
				// multitexturing
				if(mt) {
					switch (ro.overlay_type) {
					case OVERLAY_TYPE_NONE:
						disableTextures(3);
						disableTextures(2);
						disableTextures(1);
						enableTextures(0);

						texture_id2 = 0;
						break;
					case OVERLAY_TYPE_COLOR:
						disableTextures(3);
						disableTextures(2);
						bindTexture(1, m_maskOverlay);
						setEnvironmentalColor(1, ro.rgba);
						enableTextures(0);

						texture_id2 = m_maskOverlay;
						break;
					case OVERLAY_TYPE_COLOR_AND_TEXTURE:
						disableTextures(3);
						disableTextures(1);
						bindTexture(2, ro.overlay_id);
						setEnvironmentalColor(2, ro.rgba);
						enableTextures(0);

						texture_id2 = ro.overlay_id;
						break;
					case OVERLAY_TYPE_TEXTURES_AND_FACTOR:
						disableTextures(2);
						disableTextures(1);
						bindTexture(3, ro.overlay_id);
						setEnvironmentalColor(3, ro.rgba);
						enableTextures(0);

						texture_id2 = ro.overlay_id;
						break;
					}
					memcpy(color, ro.rgba, sizeof(uint8_t) * 4);
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
				render = false;
			} else {
				// else add the element
				*currentElements += ro.size;
			}
		}
		// render
		glDrawArrays(GL_QUADS, *currentIndex, *currentElements);

		//reset all states
		if (overlay_type != OVERLAY_TYPE_NONE) {
			disableTextures(3);
			disableTextures(2);
			disableTextures(1);
		}
		disableTextures(0);
		disableLighting();
		disableAlphaTest();
		disableDepthTest();

		m_renderMultitextureDatasZ.clear();
		m_renderMultitextureObjectsZ.clear();
	}

	void RenderBackendOpenGL::renderVertexArrays() {
		// z stuff
		if (!m_renderZ_objects.empty()) {
			renderWithZTest();
		}
		if (!m_renderTextureObjectsZ.empty()) {
			renderWithZ();
		}
		if (!m_renderMultitextureObjectsZ.empty()) {
			renderWithMultitextureAndZ();
		}
		if (!m_renderTextureColorObjectsZ.empty()) {
			renderWithColorAndZ();
		}

		// objects without z
		if (!m_renderObjects.empty()) {
			renderWithoutZ();
		}
	}

	bool RenderBackendOpenGL::putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if ((x < 0) || (x >= (int32_t)m_target->w) ||
			(y < 0) || (y >= (int32_t)m_target->h)) {
			return false;
		}
		renderDataP rd;
		rd.vertex[0] = static_cast<float>(x)+0.375;
		rd.vertex[1] = static_cast<float>(y)+0.375;
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);

		RenderObject ro(GL_POINTS, 1);
		m_renderObjects.push_back(ro);

		return true;
	}

	void RenderBackendOpenGL::drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderDataP rd;
		rd.vertex[0] = static_cast<float>(p1.x)+0.375;
		rd.vertex[1] = static_cast<float>(p1.y)+0.375;
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p2.x)+0.375;
		rd.vertex[1] = static_cast<float>(p2.y)+0.375;
		m_renderPrimitiveDatas.push_back(rd);

		RenderObject ro(GL_LINES, 2);
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::drawThickLine(const Point& p1, const Point& p2, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		float xDiff = p2.x - p1.x;
		float yDiff = p2.y - p1.y;
		float halfW = static_cast<float>(width) / 2.0;
		float angle = Mathf::ATan2(yDiff, xDiff) * (180.0 / Mathf::pi()) + 90.0;
		if (angle < 0.0) {
			angle += 360.0;
		} else if (angle > 360.0) {
			angle -= 360.0;
		}
		angle *= Mathf::pi() / 180.0;
		float cornerX = halfW * Mathf::Cos(angle);
		float cornerY = halfW * Mathf::Sin(angle);

		renderDataP rd;
		rd.vertex[0] = static_cast<float>(p1.x) + cornerX;
		rd.vertex[1] = static_cast<float>(p1.y) + cornerY;
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);
		rd.vertex[0] = static_cast<float>(p2.x) + cornerX;
		rd.vertex[1] = static_cast<float>(p2.y) + cornerY;
		m_renderPrimitiveDatas.push_back(rd);
		rd.vertex[0] = static_cast<float>(p2.x) - cornerX;
		rd.vertex[1] = static_cast<float>(p2.y) - cornerY;
		m_renderPrimitiveDatas.push_back(rd);
		rd.vertex[0] = static_cast<float>(p1.x) - cornerX;
		rd.vertex[1] = static_cast<float>(p1.y) - cornerY;
		m_renderPrimitiveDatas.push_back(rd);

		RenderObject ro(GL_TRIANGLE_FAN, 4);
		m_renderObjects.push_back(ro);

		/*rd.vertex[0] = static_cast<float>(p1.x) + cornerX;
		rd.vertex[1] = static_cast<float>(p1.y) + cornerY;
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);
		rd.vertex[0] = static_cast<float>(p2.x) + cornerX;
		rd.vertex[1] = static_cast<float>(p2.y) + cornerY;
		m_renderPrimitiveDatas.push_back(rd);
		rd.vertex[0] = static_cast<float>(p2.x) - cornerX;
		rd.vertex[1] = static_cast<float>(p2.y) - cornerY;
		m_renderPrimitiveDatas.push_back(rd);
		rd.vertex[0] = static_cast<float>(p1.x) - cornerX;
		rd.vertex[1] = static_cast<float>(p1.y) - cornerY;
		m_renderPrimitiveDatas.push_back(rd);
		rd.vertex[0] = static_cast<float>(p1.x) + cornerX;
		rd.vertex[1] = static_cast<float>(p1.y) + cornerY;
		m_renderPrimitiveDatas.push_back(rd);

		RenderObject ro(GL_TRIANGLE_STRIP, 5);
		m_renderObjects.push_back(ro);*/
	}

	void RenderBackendOpenGL::drawPolyLine(const std::vector<Point>& points, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if (points.size() < 2) {
			return;
		}
		std::vector<Point>::const_iterator it = points.begin();
		if (width > 1) {
			Point old = *it;
			++it;
			for (; it != points.end(); ++it) {
				drawThickLine(old, *it, width, r, g, b, a);
				drawFillCircle(old, width / 2, r, g, b, a);
				old = *it;
			}
			drawFillCircle(old, width / 2, r, g, b, a);
		} else {
			renderDataP rd;
			rd.color[0] = r;
			rd.color[1] = g;
			rd.color[2] = b;
			rd.color[3] = a;
			for (; it != points.end(); ++it) {
				rd.vertex[0] = static_cast<float>((*it).x);
				rd.vertex[1] = static_cast<float>((*it).y);
				m_renderPrimitiveDatas.push_back(rd);
			}
			RenderObject ro(GL_LINE_STRIP, points.size());
			m_renderObjects.push_back(ro);
		}
	}

	void RenderBackendOpenGL::drawBezier(const std::vector<Point>& points, int32_t steps, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if (points.size() < 2) {
			return;
		}
		int32_t elements = points.size();
		if (elements < 3 || steps < 2) {
			return;
		}

		bool thick = width > 1;
		float step = 1.0 / static_cast<float>(steps-1);
		float t = 0.0;
		Point old = getBezierPoint(points, elements+1, t);
		if (thick) {
			for (int32_t i = 0; i <= (elements*steps); ++i) {
				t += step;
				Point next = getBezierPoint(points, elements, t);
				drawThickLine(old, next, width, r, g, b, a);
				drawFillCircle(old, width / 2, r, g, b, a);
				old = next;
			}
			drawFillCircle(old, width / 2, r, g, b, a);
		} else {
			renderDataP rd;
			rd.color[0] = r;
			rd.color[1] = g;
			rd.color[2] = b;
			rd.color[3] = a;
			for (int32_t i = 0; i <= (elements*steps); ++i) {
				t += step;
				Point next = getBezierPoint(points, elements, t);
				rd.vertex[0] = static_cast<float>(old.x);
				rd.vertex[1] = static_cast<float>(old.y);
				m_renderPrimitiveDatas.push_back(rd);
				old = next;
			}
			RenderObject ro(GL_LINE_STRIP, (elements*steps)+1);
			m_renderObjects.push_back(ro);
		}
	}

	void RenderBackendOpenGL::drawTriangle(const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderDataP rd;
		rd.vertex[0] = static_cast<float>(p1.x);
		rd.vertex[1] = static_cast<float>(p1.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p2.x);
		rd.vertex[1] = static_cast<float>(p2.y);
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p3.x);
		rd.vertex[1] = static_cast<float>(p3.y);
		m_renderPrimitiveDatas.push_back(rd);

		RenderObject ro(GL_TRIANGLES, 3);
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderDataP rd;
		rd.vertex[0] = static_cast<float>(p.x);
		rd.vertex[1] = static_cast<float>(p.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x+w);
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[1] = static_cast<float>(p.y+h);
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x);
		m_renderPrimitiveDatas.push_back(rd);

		RenderObject ro(GL_LINE_LOOP, 4);
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderDataP rd;
		rd.vertex[0] = static_cast<float>(p.x);
		rd.vertex[1] = static_cast<float>(p.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[1] = static_cast<float>(p.y+h);
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x+w);
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[1] = static_cast<float>(p.y);
		m_renderPrimitiveDatas.push_back(rd);

		RenderObject ro(GL_QUADS, 4);
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderDataP rd;
		rd.vertex[0] = static_cast<float>(p1.x);
		rd.vertex[1] = static_cast<float>(p1.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p2.x);
		rd.vertex[1] = static_cast<float>(p2.y);
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p3.x);
		rd.vertex[1] = static_cast<float>(p3.y);
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p4.x);
		rd.vertex[1] = static_cast<float>(p4.y);
		m_renderPrimitiveDatas.push_back(rd);

		RenderObject ro(GL_QUADS, 4);
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		renderDataP rd;
		rd.vertex[0] = static_cast<float>(p.x-size);
		rd.vertex[1] = static_cast<float>(p.y+size);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x+size);
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[1] = static_cast<float>(p.y-size);
		m_renderPrimitiveDatas.push_back(rd);

		rd.vertex[0] = static_cast<float>(p.x-size);
		m_renderPrimitiveDatas.push_back(rd);

		RenderObject ro(GL_LINE_LOOP, 4);
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::drawCircle(const Point& p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		// set side length to 5 and calculate needed divisions
		int32_t subdivisions = round(Mathf::pi() / ( 5.0 / (2.0 * radius)));
		if (subdivisions < 12) {
			subdivisions = 12;
		}
		const float step = Mathf::twoPi()/subdivisions;
		float angle = 0;

		renderDataP rd;
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		for (uint16_t i = 0; i < subdivisions-1; ++i) {
			rd.vertex[0] = radius * Mathf::Cos(angle) + p.x;
			rd.vertex[1] = radius * Mathf::Sin(angle) + p.y;
			m_renderPrimitiveDatas.push_back(rd);
			angle += step;
		}
		RenderObject ro(GL_LINE_LOOP, subdivisions-1);
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::drawFillCircle(const Point& p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		// set side length to 5 and calculate needed divisions
		int32_t subdivisions = round(Mathf::pi() / ( 5.0 / (2.0 * radius)));
		if (subdivisions < 12) {
			subdivisions = 12;
		}
		const float step = Mathf::twoPi()/subdivisions;
		float angle = Mathf::twoPi();

		renderDataP rd;
		// center
		rd.vertex[0] = static_cast<float>(p.x);
		rd.vertex[1] = static_cast<float>(p.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);
		// reversed because of culling faces
		for (uint16_t i = 0; i <= subdivisions; ++i) {
			rd.vertex[0] = radius * Mathf::Cos(angle) + p.x;
			rd.vertex[1] = radius * Mathf::Sin(angle) + p.y;
			m_renderPrimitiveDatas.push_back(rd);
			angle -= step;
		}
		RenderObject ro(GL_TRIANGLE_FAN, subdivisions+2);
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::drawCircleSegment(const Point& p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		const float step = Mathf::twoPi()/360;
		int32_t elements = 0;
		int32_t s = (sangle + 360) % 360;
		int32_t e = (eangle + 360) % 360;
		if (e == 0) {
			e = 360;
		}
		if (s == e) {
			return;
		}

		renderDataP rd;
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		float angle = static_cast<float>(s) * step;
		for (;s <= e; ++s, angle += step, ++elements) {
			rd.vertex[0] = radius * Mathf::Cos(angle) + p.x;
			rd.vertex[1] = radius * Mathf::Sin(angle) + p.y;
			m_renderPrimitiveDatas.push_back(rd);
		}

		RenderObject ro(GL_LINE_STRIP, elements);
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::drawFillCircleSegment(const Point& p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		const float step = Mathf::twoPi() / 360;
		int32_t s = (sangle + 360) % 360;
		int32_t e = (eangle + 360) % 360;
		if (e == 0) {
			e = 360;
		}
		if (s == e) {
			return;
		}
			
		renderDataP rd;
		// center
		rd.vertex[0] = static_cast<float>(p.x);
		rd.vertex[1] = static_cast<float>(p.y);
		rd.color[0] = r;
		rd.color[1] = g;
		rd.color[2] = b;
		rd.color[3] = a;
		m_renderPrimitiveDatas.push_back(rd);
		int32_t elements = 1;
		// reversed because of culling faces
		float angle = static_cast<float>(e) * step;
		for (;s <= e; ++s, angle -= step, ++elements) {
			rd.vertex[0] = radius * Mathf::Cos(angle) + p.x;
			rd.vertex[1] = radius * Mathf::Sin(angle) + p.y;

			m_renderPrimitiveDatas.push_back(rd);
		}

		RenderObject ro(GL_TRIANGLE_FAN, elements);
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue) {
		const float step = Mathf::twoPi()/subdivisions;
		renderDataP rd;
		RenderObject ro(GL_TRIANGLES, 3);
		for(float angle=0; angle<=Mathf::twoPi(); angle+=step){
			rd.vertex[0] = static_cast<float>(p.x);
			rd.vertex[1] = static_cast<float>(p.y);
			rd.color[0] = red;
			rd.color[1] = green;
			rd.color[2] = blue;
			rd.color[3] = intensity;
			m_renderPrimitiveDatas.push_back(rd);

			rd.vertex[0] = radius*Mathf::Cos(angle+step)*xstretch + p.x;
			rd.vertex[1] = radius*Mathf::Sin(angle+step)*ystretch + p.y;
			rd.color[0] = 0;
			rd.color[1] = 0;
			rd.color[2] = 0;
			rd.color[3] = 255;
			m_renderPrimitiveDatas.push_back(rd);

			rd.vertex[0] = radius*Mathf::Cos(angle)*xstretch + p.x;
			rd.vertex[1] = radius*Mathf::Sin(angle)*ystretch + p.y;
			m_renderPrimitiveDatas.push_back(rd);

			m_renderObjects.push_back(ro);
		}
	}

	void RenderBackendOpenGL::addImageToArray(uint32_t id, const Rect& rect, float const* st, uint8_t alpha, uint8_t const* rgba) {
		RenderObject ro(GL_QUADS, 4, id);
		
		// texture quad without alpha
		if (alpha == 255 && !rgba) {
			renderDataT rd;
			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.texel[0] = st[0];
			rd.texel[1] = st[1];
			m_renderTextureDatas.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[1] = st[3];
			m_renderTextureDatas.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[0] = st[2];
			m_renderTextureDatas.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.texel[1] = st[1];
			m_renderTextureDatas.push_back(rd);

			ro.color = false;
		} else {
			if (rgba) {
				renderData2TC rd;
				rd.vertex[0] = static_cast<float>(rect.x);
				rd.vertex[1] = static_cast<float>(rect.y);
				rd.texel[0] = st[0];
				rd.texel[1] = st[1];
				rd.texel2[0] = 0.0;
				rd.texel2[1] = 0.0;
				rd.color[0] = 255;
				rd.color[1] = 255;
				rd.color[2] = 255;
				rd.color[3] = alpha;
				m_renderMultitextureDatas.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x);
				rd.vertex[1] = static_cast<float>(rect.y+rect.h);
				rd.texel[1] = st[3];
				rd.texel2[1] = 1.0;
				m_renderMultitextureDatas.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x+rect.w);
				rd.vertex[1] = static_cast<float>(rect.y+rect.h);
				rd.texel[0] = st[2];
				rd.texel2[0] = 1.0;
				m_renderMultitextureDatas.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x+rect.w);
				rd.vertex[1] = static_cast<float>(rect.y);
				rd.texel[1] = st[1];
				rd.texel2[1] = 0.0;
				m_renderMultitextureDatas.push_back(rd);

				ro.color = true;
				ro.overlay_type = OVERLAY_TYPE_COLOR;
				ro.rgba[0] = rgba[0];
				ro.rgba[1] = rgba[1];
				ro.rgba[2] = rgba[2];
				ro.rgba[3] = rgba[3];
			// texture quad with alpha
			} else {
				renderDataTC rd;
				rd.vertex[0] = static_cast<float>(rect.x);
				rd.vertex[1] = static_cast<float>(rect.y);
				rd.texel[0] = st[0];
				rd.texel[1] = st[1];
				rd.color[0] = 255;
				rd.color[1] = 255;
				rd.color[2] = 255;
				rd.color[3] = alpha;
				m_renderTextureColorDatas.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x);
				rd.vertex[1] = static_cast<float>(rect.y+rect.h);
				rd.texel[1] = st[3];
				m_renderTextureColorDatas.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x+rect.w);
				rd.vertex[1] = static_cast<float>(rect.y+rect.h);
				rd.texel[0] = st[2];
				m_renderTextureColorDatas.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x+rect.w);
				rd.vertex[1] = static_cast<float>(rect.y);
				rd.texel[1] = st[1];
				m_renderTextureColorDatas.push_back(rd);

				ro.color = true;
			}
		}
		m_renderObjects.push_back(ro);
	}

	void RenderBackendOpenGL::addImageToArray(const Rect& rect, uint32_t id1, float const* st1, uint32_t id2, float const* st2, uint8_t alpha, uint8_t const* rgba) {
		if (rgba) {
			renderData2TC rd;
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
			m_renderMultitextureDatas.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[1] = st1[3];
			rd.texel2[1] = st2[3];
			m_renderMultitextureDatas.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[0] = st1[2];
			rd.texel2[0] = st2[2];
			m_renderMultitextureDatas.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.texel[1] = st1[1];
			rd.texel2[1] = st2[1];
			m_renderMultitextureDatas.push_back(rd);

			RenderObject ro(GL_QUADS, 4, id1, id2);
			ro.overlay_type = OVERLAY_TYPE_TEXTURES_AND_FACTOR;
			ro.rgba[0] = rgba[0];
			ro.rgba[1] = rgba[1];
			ro.rgba[2] = rgba[2];
			ro.rgba[3] = rgba[3];
			m_renderObjects.push_back(ro);
		}
	}

	RenderBackendOpenGL::RenderZObjectTest* RenderBackendOpenGL::getRenderBufferObject(GLuint texture_id) {
		for (std::vector<RenderZObjectTest>::iterator it = m_renderZ_objects.begin(); it != m_renderZ_objects.end(); ++it) {
			if (it->texture_id == texture_id) {
				if (it->elements < it->max_size - 4) {
					return &(*it);
				}
			}
		}
		int32_t max_quads_per_texbatch = 1000;
		// nothing was found (or we were forced to make new batch), we need to create new one
		RenderZObjectTest obj;
		if (!m_renderZ_objects.empty()) {
			obj.index = m_renderZ_objects.back().index + m_renderZ_objects.back().max_size;
		} else {
			obj.index = 0;
		}
		obj.texture_id = texture_id;
		obj.elements = 0;
		obj.max_size = max_quads_per_texbatch * 4;

		m_renderZ_objects.push_back(obj);
		return &m_renderZ_objects.back();
	}

	void RenderBackendOpenGL::addImageToArrayZ(uint32_t id, const Rect& rect, float vertexZ, float const* st, uint8_t alpha, uint8_t const* rgba) {
		// texture quad without alpha and coloring
		if (alpha == 255 && !rgba) {
			// ToDo: Consider if this is better.
			/*RenderZObjectTest* renderObj = getRenderBufferObject(id);
			uint32_t offset = renderObj->index + renderObj->elements;
			renderObj->elements += 4;

			renderDataZ* rd = &m_renderZ_datas[offset];
			rd->vertex[0] = static_cast<float>(rect.x);
			rd->vertex[1] = static_cast<float>(rect.y);
			rd->vertex[2] = vertexZ;
			rd->texel[0] = st[0];
			rd->texel[1] = st[1];

			++rd;
			rd->vertex[0] = static_cast<float>(rect.x);
			rd->vertex[1] = static_cast<float>(rect.y+rect.h);
			rd->vertex[2] = vertexZ;
			rd->texel[0] = st[0];
			rd->texel[1] = st[3];

			++rd;
			rd->vertex[0] = static_cast<float>(rect.x+rect.w);
			rd->vertex[1] = static_cast<float>(rect.y+rect.h);
			rd->vertex[2] = vertexZ;
			rd->texel[0] = st[2];
			rd->texel[1] = st[3];

			++rd;
			rd->vertex[0] = static_cast<float>(rect.x+rect.w);
			rd->vertex[1] = static_cast<float>(rect.y);
			rd->vertex[2] = vertexZ;
			rd->texel[0] = st[2];
			rd->texel[1] = st[1];*/

			renderDataZ rd;
			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.vertex[2] = vertexZ;
			rd.texel[0] = st[0];
			rd.texel[1] = st[1];
			m_renderTextureDatasZ.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[1] = st[3];
			m_renderTextureDatasZ.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[0] = st[2];
			m_renderTextureDatasZ.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.texel[1] = st[1];
			m_renderTextureDatasZ.push_back(rd);

			RenderZObject ro;
			ro.texture_id = id;
			m_renderTextureObjectsZ.push_back(ro);
		} else {
			// multitexture with color, second texel is used for m_maskOverlay
			if (rgba) {
				renderData2TCZ rd;
				rd.vertex[0] = static_cast<float>(rect.x);
				rd.vertex[1] = static_cast<float>(rect.y);
				rd.vertex[2] = vertexZ;
				rd.texel[0] = st[0];
				rd.texel[1] = st[1];
				rd.texel2[0] = 0.0;
				rd.texel2[1] = 0.0;
				rd.color[0] = 255;
				rd.color[1] = 255;
				rd.color[2] = 255;
				rd.color[3] = alpha;
				m_renderMultitextureDatasZ.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x);
				rd.vertex[1] = static_cast<float>(rect.y+rect.h);
				rd.texel[1] = st[3];
				rd.texel2[1] = 1.0;
				m_renderMultitextureDatasZ.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x+rect.w);
				rd.vertex[1] = static_cast<float>(rect.y+rect.h);
				rd.texel[0] = st[2];
				rd.texel2[0] = 1.0;
				m_renderMultitextureDatasZ.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x+rect.w);
				rd.vertex[1] = static_cast<float>(rect.y);
				rd.texel[1] = st[1];
				rd.texel2[1] = 0.0;
				m_renderMultitextureDatasZ.push_back(rd);

				RenderObject ro(GL_QUADS, 4, id);
				ro.color = true;
				ro.overlay_type = OVERLAY_TYPE_COLOR;
				ro.rgba[0] = rgba[0];
				ro.rgba[1] = rgba[1];
				ro.rgba[2] = rgba[2];
				ro.rgba[3] = rgba[3];
				m_renderMultitextureObjectsZ.push_back(ro);
			// texture with alpha
			} else {
				renderDataColorZ rd;
				rd.vertex[0] = static_cast<float>(rect.x);
				rd.vertex[1] = static_cast<float>(rect.y);
				rd.vertex[2] = vertexZ;
				rd.texel[0] = st[0];
				rd.texel[1] = st[1];
				rd.color[0] = 255;
				rd.color[1] = 255;
				rd.color[2] = 255;
				rd.color[3] = alpha;
				m_renderTextureColorDatasZ.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x);
				rd.vertex[1] = static_cast<float>(rect.y+rect.h);
				rd.texel[1] = st[3];
				m_renderTextureColorDatasZ.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x+rect.w);
				rd.vertex[1] = static_cast<float>(rect.y+rect.h);
				rd.texel[0] = st[2];
				m_renderTextureColorDatasZ.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x+rect.w);
				rd.vertex[1] = static_cast<float>(rect.y);
				rd.texel[1] = st[1];
				m_renderTextureColorDatasZ.push_back(rd);

				RenderZObject ro;
				ro.texture_id = id;
				m_renderTextureColorObjectsZ.push_back(ro);
			}
		}
	}

	void RenderBackendOpenGL::addImageToArrayZ(const Rect& rect, float vertexZ, uint32_t id1, float const* st1, uint32_t id2, float const* st2, uint8_t alpha, uint8_t const* rgba) {
		if (rgba) {
			renderData2TCZ rd;
			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.vertex[2] = vertexZ;
			rd.texel[0] = st1[0];
			rd.texel[1] = st1[1];
			rd.texel2[0] = st2[0];
			rd.texel2[1] = st2[1];
			rd.color[0] = 255;
			rd.color[1] = 255;
			rd.color[2] = 255;
			rd.color[3] = alpha;
			m_renderMultitextureDatasZ.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[1] = st1[3];
			rd.texel2[1] = st2[3];
			m_renderMultitextureDatasZ.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[0] = st1[2];
			rd.texel2[0] = st2[2];
			m_renderMultitextureDatasZ.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.texel[1] = st1[1];
			rd.texel2[1] = st2[1];
			m_renderMultitextureDatasZ.push_back(rd);

			//RenderObject ro(GL_QUADS, 4, id1);
			RenderObject ro(GL_QUADS, 4, id1, id2);
			ro.overlay_type = OVERLAY_TYPE_TEXTURES_AND_FACTOR;
			ro.rgba[0] = rgba[0];
			ro.rgba[1] = rgba[1];
			ro.rgba[2] = rgba[2];
			ro.rgba[3] = rgba[3];
			m_renderMultitextureObjectsZ.push_back(ro);
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
		SDL_Surface *surface = SDL_CreateRGBSurface(0, swidth, sheight, 24,
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
		SDL_Surface* src = SDL_CreateRGBSurface(0, swidth, sheight, 32,
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
		SDL_Surface* dst = SDL_CreateRGBSurface(0, width, height, 32,
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
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

			// currenty unused, is needed in case the static layers should be rendered with depth buffer (instead of sorting)
			// see LayerCache
			//glBindRenderbuffer(GL_RENDERBUFFER, m_depthbuffer_id);
			//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
			//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbuffer_id);
		}

		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		// invert top with bottom
		glOrtho(0, w, 0, h, -100, 100);
		glMatrixMode(GL_MODELVIEW);
		// because of inversion 2 lines above we need to also invert culling faces
		glCullFace(GL_FRONT);

		if (m_target_discard) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
		glOrtho(0, m_screen->w, m_screen->h, 0, -100, 100);
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
