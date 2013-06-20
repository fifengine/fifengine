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

#include "gleimage.h"
#include "renderbackendopengle.h"
#include "SDL_image.h"

#define ALPHA_REF 0.3f


namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIDEO);

	class RenderBackendOpenGLe::RenderObject {
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
			stencil_func(0)
			{}

	public:
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
		uint8_t rgba[4];
	};

	const float RenderBackendOpenGLe::zfar =   100.0f;
	const float RenderBackendOpenGLe::znear = -100.0f;

	static const int max_quads_per_texbatch = 600;
	static const int max_tex = 400; // TODO: could do this expandable
	static const int buffer_default_size = 4 * max_quads_per_texbatch * max_tex;

	RenderBackendOpenGLe::RenderBackendOpenGLe(const SDL_Color& colorkey)
		: RenderBackend(colorkey), m_mask_overlays(0){

		m_state.tex_enabled[0] = false;
		m_state.tex_enabled[1] = false;
		m_state.texture[0] = 0;
		m_state.texture[1] = 0;
		m_state.active_tex = 0;

		m_state.lightmodel = 0;
		m_state.light_enabled = false;

		m_state.sten_enabled = false;
		m_state.sten_ref = 0;
		m_state.sten_buf = 0;
		m_state.sten_op = 0;
		m_state.sten_func = 0;

		m_state.env_color[0] = 0;
		m_state.env_color[1] = 0;
		m_state.env_color[2] = 0;
		m_state.env_color[3] = 0;
		m_state.blend_src = GL_SRC_ALPHA;
		m_state.blend_dst = GL_ONE_MINUS_SRC_ALPHA;
		m_state.alpha_enabled = true;
		m_state.depth_enabled = true;
		m_state.scissor_test = true;
	}

	RenderBackendOpenGLe::~RenderBackendOpenGLe() {
		glDeleteTextures(1, &m_mask_overlays);
		if(GLEE_EXT_framebuffer_object && m_useframebuffer) {
			glDeleteFramebuffers(1, &m_fbo_id);
		}
		deinit();
	}

	const std::string& RenderBackendOpenGLe::getName() const {
		static std::string backend_name = "OpenGLe";
		return backend_name;
	}

	void RenderBackendOpenGLe::init(const std::string& driver) {
		// note: driver has no affect on the opengl renderer so do nothing with it here.
		Uint32 flags = SDL_INIT_VIDEO;
		if (SDL_InitSubSystem(flags) < 0)
			throw SDLException(SDL_GetError());
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL); // temporary hack
	}

	void RenderBackendOpenGLe::clearBackBuffer() {
		disableScissorTest();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		enableScissorTest();
	}

	void RenderBackendOpenGLe::createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon){
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

	void RenderBackendOpenGLe::setScreenMode(const ScreenMode& mode) {
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

		FL_LOG(_log, LMsg("RenderBackendOpenGLe")
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
		glOrtho(0, width, height, 0, znear, zfar);
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
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, ALPHA_REF);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_SCISSOR_TEST);

		glEnableClientState(GL_VERTEX_ARRAY);

		prepareForOverlays();

		glPointSize(1.0);
		glLineWidth(1.0);

		if(GLEE_EXT_framebuffer_object && m_useframebuffer) {
			glGenFramebuffers(1, &m_fbo_id);
		}

		m_renderZ_datas.resize(buffer_default_size);
	}

	void RenderBackendOpenGLe::startFrame() {
		RenderBackend::startFrame();
	}

	void RenderBackendOpenGLe::endFrame() {
		SDL_GL_SwapBuffers();
		RenderBackend::endFrame();
	}

	Image* RenderBackendOpenGLe::createImage(IResourceLoader* loader) {
		return new GLeImage(loader);
	}

	Image* RenderBackendOpenGLe::createImage(const std::string& name, IResourceLoader* loader) {
		return new GLeImage(name, loader);
	}

	Image* RenderBackendOpenGLe::createImage(SDL_Surface* surface) {
		// Given an abritary surface, we must convert it to the format GLeImage will understand.
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

			return new GLeImage(surface);
		}

		uint8_t bpp = m_rgba_format.BitsPerPixel;
		m_rgba_format.BitsPerPixel = 32;
		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, SDL_SWSURFACE | SDL_SRCALPHA);
		m_rgba_format.BitsPerPixel = bpp;
		GLeImage* image = new GLeImage(conv);
		SDL_FreeSurface( surface );
		return image;
	}

	Image* RenderBackendOpenGLe::createImage(const std::string& name, SDL_Surface* surface) {
		// Given an abritary surface, we must convert it to the format GLeImage will understand.
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

			return new GLeImage(name, surface);
		}

		uint8_t bpp = m_rgba_format.BitsPerPixel;
		m_rgba_format.BitsPerPixel = 32;
		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, SDL_SWSURFACE | SDL_SRCALPHA);
		m_rgba_format.BitsPerPixel = bpp;
		GLeImage* image = new GLeImage(name, conv);
		SDL_FreeSurface( surface );
		return image;
	}

	Image* RenderBackendOpenGLe::createImage(const uint8_t* data, uint32_t width, uint32_t height) {
		return new GLeImage(data, width, height);
	}

	Image* RenderBackendOpenGLe::createImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height) {
		return new GLeImage(name, data, width, height);
	}

	void RenderBackendOpenGLe::setLightingModel(uint32_t lighting) {
		if (m_state.lightmodel != lighting) {
			if (m_state.lightmodel != 0) {
				disableLighting();
				glDisable(GL_COLOR_MATERIAL);
			} else if (lighting != 0) {
				m_state.lightmodel = lighting;
				enableLighting();
				glEnable(GL_LIGHT0);
				glColorMaterial(GL_FRONT, GL_DIFFUSE);
				glEnable(GL_COLOR_MATERIAL);
			}
			m_state.lightmodel = lighting;
		}
	}

	uint32_t RenderBackendOpenGLe::getLightingModel() const {
		return m_state.lightmodel;
	}

	void RenderBackendOpenGLe::enableTextures(uint32_t texUnit) {
		if(m_state.tex_enabled[texUnit] == false) {
			if(m_state.active_tex != texUnit) {
				m_state.active_tex = texUnit;
				glActiveTexture(GL_TEXTURE0 + texUnit);
			}
			m_state.tex_enabled[texUnit] = true;

			glEnable(GL_TEXTURE_2D);
			if(texUnit == 0) {
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			}
		}
	}

	void RenderBackendOpenGLe::disableTextures(uint32_t texUnit)
	{
		if(m_state.tex_enabled[texUnit] == true) {
			if(m_state.active_tex != texUnit) {
				m_state.active_tex = texUnit;
				glActiveTexture(GL_TEXTURE0 + texUnit);
			}
			m_state.tex_enabled[texUnit] = false;

			glDisable(GL_TEXTURE_2D);
			if(texUnit == 0) {
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
		}
	}

	void RenderBackendOpenGLe::bindTexture(uint32_t texUnit, GLuint texId) {
		enableTextures(texUnit);

		if(m_state.texture[texUnit] != texId) {
			if(m_state.active_tex != texUnit) {
				m_state.active_tex = texUnit;
				glActiveTexture(GL_TEXTURE0 + texUnit);
			}
			m_state.texture[texUnit] = texId;
			glBindTexture(GL_TEXTURE_2D, texId);
		}
	}

	void RenderBackendOpenGLe::bindTexture(GLuint texId) {
		if(m_state.texture[m_state.active_tex] != texId) {
			m_state.texture[m_state.active_tex] = texId;
			glBindTexture(GL_TEXTURE_2D, texId);
		}
	}

	void RenderBackendOpenGLe::enableLighting() {
		if (m_state.lightmodel != 0 && !m_state.light_enabled) {
			glEnable(GL_LIGHTING);
			m_state.light_enabled = true;
		}
	}

	void RenderBackendOpenGLe::disableLighting() {
		if (m_state.lightmodel != 0 && m_state.light_enabled) {
			glDisable(GL_LIGHTING);
			m_state.light_enabled = false;
		}
	}

	void RenderBackendOpenGLe::setLighting(float red, float green, float blue) {
		if (m_state.lightmodel != 0) {
			GLfloat lightDiffuse[] = {red, green, blue, 1.0f};
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		}
	}

	void RenderBackendOpenGLe::resetLighting() {
		m_state.light_enabled = false;
	}

	void RenderBackendOpenGLe::enableStencilTest() {
		if (!m_state.sten_enabled) {
			glEnable(GL_STENCIL_TEST);
			m_state.sten_enabled = true;
		}
	}

	void RenderBackendOpenGLe::disableStencilTest() {
		if (m_state.sten_enabled) {
			glDisable(GL_STENCIL_TEST);
			m_state.sten_enabled = false;
		}
	}

	void RenderBackendOpenGLe::setStencilTest(uint8_t stencil_ref, GLenum stencil_op, GLenum stencil_func) {
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

	void RenderBackendOpenGLe::resetStencilBuffer(uint8_t buffer) {
		if (buffer != m_state.sten_buf) {
			m_state.sten_buf = buffer;
			glClearStencil(buffer);
		}
		disableScissorTest();
		glClear(GL_STENCIL_BUFFER_BIT);
		enableScissorTest();
	}

	void RenderBackendOpenGLe::enableAlphaTest() {
		if (!m_state.alpha_enabled) {
			glEnable(GL_ALPHA_TEST);
			m_state.alpha_enabled = true;
		}
	}

	void RenderBackendOpenGLe::disableAlphaTest() {
		if (m_state.alpha_enabled) {
			glDisable(GL_ALPHA_TEST);
			m_state.alpha_enabled = false;
		}
	}

	void RenderBackendOpenGLe::setAlphaTest(float ref_alpha) {
		enableAlphaTest();
		glAlphaFunc(GL_GREATER, ref_alpha);
	}

	void RenderBackendOpenGLe::enableDepthTest() {
		if (!m_state.depth_enabled) {
			glEnable(GL_DEPTH_TEST);
			m_state.depth_enabled = true;
		}
	}

	void RenderBackendOpenGLe::disableDepthTest() {
		if (m_state.depth_enabled) {
			glDisable(GL_DEPTH_TEST);
			m_state.depth_enabled = false;
		}
	}

	void RenderBackendOpenGLe::setEnvironmentalColor(const uint8_t* rgba) {
		if (memcmp(m_state.env_color, rgba, sizeof(uint8_t) * 4)) {
			memcpy(m_state.env_color, rgba, sizeof(uint8_t) * 4);
			GLfloat rgbaf[4] = {
				static_cast<float>(m_state.env_color[0]) / 255.0f,
				static_cast<float>(m_state.env_color[1]) / 255.0f,
				static_cast<float>(m_state.env_color[2]) / 255.0f,
				static_cast<float>(m_state.env_color[3]) / 255.0f };
			glActiveTexture(GL_TEXTURE1);
			glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, rgbaf);
			glActiveTexture(GL_TEXTURE0);
		}
	}

	void RenderBackendOpenGLe::enableScissorTest() {
		if(m_state.scissor_test == false) {
			m_state.scissor_test = true;
			glEnable(GL_SCISSOR_TEST);
		}
	}

	void RenderBackendOpenGLe::disableScissorTest() {
		if(m_state.scissor_test == true)	{
			m_state.scissor_test = false;
			glDisable(GL_SCISSOR_TEST);
		}
	}

	void RenderBackendOpenGLe::changeBlending(int32_t src, int32_t dst) {
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

	void RenderBackendOpenGLe::changeRenderInfos(uint16_t elements, int32_t src, int32_t dst, bool light,
		bool stentest, uint8_t stenref, GLConstants stenop, GLConstants stenfunc, OverlayType otype) {

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

	void RenderBackendOpenGLe::renderVertexArrays() {
		// Rendering order:
		// * batched ordinary, full opacity textured quads
		// * outlines and unlit with optional stencil test on (write on) if light is enabled
		// * colored overlays - full opacity and (semi)transparent
		// * semi transparent textured quads (sorted by instancerenderer)
		if(!m_renderZ_objects.empty() || !m_renderZ_objects_forced.empty()) {
			renderWithZ();
		}

		// * everything else that doesn't use Z value or features like
		//   stencil test/alpha test/colored overlays/semi transparency:
		//    - different renderers (aside from instance one)
		//    - gui
		if(!m_render_objects.empty()) {
			renderWithoutZ();
		}
	}

	void RenderBackendOpenGLe::renderWithZ() {
		enableAlphaTest();
		enableDepthTest();
		enableTextures(0);
		enableLighting();
		glDisableClientState(GL_COLOR_ARRAY);

		/* 1) ordinary z-valued quads */ {
			static const uint32_t stride = sizeof(RenderZData);

			glVertexPointer(3, GL_FLOAT, stride, &m_renderZ_datas[0].vertex);
			glTexCoordPointer(2, GL_FLOAT, stride, &m_renderZ_datas[0].texel);

			std::vector<RenderZObject>::iterator iter = m_renderZ_objects.begin();
			for ( ; iter != m_renderZ_objects.end(); ++iter) {
				bindTexture(iter->texture_id);
				glDrawArrays(GL_QUADS, iter->index, iter->elements);
			}
			m_renderZ_objects.clear();
		}

		// 2) forced batches (for unlit quads like outlines and unlit demanded instances)
		if (!m_renderZ_objects_forced.empty()) {
			static const uint32_t stride = sizeof(RenderZData);


			glVertexPointer(3, GL_FLOAT, stride, &m_renderZ_datas[0].vertex);
			glTexCoordPointer(2, GL_FLOAT, stride, &m_renderZ_datas[0].texel);
			setStencilTest(255, GL_REPLACE, GL_ALWAYS);
			disableLighting();

			std::vector<RenderZObject>::iterator iter = m_renderZ_objects_forced.begin();
			for ( ; iter != m_renderZ_objects_forced.end(); ++iter) {
				bindTexture(iter->texture_id);
				glDrawArrays(GL_QUADS, iter->index, iter->elements);
			}
			disableStencilTest();
			enableLighting();
			m_renderZ_objects_forced.clear();
		}

		// now we gonna need color values
		glEnableClientState(GL_COLOR_ARRAY);

		// 3) full opacity and (semi)transparent colored overlays
		if (!m_render_objects2T.empty()) {
			static const uint32_t stride = sizeof(RenderZData2T);

			glActiveTexture(GL_TEXTURE1);
			glEnable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE0);

			glVertexPointer(3, GL_FLOAT, stride, &m_render_datas2T[0].vertex);
			glColorPointer(4, GL_UNSIGNED_BYTE, stride, &m_render_datas2T[0].color);

			glClientActiveTexture(GL_TEXTURE1);
			glTexCoordPointer(2, GL_FLOAT, stride, &m_render_datas2T[0].texel2);
			glClientActiveTexture(GL_TEXTURE0);
			glTexCoordPointer(2, GL_FLOAT, stride, &m_render_datas2T[0].texel);

			// array index
			GLint index = 0;
			// elements to render
			GLsizei elements = 0;
			// texture id
			uint32_t texture_id = 0;
			// color of overlay
			uint8_t rgba[4] = {0};

			std::vector<RenderObject>::iterator iter = m_render_objects2T.begin();
			for (; iter != m_render_objects2T.end(); ++iter) {
				if (iter->texture_id != texture_id || memcmp(rgba, iter->rgba, sizeof(uint8_t)*4)) {
					if (elements > 0) {
						glDrawArrays(GL_QUADS, index, elements);
						index += elements;
					}

					setEnvironmentalColor(iter->rgba);
					bindTexture(iter->texture_id);
					texture_id = iter->texture_id;
					elements = iter->size;;
					memcpy(rgba, iter->rgba, sizeof(uint8_t)*4);
				} else {
					elements += iter->size;
				}
			}
			glDrawArrays(GL_QUADS, index, elements);

			glActiveTexture(GL_TEXTURE1);
			glDisable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE0);

			m_render_objects2T.clear();
			m_render_datas2T.clear();
		}

		// we should stop using alpha test now
		disableAlphaTest();

		// 4) now render (semi)transparent data (they are already sorted by instancerenderer)
		if (!m_render_trans_objects.empty()) {
			static const uint32_t stride = sizeof(RenderZData2T);

			glVertexPointer(3, GL_FLOAT, stride, &m_render_trans_datas[0].vertex);
			glColorPointer(4, GL_UNSIGNED_BYTE, stride, &m_render_trans_datas[0].color);
			glClientActiveTexture(GL_TEXTURE0);
			glTexCoordPointer(2, GL_FLOAT, stride, &m_render_trans_datas[0].texel);

			// array index
			GLint index = 0;
			// elements to render
			GLsizei elements = 0;
			// texture id
			uint32_t texture_id = 0;

			std::vector<RenderObject>::iterator iter = m_render_trans_objects.begin();
			for( ; iter != m_render_trans_objects.end(); ++iter) {
				if (iter->texture_id != texture_id) {
					if (elements > 0) {
						glDrawArrays(GL_QUADS, index, elements);
						index += elements;
					}

					bindTexture(iter->texture_id);
					texture_id = iter->texture_id;
					elements = iter->size;;
				} else {
					elements += iter->size;
				}
			}
			glDrawArrays(GL_QUADS, index, elements);

			m_render_trans_datas.clear();
			m_render_trans_objects.clear();
		}

		disableTextures(0);
		disableDepthTest();
		disableLighting();
	}

	void RenderBackendOpenGLe::renderWithoutZ() {
		//bools to indicate changes
		bool type = false;
		bool texture = false;
		bool render = false;
		bool blending = false;
		bool stencil = false;

		static const uint32_t stride = sizeof(RenderData);

		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2, GL_FLOAT, stride, &m_render_datas[0].vertex);
		glTexCoordPointer(2, GL_FLOAT, stride, &m_render_datas[0].texel);
		glColorPointer(4, GL_UNSIGNED_BYTE, stride, &m_render_datas[0].color);

		disableAlphaTest();
		disableDepthTest();
		disableTextures(0);

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

		for(std::vector<RenderObject>::iterator iter = m_render_objects.begin(); iter != m_render_objects.end(); ++iter) {
			//first we look for changes
			if (iter->mode != mode) {
				type = true;
				render = true;
			}
			if (iter->texture_id != texture_id) {
				texture = true;
				render = true;
			}
			if (m_state.lightmodel != 0) {
				if (iter->src != src || iter->dst != dst) {
					blending = true;
					render = true;
				}
				// Note that we don't need to check iter->light_enabled since only Instances can be lightened
				if (iter->stencil_test != m_state.sten_enabled) {
					stencil = true;
					render = true;
				}
				if (iter->stencil_ref != m_state.sten_ref ||
					iter->stencil_op != m_state.sten_op ||
					iter->stencil_func != m_state.sten_func) {
					stencil = true;
					render = true;
				}
			}

			// if no changes then we iterate to next element
			if (!render) {
				elements += iter->size;
			// else we render everything so far
			} else {
				if (elements > 0) {
					//render
					glDrawArrays(mode, index, elements);
					index += elements;
				}
				// set element to current size
				elements = iter->size;

				// switch mode
				if (type) {
					mode = iter->mode;
					type = false;
				}
				// switch texturing
				if (texture) {
					if (iter->texture_id != 0) {
						enableTextures(0);
						bindTexture(iter->texture_id);
						texture_id = iter->texture_id;
					} else {
						disableTextures(0);
						texture_id = 0;
					}
					texture = false;
				}
				// if lighting is enabled we have to consider a few more values
				if (m_state.lightmodel != 0) {
					// change blending
					if (blending) {
						src = iter->src;
						dst = iter->dst;
						changeBlending(src, dst);
						blending = false;
					}
					// change stencil
					if (stencil) {
						if (iter->stencil_test) {
							setStencilTest(iter->stencil_ref, iter->stencil_op, iter->stencil_func);
							enableAlphaTest();
						} else {
							disableStencilTest();
							disableAlphaTest();
						}
						stencil = false;
					}
				}

				render = false;
			}
		}
		// render
		glDrawArrays(mode, index, elements);

		changeBlending(4, 5);
		disableStencilTest();
		disableTextures(0);

		//reset all states
		glDisableClientState(GL_COLOR_ARRAY);

		m_render_objects.clear();
		m_render_datas.clear();
	}

	bool RenderBackendOpenGLe::putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if ((x < 0) || (x >= (int32_t)m_target->w) ||
			(y < 0) || (y >= (int32_t)m_target->h)) {
			return false;
		}
		RenderData rd;
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

	void RenderBackendOpenGLe::drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		RenderData rd;
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

	void RenderBackendOpenGLe::drawTriangle(const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		RenderData rd;
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

	void RenderBackendOpenGLe::drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		RenderData rd;
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

	void RenderBackendOpenGLe::fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		RenderData rd;
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

	void RenderBackendOpenGLe::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		RenderData rd;
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

	void RenderBackendOpenGLe::drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		RenderData rd;
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

	void RenderBackendOpenGLe::drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int32_t subdivisions,
		float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue) {
		const float step = Mathf::twoPi()/subdivisions;
		RenderData rd;;
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

	void RenderBackendOpenGLe::addImageToArray(uint32_t id, const Rect& rect, float const* st, uint8_t alpha, uint8_t const* rgba) {
		RenderData rd;
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
		m_render_objects.push_back(ro);
	}

	void RenderBackendOpenGLe::addImageToArray(const Rect& rect, uint32_t id1, float const* st1, uint32_t id2, float const* st2, uint8_t alpha, uint8_t const* rgba) {
	}

	RenderBackendOpenGLe::RenderZObject* RenderBackendOpenGLe::getRenderBufferObject(GLuint texture_id, bool forceNewBatch) {
		if (!forceNewBatch) {
			for (std::vector<RenderZObject>::iterator it = m_renderZ_objects.begin(); it != m_renderZ_objects.end(); ++it) {
				if (it->texture_id == texture_id) {
					if (it->elements < it->max_size - 4) {
						return &(*it);
					}
				}
			}
		}
		static int last_forced = 0;

		// nothing was found (or we were forced to make new batch), we need to create new one
		RenderZObject obj;
		if (!m_renderZ_objects.empty()) {
			obj.index = m_renderZ_objects.back().index + m_renderZ_objects.back().max_size;
			obj.index += last_forced * 4;
		} else {
			obj.index = 0;
		}
		obj.texture_id = texture_id;
		obj.elements = 0;
		obj.max_size = forceNewBatch ? 4 : max_quads_per_texbatch * 4;

		if (!forceNewBatch) {
			last_forced = 0;
			m_renderZ_objects.push_back(obj);
			return &m_renderZ_objects.back();
		} else {
			++last_forced;
			m_renderZ_objects_forced.push_back(obj);
			return &m_renderZ_objects_forced.back();
		}
	}

	void RenderBackendOpenGLe::addImageToArrayZ(uint32_t id, const Rect& rect, float vertexZ, float const* st, uint8_t alpha, bool forceNewBatch, uint8_t const* rgba) {
		if (alpha == 255) {
			if (rgba == NULL) {
				// ordinary z-valued quad
				RenderZObject* renderObj = getRenderBufferObject(id, forceNewBatch);
				uint32_t offset = renderObj->index + renderObj->elements;
				renderObj->elements += 4;
				RenderZData* rd;

				rd = &m_renderZ_datas[offset];
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
				rd->texel[1] = st[1];
			} else {
				// full opacity colored overlay
				RenderZData2T rd;
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
				rd.color[3] = 255;
				m_render_datas2T.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x);
				rd.vertex[1] = static_cast<float>(rect.y+rect.h);
				rd.texel[1] = st[3];
				rd.texel2[1] = 1.0;
				m_render_datas2T.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x+rect.w);
				rd.vertex[1] = static_cast<float>(rect.y+rect.h);
				rd.texel[0] = st[2];
				rd.texel2[0] = 1.0;
				m_render_datas2T.push_back(rd);

				rd.vertex[0] = static_cast<float>(rect.x+rect.w);
				rd.vertex[1] = static_cast<float>(rect.y);
				rd.texel[1] = st[1];
				rd.texel2[1] = 0.0;
				m_render_datas2T.push_back(rd);

				RenderObject ro(GL_QUADS, 4, id);
				ro.rgba[0] = rgba[0];
				ro.rgba[1] = rgba[1];
				ro.rgba[2] = rgba[2];
				ro.rgba[3] = rgba[3];
				m_render_objects2T.push_back(ro);
			}
		} else {
			RenderZData2T rd;
			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.vertex[2] = vertexZ;
			rd.texel[0] = st[0];
			rd.texel[1] = st[1];
			rd.color[0] = 255;
			rd.color[1] = 255;
			rd.color[2] = 255;
			rd.color[3] = alpha;
			m_render_trans_datas.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[1] = st[3];
			m_render_trans_datas.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y+rect.h);
			rd.texel[0] = st[2];
			m_render_trans_datas.push_back(rd);

			rd.vertex[0] = static_cast<float>(rect.x+rect.w);
			rd.vertex[1] = static_cast<float>(rect.y);
			rd.texel[1] = st[1];
			m_render_trans_datas.push_back(rd);

			RenderObject ro(GL_QUADS, 4, id);
			if (rgba != NULL) {
				RenderObject ro(GL_QUADS, 4, id);
				ro.rgba[0] = rgba[0];
				ro.rgba[1] = rgba[1];
				ro.rgba[2] = rgba[2];
				ro.rgba[3] = rgba[3];
			}
			m_render_trans_objects.push_back(ro);

		}
	}

	void RenderBackendOpenGLe::prepareForOverlays() {
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);

		if(m_mask_overlays == 0) {
			// Constant texture - can be constant across every tilesets
			glGenTextures(1, &m_mask_overlays);

			uint8_t dummydata[3] = {127, 127, 127};
			glBindTexture(GL_TEXTURE_2D, m_mask_overlays);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0,
				GL_RGB, GL_UNSIGNED_BYTE, dummydata);
		} else {
			glBindTexture(GL_TEXTURE_2D, m_mask_overlays);
		}

		m_state.texture[1] = m_mask_overlays;

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

		// Return to normal sampling mode
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		// For now it's unneecessary - Only needed if we intend to use the 2nd texture unit in different case
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	void RenderBackendOpenGLe::captureScreen(const std::string& filename) {
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

	void RenderBackendOpenGLe::captureScreen(const std::string& filename, uint32_t width, uint32_t height) {
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

	void RenderBackendOpenGLe::setClipArea(const Rect& cliparea, bool clear) {
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

	void RenderBackendOpenGLe::attachRenderTarget(ImagePtr& img, bool discard) {
		// flush down what we batched for the old target
		renderVertexArrays();

		m_img_target = img;
		m_target_discard = discard;

		// to render on something, we need to make sure its loaded already in gpu memory
		m_img_target->forceLoadInternal();
		m_target = m_img_target->getSurface();

		GLeImage* glimage = static_cast<GLeImage*>(m_img_target.get());

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
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		} else if (!m_target_discard && (!GLEE_EXT_framebuffer_object || !m_useframebuffer)) {
			// if we wanna just add something to render target, we need to first render previous contents
			addImageToArray(targetid, m_img_target->getArea(),
				static_cast<GLeImage*>(m_img_target.get())->getTexCoords(), 255, 0);
			// flush it down
			renderWithoutZ();
		}
	}

	void RenderBackendOpenGLe::detachRenderTarget() {
		assert(m_target != m_screen);

		// flush down what we batched
		renderVertexArrays();

		if (GLEE_EXT_framebuffer_object && m_useframebuffer) {
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		} else {
			bindTexture(0, static_cast<GLeImage*>(m_img_target.get())->getTexId());
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0,
				m_img_target->getWidth(), m_img_target->getHeight(), 0);
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		m_target = m_screen;
		glViewport(0, 0, m_screen->w, m_screen->h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, m_screen->w, m_screen->h, 0, znear, zfar);
		glMatrixMode(GL_MODELVIEW);
		glCullFace(GL_BACK);
	}
	
	void RenderBackendOpenGLe::renderGuiGeometry(const std::vector<GuiVertex>& vertices, const std::vector<int>& indices, const DoublePoint& translation, ImagePtr texture) {
		
	}
}
