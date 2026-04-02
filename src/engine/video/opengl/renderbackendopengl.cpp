// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "renderbackendopengl.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <limits>
#include <string>
#include <utility>
#include <vector>

// Platform specific includes

// 3rd party library includes
#include <SDL.h>

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "video/devicecaps.h"

#include "SDL_image.h"
#include "glimage.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIDEO);

    namespace
    {
        int32_t toInt32Dimension(uint32_t value)
        {
            assert(value <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
            return static_cast<int32_t>(value);
        }

        std::size_t toSizeTIndex(int32_t value)
        {
            assert(value >= 0);
            return static_cast<std::size_t>(value);
        }

        GLsizei toGLsizei(uint32_t value)
        {
            assert(value <= static_cast<uint32_t>(std::numeric_limits<GLsizei>::max()));
            return static_cast<GLsizei>(value);
        }

        GLsizei toGLsizei(uint16_t value)
        {
            return static_cast<GLsizei>(value);
        }

        GLsizei toGLsizei(std::size_t value)
        {
            assert(value <= static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()));
            return static_cast<GLsizei>(value);
        }

        GLint toGLint(uint32_t value)
        {
            assert(value <= static_cast<uint32_t>(std::numeric_limits<GLint>::max()));
            return static_cast<GLint>(value);
        }

        uint16_t toRenderObjectSize(std::size_t value)
        {
            assert(value <= static_cast<std::size_t>(std::numeric_limits<uint16_t>::max()));
            return static_cast<uint16_t>(value);
        }

        uint16_t toRenderObjectSize(int32_t value)
        {
            assert(value >= 0);
            assert(value <= std::numeric_limits<uint16_t>::max());
            return static_cast<uint16_t>(value);
        }

        uint16_t toRenderObjectSize(uint32_t value)
        {
            assert(value <= std::numeric_limits<uint16_t>::max());
            return static_cast<uint16_t>(value);
        }

        int32_t toDisplayWindowPos(uint8_t displayIndex, bool centered)
        {
            return centered ? static_cast<int32_t>(SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex)) :
                              static_cast<int32_t>(SDL_WINDOWPOS_UNDEFINED_DISPLAY(displayIndex));
        }
    } // namespace

    class RenderBackendOpenGL::RenderObject
    {
    public:
        RenderObject(GLenum m, uint16_t s, uint32_t t1 = 0, uint32_t t2 = 0) :
            mode(m),
            texture_id(t1),
            overlay_id(t2),
            src(4),
            dst(5),
            overlay_type(OVERLAY_TYPE_NONE),
            stencil_op(0),
            stencil_func(0),
            size(s),
            stencil_ref(0),
            light(true),
            stencil_test(false),
            color(true),
            rgba{0}

        {
        }

        GLenum mode;
        uint32_t texture_id;
        uint32_t overlay_id;
        int32_t src;
        int32_t dst;
        OverlayType overlay_type;
        GLenum stencil_op;
        GLenum stencil_func;
        uint16_t size;
        uint8_t stencil_ref;
        bool light;
        bool stencil_test;
        bool color;
        uint8_t rgba[4];
        uint8_t reserved[2]{0, 0};
    };

    RenderBackendOpenGL::RenderBackendOpenGL(const SDL_Color& colorkey) :
        RenderBackend(colorkey),
        m_maskOverlay(0),
        m_state{},
        m_fbo_id(0),
        m_indicebufferId(0),
        m_target_discard(false),
        m_context(nullptr)
    {

        m_state.tex_enabled[0]      = false;
        m_state.tex_enabled[1]      = false;
        m_state.tex_enabled[2]      = false;
        m_state.tex_enabled[3]      = false;
        m_state.texture[0]          = 0;
        m_state.texture[1]          = 0;
        m_state.texture[2]          = 0;
        m_state.texture[3]          = 0;
        m_state.active_tex          = 0;
        m_state.alpha_test          = 0.0;
        m_state.vertex_pointer_size = 2;
        m_state.color_pointer       = nullptr;
        m_state.tex_pointer[0]      = nullptr;
        m_state.tex_pointer[1]      = nullptr;
        m_state.tex_pointer[2]      = nullptr;
        m_state.tex_pointer[3]      = nullptr;
        m_state.vertex_pointer      = nullptr;

        m_state.sten_enabled = false;
        m_state.sten_ref     = 0;
        m_state.sten_buf     = 0;
        m_state.sten_op      = 0;
        m_state.sten_func    = 0;

        m_state.lightmodel    = 0;
        m_state.light_enabled = false;

        m_state.env_color[0]  = 0;
        m_state.env_color[1]  = 0;
        m_state.env_color[2]  = 0;
        m_state.env_color[3]  = 0;
        m_state.blend_src     = GL_SRC_ALPHA;
        m_state.blend_dst     = GL_ONE_MINUS_SRC_ALPHA;
        m_state.alpha_enabled = true;
        m_state.scissor_test  = true;
        m_state.depth_enabled = true;
        m_state.color_enabled = true;
    }

    RenderBackendOpenGL::~RenderBackendOpenGL()
    {
        glDeleteTextures(1, &m_maskOverlay);
        if (GLEW_EXT_framebuffer_object && m_useframebuffer) {
            glDeleteFramebuffers(1, &m_fbo_id);
        }
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_window);
        deinit();
    }

    const std::string& RenderBackendOpenGL::getName() const
    {
        static std::string const backend_name = "OpenGL";
        return backend_name;
    }

    void RenderBackendOpenGL::init(const std::string& driver)
    {
        Uint32 const flags = SDL_INIT_VIDEO;
        if (SDL_InitSubSystem(flags) < 0) {
            throw SDLException(SDL_GetError());
        }
        if (!driver.empty()) {
            if (SDL_VideoInit(driver.c_str()) < 0) {
                throw SDLException(SDL_GetError());
            }
        }

        // setup OpenGL
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        // defines buffer sizes
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
        // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    }

    void RenderBackendOpenGL::clearBackBuffer()
    {
        disableScissorTest();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        enableScissorTest();
    }

    void RenderBackendOpenGL::createMainScreen(
        const ScreenMode& mode, const std::string& title, const std::string& icon)
    {
        setScreenMode(mode);
        if (m_window != nullptr) {
            if (!icon.empty()) {
                SDL_Surface* img = IMG_Load(icon.c_str());
                if (img != nullptr) {
                    SDL_SetWindowIcon(m_window, img);
                    SDL_FreeSurface(img);
                }
            }
            SDL_SetWindowTitle(m_window, title.c_str());
        }
    }

    void RenderBackendOpenGL::setScreenMode(const ScreenMode& mode)
    {
        bool const recreate         = m_window != nullptr;
        uint16_t const width        = mode.getWidth();
        uint16_t const height       = mode.getHeight();
        uint16_t const bitsPerPixel = mode.getBPP();
        uint32_t const flags        = mode.getSDLFlags();
        // in case of recreating
        if (recreate) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
            m_screen = nullptr;

            if (GLEW_EXT_framebuffer_object && m_useframebuffer) {
                glDeleteFramebuffers(1, &m_fbo_id);
            }
        }
        // create window
        uint8_t const displayIndex = mode.getDisplay();
        int32_t const windowX      = mode.getWindowPositionX();
        int32_t const windowY      = mode.getWindowPositionY();

        // Determine X position
        int xPos{};
        if (windowX >= 0) {
            xPos = windowX;
        } else if (mode.isFullScreen()) {
            xPos = toDisplayWindowPos(displayIndex, false);
        } else {
            xPos = toDisplayWindowPos(displayIndex, true);
        }

        // Determine Y position
        int yPos{};
        if (windowY >= 0) {
            yPos = windowY;
        } else if (mode.isFullScreen()) {
            yPos = toDisplayWindowPos(displayIndex, false);
        } else {
            yPos = toDisplayWindowPos(displayIndex, true);
        }

        if (mode.isFullScreen()) {
            m_window = SDL_CreateWindow(
                "",
                toDisplayWindowPos(displayIndex, false),
                toDisplayWindowPos(displayIndex, false),
                width,
                height,
                flags | SDL_WINDOW_SHOWN);
        } else {
            m_window = SDL_CreateWindow("", xPos, yPos, width, height, flags | SDL_WINDOW_SHOWN);
        }

        if (m_window == nullptr) {
            throw SDLException(SDL_GetError());
        }
        // make sure the window have the right settings
        SDL_DisplayMode displayMode;
        displayMode.format       = mode.getFormat();
        displayMode.w            = width;
        displayMode.h            = height;
        displayMode.refresh_rate = mode.getRefreshRate();
        if (SDL_SetWindowDisplayMode(m_window, &displayMode) != 0) {
            throw SDLException(SDL_GetError());
        }

        // create render context or use the old with new window
        if (recreate) {
            if (SDL_GL_MakeCurrent(m_window, m_context) < 0) {
                throw SDLException(SDL_GetError());
            }
        } else {
            m_context = SDL_GL_CreateContext(m_window);
        }
        // set the window surface as main surface, not really needed anymore
        m_screen = SDL_GetWindowSurface(m_window);
        m_target = m_screen;
        if (m_screen == nullptr) {
            throw SDLException(SDL_GetError());
        }

        // initialize GLEW
        glewExperimental       = GL_TRUE;
        GLenum const glewError = glewInit();
        if (glewError != GLEW_OK) {
            FL_LOG(_log, LMsg("RenderBackendOpenGL") << "Error initializing GLEW!" << glewGetErrorString(glewError));
        }

        FL_LOG(
            _log,
            LMsg("RenderBackendOpenGL") << "Videomode " << width << "x" << height << " at " << int32_t(bitsPerPixel)
                                        << " bpp with " << displayMode.refresh_rate << " Hz");

        // this is needed, otherwise we would have screen pixel formats which will not work with
        // our texture generation. 32 bit surfaces to BitsPerPixel texturen.
        m_rgba_format = *(m_screen->format);
        if (bitsPerPixel != 16) {
            m_rgba_format.format       = SDL_PIXELFORMAT_RGBA8888;
            m_rgba_format.BitsPerPixel = 32;
        } else {
            m_rgba_format.format       = SDL_PIXELFORMAT_RGBA4444;
            m_rgba_format.BitsPerPixel = 16;
        }
        m_rgba_format.Rmask = RMASK;
        m_rgba_format.Gmask = GMASK;
        m_rgba_format.Bmask = BMASK;
        m_rgba_format.Amask = AMASK;

        // update the screen mode with the actual flags used
        m_screenMode = mode;

        glViewport(0, 0, toGLsizei(width), toGLsizei(height));
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        const GLdouble left   = 0.0;
        const GLdouble right  = static_cast<GLdouble>(width);
        const GLdouble bottom = static_cast<GLdouble>(height);
        const GLdouble top    = 0.0;
        glOrtho(left, right, bottom, top, -100.0, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // dont reset
        if (!recreate) {
            glClearColor(0.0F, 0.0F, 0.0F, 0.0F);
            glClearDepth(1.0F);
            glClearStencil(0);
        }

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

        if (GLEW_EXT_framebuffer_object && m_useframebuffer) {
            glGenFramebuffers(1, &m_fbo_id);
        }

        if (m_textureFilter == TEXTURE_FILTER_ANISOTROPIC) {
            if (GLEW_EXT_texture_filter_anisotropic) {
                GLint largest = 0;
                glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
                m_maxAnisotropy = static_cast<int32_t>(largest);
            } else {
                // if not available use trilinear filter
                m_maxAnisotropy = 0;
                m_textureFilter = TEXTURE_FILTER_TRILINEAR;
            }
        }

        // sync swaping with refresh rate if VSync is enabled
        SDL_GL_SetSwapInterval(static_cast<uint8_t>(m_vSync));

        // currently unused, 1000 objects x 400 textures x 4 renderDataZ
        // m_renderZ_datas.resize(1600000);

        // 6 indices x 100000 objects
        m_indices.resize(600000);
        // a rough way to fill the index buffer, result is: 0, 1, 2, 0, 2, 3 | 4, 5, 6, 4, 6, 7
        uint32_t index = 0;
        for (std::vector<uint32_t>::size_type i = 0; i != m_indices.size(); i += 6) {
            m_indices[i]     = index;
            m_indices[i + 1] = index + 1;
            m_indices[i + 2] = index + 2;
            m_indices[i + 3] = index;
            m_indices[i + 4] = index + 2;
            m_indices[i + 5] = index + 3;
            index += 4;
        }

        // currently unused, create buffer and send data
        // glGenBuffers(1, &m_indicebufferId);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicebufferId);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void RenderBackendOpenGL::startFrame()
    {
        RenderBackend::startFrame();
    }

    void RenderBackendOpenGL::endFrame()
    {
        if (m_window != nullptr) {
            SDL_GL_SwapWindow(m_window);
        }
        RenderBackend::endFrame();
    }

    Image* RenderBackendOpenGL::createImage(IResourceLoader* loader)
    {
        return new GLImage(loader);
    }

    Image* RenderBackendOpenGL::createImage(const std::string& name, IResourceLoader* loader)
    {
        return new GLImage(name, loader);
    }

    Image* RenderBackendOpenGL::createImage(SDL_Surface* surface)
    {
        // Given an abritary surface, we must convert it to the format GLImage will understand.
        // It's easiest to let SDL do this for us.

        // Uh. Gotta love this :-)
        // Check for colorkey too?
        // Leave out the loss/shift checks?
        if (32 == surface->format->BitsPerPixel && m_rgba_format.Rmask == surface->format->Rmask &&
            m_rgba_format.Gmask == surface->format->Gmask && m_rgba_format.Bmask == surface->format->Bmask &&
            m_rgba_format.Amask == surface->format->Amask && m_rgba_format.Rshift == surface->format->Rshift &&
            m_rgba_format.Gshift == surface->format->Gshift && m_rgba_format.Bshift == surface->format->Bshift &&
            m_rgba_format.Ashift == surface->format->Ashift && m_rgba_format.Rloss == surface->format->Rloss &&
            m_rgba_format.Gloss == surface->format->Gloss && m_rgba_format.Bloss == surface->format->Bloss &&
            m_rgba_format.Aloss == surface->format->Aloss) {

            return new GLImage(surface);
        }

        uint8_t const bpp          = m_rgba_format.BitsPerPixel;
        m_rgba_format.BitsPerPixel = 32;
        SDL_Surface* conv          = SDL_ConvertSurface(surface, &m_rgba_format, 0);
        m_rgba_format.BitsPerPixel = bpp;
        auto* image                = new GLImage(conv);

        SDL_FreeSurface(surface);
        return image;
    }

    Image* RenderBackendOpenGL::createImage(const std::string& name, SDL_Surface* surface)
    {
        // Given an abritary surface, we must convert it to the format GLImage will understand.
        // It's easiest to let SDL do this for us.

        // Uh. Gotta love this :-)
        // Check for colorkey too?
        // Leave out the loss/shift checks?
        if (32 == surface->format->BitsPerPixel && m_rgba_format.Rmask == surface->format->Rmask &&
            m_rgba_format.Gmask == surface->format->Gmask && m_rgba_format.Bmask == surface->format->Bmask &&
            m_rgba_format.Amask == surface->format->Amask && m_rgba_format.Rshift == surface->format->Rshift &&
            m_rgba_format.Gshift == surface->format->Gshift && m_rgba_format.Bshift == surface->format->Bshift &&
            m_rgba_format.Ashift == surface->format->Ashift && m_rgba_format.Rloss == surface->format->Rloss &&
            m_rgba_format.Gloss == surface->format->Gloss && m_rgba_format.Bloss == surface->format->Bloss &&
            m_rgba_format.Aloss == surface->format->Aloss) {

            return new GLImage(name, surface);
        }

        uint8_t const bpp          = m_rgba_format.BitsPerPixel;
        m_rgba_format.BitsPerPixel = 32;
        SDL_Surface* conv          = SDL_ConvertSurface(surface, &m_rgba_format, 0);
        m_rgba_format.BitsPerPixel = bpp;
        auto* image                = new GLImage(name, conv);

        SDL_FreeSurface(surface);
        return image;
    }

    Image* RenderBackendOpenGL::createImage(const uint8_t* data, uint32_t width, uint32_t height)
    {
        return new GLImage(data, width, height);
    }

    Image* RenderBackendOpenGL::createImage(
        const std::string& name, const uint8_t* data, uint32_t width, uint32_t height)
    {
        return new GLImage(name, data, width, height);
    }

    void RenderBackendOpenGL::setLightingModel(uint32_t lighting)
    {
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

    uint32_t RenderBackendOpenGL::getLightingModel() const
    {
        return m_state.lightmodel;
    }

    void RenderBackendOpenGL::enableTextures(uint32_t texUnit)
    {
        if (!m_state.tex_enabled[texUnit]) {
            if (m_state.active_tex != texUnit) {
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

    void RenderBackendOpenGL::disableTextures(uint32_t texUnit)
    {
        if (m_state.tex_enabled[texUnit]) {
            if (m_state.active_tex != texUnit) {
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

    void RenderBackendOpenGL::bindTexture(uint32_t texUnit, GLuint texId)
    {
        enableTextures(texUnit);

        if (m_state.texture[texUnit] != texId) {
            if (m_state.active_tex != texUnit) {
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

    void RenderBackendOpenGL::bindTexture(GLuint texId)
    {
        if (m_state.texture[m_state.active_tex] != texId) {
            m_state.texture[m_state.active_tex] = texId;
            glBindTexture(GL_TEXTURE_2D, texId);
        }
    }

    void RenderBackendOpenGL::enableLighting()
    {
        if (m_state.lightmodel != 0 && !m_state.light_enabled) {
            glEnable(GL_LIGHTING);
            m_state.light_enabled = true;
        }
    }

    void RenderBackendOpenGL::disableLighting()
    {
        if (m_state.lightmodel != 0 && m_state.light_enabled) {
            glDisable(GL_LIGHTING);
            m_state.light_enabled = false;
        }
    }

    void RenderBackendOpenGL::setLighting(float red, float green, float blue)
    {
        if (m_state.lightmodel != 0) {
            GLfloat const lightDiffuse[] = {red, green, blue, 1.0F};
            glLightfv(GL_LIGHT0, GL_DIFFUSE, &lightDiffuse[0]);
        }
    }

    void RenderBackendOpenGL::resetLighting()
    {
        if (m_state.lightmodel != 0) {
            setLighting(1.0, 1.0, 1.0);
        }
    }

    void RenderBackendOpenGL::enableStencilTest()
    {
        if (!m_state.sten_enabled) {
            glEnable(GL_STENCIL_TEST);
            m_state.sten_enabled = true;
        }
    }

    void RenderBackendOpenGL::disableStencilTest()
    {
        if (m_state.sten_enabled) {
            glDisable(GL_STENCIL_TEST);
            m_state.sten_enabled = false;
        }
    }

    void RenderBackendOpenGL::setStencilTest(uint8_t stencil_ref, GLenum stencil_op, GLenum stencil_func)
    {
        enableStencilTest();
        if (m_state.sten_op != stencil_op) {
            m_state.sten_op = stencil_op;
            glStencilOp(GL_KEEP, GL_KEEP, m_state.sten_op);
        }

        if (m_state.sten_ref != stencil_ref || m_state.sten_func != stencil_func) {
            m_state.sten_ref  = stencil_ref;
            m_state.sten_func = stencil_func;
            glStencilFunc(m_state.sten_func, stencil_ref, 0xff);
        }
    }

    void RenderBackendOpenGL::resetStencilBuffer(uint8_t buffer)
    {
        if (std::cmp_not_equal(buffer, m_state.sten_buf)) {
            m_state.sten_buf = buffer;
            glClearStencil(buffer);
        }
        disableScissorTest();
        glClear(GL_STENCIL_BUFFER_BIT);
        enableScissorTest();
    }

    uint8_t RenderBackendOpenGL::getStencilRef() const
    {
        return m_state.sten_ref;
    }

    void RenderBackendOpenGL::enableAlphaTest()
    {
        if (!m_state.alpha_enabled) {
            glEnable(GL_ALPHA_TEST);
            m_state.alpha_enabled = true;
        }
    }

    void RenderBackendOpenGL::disableAlphaTest()
    {
        if (m_state.alpha_enabled) {
            glDisable(GL_ALPHA_TEST);
            m_state.alpha_enabled = false;
        }
    }

    void RenderBackendOpenGL::setAlphaTest(float ref_alpha)
    {
        enableAlphaTest();
        if (!Mathf::Equal(m_state.alpha_test, ref_alpha)) {
            m_state.alpha_test = ref_alpha;
            glAlphaFunc(GL_GREATER, ref_alpha);
        }
    }

    void RenderBackendOpenGL::enableDepthTest()
    {
        if (!m_state.depth_enabled) {
            glEnable(GL_DEPTH_TEST);
            m_state.depth_enabled = true;
        }
    }

    void RenderBackendOpenGL::disableDepthTest()
    {
        if (m_state.depth_enabled) {
            glDisable(GL_DEPTH_TEST);
            m_state.depth_enabled = false;
        }
    }

    void RenderBackendOpenGL::enableColorArray()
    {
        if (!m_state.color_enabled) {
            glEnableClientState(GL_COLOR_ARRAY);
            m_state.color_enabled = true;
        }
    }

    void RenderBackendOpenGL::disableColorArray()
    {
        if (m_state.color_enabled) {
            glDisableClientState(GL_COLOR_ARRAY);
            glColor4ub(255, 255, 255, 255);
            m_state.color_enabled = false;
        }
    }

    void RenderBackendOpenGL::setEnvironmentalColor(uint32_t texUnit, const uint8_t* rgba)
    {
        if ((memcmp(m_state.env_color, rgba, sizeof(uint8_t) * 4) != 0) || m_state.active_tex != texUnit) {

            memcpy(m_state.env_color, rgba, sizeof(uint8_t) * 4);
            GLfloat const rgbaf[4] = {
                static_cast<float>(m_state.env_color[0]) / 255.0F,
                static_cast<float>(m_state.env_color[1]) / 255.0F,
                static_cast<float>(m_state.env_color[2]) / 255.0F,
                static_cast<float>(m_state.env_color[3]) / 255.0F};

            if (m_state.active_tex != texUnit) {
                m_state.active_tex = texUnit;
                glActiveTexture(GL_TEXTURE0 + texUnit);
                m_state.active_client_tex = texUnit;
                glClientActiveTexture(GL_TEXTURE0 + texUnit);
            }

            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &rgbaf[0]);
        }
    }

    void RenderBackendOpenGL::setVertexPointer(GLint size, GLsizei stride, const GLvoid* ptr)
    {
        if (m_state.vertex_pointer != ptr || m_state.vertex_pointer_size != size) {
            m_state.vertex_pointer      = ptr;
            m_state.vertex_pointer_size = size;
            glVertexPointer(size, GL_FLOAT, stride, ptr);
        }
    }

    void RenderBackendOpenGL::setColorPointer(GLsizei stride, const GLvoid* ptr)
    {
        if (m_state.color_pointer != ptr) {
            m_state.color_pointer = ptr;
            glColorPointer(4, GL_UNSIGNED_BYTE, stride, ptr);
        }
    }

    void RenderBackendOpenGL::setTexCoordPointer(uint32_t texUnit, GLsizei stride, const GLvoid* ptr)
    {
        if (m_state.tex_pointer[texUnit] != ptr) {
            if (m_state.active_tex != texUnit) {
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

    void RenderBackendOpenGL::enableScissorTest()
    {
        if (!m_state.scissor_test) {
            m_state.scissor_test = true;
            glEnable(GL_SCISSOR_TEST);
        }
    }

    void RenderBackendOpenGL::disableScissorTest()
    {
        if (m_state.scissor_test) {
            m_state.scissor_test = false;
            glDisable(GL_SCISSOR_TEST);
        }
    }

    void RenderBackendOpenGL::changeBlending(int32_t src, int32_t dst)
    {
        GLenum src_fact = GL_ZERO;
        GLenum dst_fact = GL_ZERO;

        switch (src) {
        case 0:
            src_fact = GL_ZERO;
            break;
        case 1:
            src_fact = GL_ONE;
            break;
        case 2:
            src_fact = GL_DST_COLOR;
            break;
        case 3:
            src_fact = GL_ONE_MINUS_DST_COLOR;
            break;
        case 4:
            src_fact = GL_SRC_ALPHA;
            break;
        case 5:
            src_fact = GL_ONE_MINUS_SRC_ALPHA;
            break;
        case 6:
            src_fact = GL_DST_ALPHA;
            break;
        case 7:
            src_fact = GL_ONE_MINUS_DST_ALPHA;
            break;

        default:
            src_fact = GL_DST_COLOR;
            break;
        }

        switch (dst) {
        case 0:
            dst_fact = GL_ZERO;
            break;
        case 1:
            dst_fact = GL_ONE;
            break;
        case 2:
            dst_fact = GL_SRC_COLOR;
            break;
        case 3:
            dst_fact = GL_ONE_MINUS_SRC_COLOR;
            break;
        case 4:
            dst_fact = GL_SRC_ALPHA;
            break;
        case 5:
            dst_fact = GL_ONE_MINUS_SRC_ALPHA;
            break;
        case 6:
            dst_fact = GL_DST_ALPHA;
            break;
        case 7:
            dst_fact = GL_ONE_MINUS_DST_ALPHA;
            break;

        default:
            dst_fact = GL_SRC_ALPHA;
            break;
        }

        if (m_state.blend_src != src_fact || m_state.blend_dst != dst_fact) {
            m_state.blend_src = src_fact;
            m_state.blend_dst = dst_fact;
            glBlendFunc(src_fact, dst_fact);
        }
    }

    void RenderBackendOpenGL::changeRenderInfos(
        RenderDataType type,
        uint16_t elements,
        int32_t src,
        int32_t dst,
        bool light,
        bool stentest,
        uint8_t stenref,
        GLConstants stenop,
        GLConstants stenfunc,
        OverlayType otype)
    {

        uint16_t count = 0;
        switch (type) {
        case RENDER_DATA_WITHOUT_Z: {
            const std::size_t size = m_renderObjects.size();
            while (count != elements) {
                ++count;
                RenderObject& r = m_renderObjects.at(size - count);

                r.src          = src;
                r.dst          = dst;
                r.light        = light;
                r.overlay_type = otype;
                if (stentest) {
                    r.stencil_test = stentest;
                    r.stencil_ref  = stenref;
                    r.stencil_op   = stenop;
                    r.stencil_func = stenfunc;
                }
            }
        } break;
        case RENDER_DATA_TEXTURE_Z: {
            // not needed currently
        } break;
        case RENDER_DATA_TEXCOLOR_Z: {
            // not needed currently
        } break;
        case RENDER_DATA_MULTITEXTURE_Z: {
            const std::size_t size = m_renderMultitextureObjectsZ.size();
            while (count != elements) {
                ++count;
                RenderObject& r = m_renderMultitextureObjectsZ.at(size - count);
                // only overlay_type is important
                r.src          = src;
                r.dst          = dst;
                r.light        = light;
                r.overlay_type = otype;
                if (stentest) {
                    r.stencil_test = stentest;
                    r.stencil_ref  = stenref;
                    r.stencil_op   = stenop;
                    r.stencil_func = stenfunc;
                }
            }
        } break;
        default: {
            // nothing
        } break;
        }
    }

    void RenderBackendOpenGL::renderWithoutZ()
    {
        // bools to indicate changes
        bool type     = false;
        bool texture  = false;
        bool blending = false;
        bool light    = false;
        bool stencil  = false;
        bool color    = false;
        bool mt       = false;
        bool render   = false;

        // render mode
        GLenum mode = GL_TRIANGLES;
        // texture id
        uint32_t texture_id  = 0;
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
        int32_t indexP   = 0;
        int32_t indexT   = 0;
        int32_t indexTC  = 0;
        int32_t index2TC = 0;
        // elements to render
        uint32_t elementsP   = 0;
        uint32_t elementsT   = 0;
        uint32_t elementsTC  = 0;
        uint32_t elements2TC = 0;

        int32_t* currentIndex     = nullptr;
        uint32_t* currentElements = nullptr;

        // index buffer pointer
        uint32_t* indexBuffer = nullptr;

        // stride
        const uint32_t strideP   = sizeof(renderDataP);
        const uint32_t strideT   = sizeof(renderDataT);
        const uint32_t strideTC  = sizeof(renderDataTC);
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
                indexBuffer     = m_tIndices.data();
                currentIndex    = &indexT;
                currentElements = &elementsT;
                // texture with color/alpha
            } else if (m_renderObjects[0].texture_id != 0) {
                // set pointer
                enableColorArray();
                setVertexPointer(2, strideTC, &m_renderTextureColorDatas[0].vertex);
                setTexCoordPointer(0, strideTC, &m_renderTextureColorDatas[0].texel);
                setColorPointer(strideTC, &m_renderTextureColorDatas[0].color);
                indexBuffer     = m_tcIndices.data();
                currentIndex    = &indexTC;
                currentElements = &elementsTC;
                // primitive
            } else {
                // set pointer
                enableColorArray();
                setVertexPointer(2, strideP, &m_renderPrimitiveDatas[0].vertex);
                setColorPointer(strideP, &m_renderPrimitiveDatas[0].color);
                indexBuffer     = m_pIndices.data();
                currentIndex    = &indexP;
                currentElements = &elementsP;
            }
            // multitexture overlay
        } else {
            // set pointer
            setVertexPointer(2, stride2TC, &m_renderMultitextureDatas[0].vertex);
            setColorPointer(stride2TC, &m_renderMultitextureDatas[0].color);
            setTexCoordPointer(0, stride2TC, &m_renderMultitextureDatas[0].texel);
            indexBuffer     = m_tc2Indices.data();
            currentIndex    = &index2TC;
            currentElements = &elements2TC;
        }

        for (auto& ro : m_renderObjects) {
            // first we look for changes
            if (ro.mode != mode) {
                type   = true;
                render = true;
            } else if (ro.mode == GL_LINE_STRIP || ro.mode == GL_LINE_LOOP) {
                // do not batch line strips, loops or triangle fans to avoid side effects
                render = true;
            }
            if (ro.texture_id != texture_id) {
                texture = true;
                render  = true;
            }
            if (ro.color != m_state.color_enabled) {
                color  = true;
                render = true;
            }
            if (ro.overlay_type != overlay_type ||
                (ro.overlay_type != OVERLAY_TYPE_NONE &&
                 ((memcmp(rgba, ro.rgba, sizeof(uint8_t) * 4) != 0) || ro.overlay_id != texture_id2))) {
                mt     = true;
                render = true;
            }
            if (m_state.lightmodel != 0U) {
                if (ro.src != src || ro.dst != dst) {
                    blending = true;
                    render   = true;
                }
                if (ro.light != m_state.light_enabled) {
                    light  = true;
                    render = true;
                }
                if (ro.stencil_test != m_state.sten_enabled) {
                    stencil = true;
                    render  = true;
                } else if (ro.stencil_test) {
                    if (ro.stencil_ref != m_state.sten_ref || ro.stencil_op != m_state.sten_op ||
                        ro.stencil_func != m_state.sten_func) {
                        stencil = true;
                        render  = true;
                    }
                }
            }

            // if changes then we render all previously elements
            if (render) {
                if (*currentElements > 0) {
                    // render
                    glDrawElements(mode, toGLsizei(*currentElements), GL_UNSIGNED_INT, indexBuffer + *currentIndex);
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
                                indexBuffer     = m_tcIndices.data();
                                currentElements = &elementsTC;
                                currentIndex    = &indexTC;
                            } else {
                                setVertexPointer(2, strideP, &m_renderPrimitiveDatas[0].vertex);
                                setColorPointer(strideP, &m_renderPrimitiveDatas[0].color);
                                indexBuffer     = m_pIndices.data();
                                currentElements = &elementsP;
                                currentIndex    = &indexP;
                            }
                        }
                    } else if (!ro.color && m_state.color_enabled) {
                        disableColorArray();
                        if (ro.overlay_type == OVERLAY_TYPE_NONE) {
                            setVertexPointer(2, strideT, &m_renderTextureDatas[0].vertex);
                            setTexCoordPointer(0, strideT, &m_renderTextureDatas[0].texel);
                            indexBuffer     = m_tIndices.data();
                            currentElements = &elementsT;
                            currentIndex    = &indexT;
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
                                indexBuffer     = m_tcIndices.data();
                                currentElements = &elementsTC;
                                currentIndex    = &indexTC;
                            } else {
                                setVertexPointer(2, strideT, &m_renderTextureDatas[0].vertex);
                                setTexCoordPointer(0, strideT, &m_renderTextureDatas[0].texel);
                                indexBuffer     = m_tIndices.data();
                                currentIndex    = &indexT;
                                currentElements = &elementsT;
                            }
                        } else {
                            setVertexPointer(2, strideP, &m_renderPrimitiveDatas[0].vertex);
                            setColorPointer(strideP, &m_renderPrimitiveDatas[0].color);
                            indexBuffer     = m_pIndices.data();
                            currentElements = &elementsP;
                            currentIndex    = &indexP;
                        }
                        texture_id2 = 0;
                        break;
                    case OVERLAY_TYPE_COLOR:
                        disableTextures(3);
                        disableTextures(2);
                        bindTexture(1, m_maskOverlay);
                        setEnvironmentalColor(1, &ro.rgba[0]);
                        enableTextures(0);

                        // set pointer
                        setVertexPointer(2, stride2TC, &m_renderMultitextureDatas[0].vertex);
                        setColorPointer(stride2TC, &m_renderMultitextureDatas[0].color);
                        setTexCoordPointer(1, stride2TC, &m_renderMultitextureDatas[0].texel2);
                        setTexCoordPointer(0, stride2TC, &m_renderMultitextureDatas[0].texel);
                        indexBuffer = m_tc2Indices.data();

                        texture_id2     = m_maskOverlay;
                        currentElements = &elements2TC;
                        currentIndex    = &index2TC;
                        break;
                    case OVERLAY_TYPE_COLOR_AND_TEXTURE:
                        disableTextures(3);
                        disableTextures(1);
                        bindTexture(2, ro.overlay_id);
                        setEnvironmentalColor(2, &ro.rgba[0]);
                        enableTextures(0);

                        // set pointer
                        setVertexPointer(2, stride2TC, &m_renderMultitextureDatas[0].vertex);
                        setColorPointer(stride2TC, &m_renderMultitextureDatas[0].color);
                        setTexCoordPointer(2, stride2TC, &m_renderMultitextureDatas[0].texel2);
                        setTexCoordPointer(0, stride2TC, &m_renderMultitextureDatas[0].texel);
                        indexBuffer = m_tc2Indices.data();

                        texture_id2     = ro.overlay_id;
                        currentElements = &elements2TC;
                        currentIndex    = &index2TC;
                        break;
                    case OVERLAY_TYPE_TEXTURES_AND_FACTOR:
                        disableTextures(2);
                        disableTextures(1);
                        bindTexture(3, ro.overlay_id);
                        setEnvironmentalColor(3, &ro.rgba[0]);
                        enableTextures(0);

                        // set pointer
                        setVertexPointer(2, stride2TC, &m_renderMultitextureDatas[0].vertex);
                        setColorPointer(stride2TC, &m_renderMultitextureDatas[0].color);
                        setTexCoordPointer(3, stride2TC, &m_renderMultitextureDatas[0].texel2);
                        setTexCoordPointer(0, stride2TC, &m_renderMultitextureDatas[0].texel);
                        indexBuffer = m_tc2Indices.data();

                        texture_id2     = ro.overlay_id;
                        currentElements = &elements2TC;
                        currentIndex    = &index2TC;
                        break;
                    }
                    memcpy(rgba, ro.rgba, sizeof(uint8_t) * 4);
                    overlay_type = ro.overlay_type;
                    mt           = false;
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
                                indexBuffer     = m_tcIndices.data();
                                currentElements = &elementsTC;
                                currentIndex    = &indexTC;
                            } else {
                                setVertexPointer(2, strideT, &m_renderTextureDatas[0].vertex);
                                setTexCoordPointer(0, strideT, &m_renderTextureDatas[0].texel);
                                indexBuffer     = m_tIndices.data();
                                currentElements = &elementsT;
                                currentIndex    = &indexT;
                            }
                        }
                    } else {
                        disableTextures(0);
                        texture_id = 0;
                        if (ro.overlay_type == OVERLAY_TYPE_NONE) {
                            setVertexPointer(2, strideP, &m_renderPrimitiveDatas[0].vertex);
                            setColorPointer(strideP, &m_renderPrimitiveDatas[0].color);
                            indexBuffer     = m_pIndices.data();
                            currentElements = &elementsP;
                            currentIndex    = &indexP;
                        }
                    }
                    texture = false;
                }

                // set element to current size
                *currentElements = ro.size;

                // if lighting is enabled we have to consider a few more values
                if (m_state.lightmodel != 0U) {
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
        glDrawElements(mode, toGLsizei(*currentElements), GL_UNSIGNED_INT, indexBuffer + *currentIndex);

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

        m_pIndices.clear();
        m_tIndices.clear();
        m_tcIndices.clear();
        m_tc2Indices.clear();
    }

    void RenderBackendOpenGL::renderWithZ()
    {
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

        int32_t* currentIndex     = &index;
        uint32_t* currentElements = &elements;

        enableAlphaTest();
        enableDepthTest();
        enableTextures(0);
        enableLighting();
        disableColorArray();

        for (auto& ro : m_renderTextureObjectsZ) {
            // if changes then we render all previously elements
            if (ro.texture_id != texture_id) {
                if (*currentElements > 0) {
                    // render
                    glDrawElements(
                        GL_TRIANGLES,
                        toGLsizei(*currentElements),
                        GL_UNSIGNED_INT,
                        &m_indices[toSizeTIndex(*currentIndex)]);
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
                *currentElements = 6;
            } else {
                // else add the element
                *currentElements += 6;
            }
        }

        // render
        glDrawElements(
            GL_TRIANGLES, toGLsizei(*currentElements), GL_UNSIGNED_INT, &m_indices[toSizeTIndex(*currentIndex)]);

        // reset all states
        disableLighting();
        disableTextures(0);
        disableAlphaTest();
        disableDepthTest();
        enableColorArray();

        m_renderTextureDatasZ.clear();
        m_renderTextureObjectsZ.clear();
    }

    void RenderBackendOpenGL::renderWithZTest()
    {
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

        auto iter = m_renderZ_objects.begin();
        for (; iter != m_renderZ_objects.end(); ++iter) {
            bindTexture(iter->texture_id);
            glDrawArrays(GL_QUADS, toGLint(iter->index), toGLsizei(iter->elements));
        }
        m_renderZ_objects.clear();

        // reset all states
        disableLighting();
        disableTextures(0);
        disableAlphaTest();
        disableDepthTest();
        enableColorArray();
    }

    void RenderBackendOpenGL::renderWithColorAndZ()
    {
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

        int32_t* currentIndex     = &index;
        uint32_t* currentElements = &elements;

        enableDepthTest();
        // use own value, other option would be to disable it
        setAlphaTest(0.008F);
        enableTextures(0);
        enableLighting();

        for (auto& ro : m_renderTextureColorObjectsZ) {
            // if changes then we render all previously elements
            if (ro.texture_id != texture_id) {
                if (*currentElements > 0) {
                    // render
                    glDrawElements(
                        GL_TRIANGLES,
                        toGLsizei(*currentElements),
                        GL_UNSIGNED_INT,
                        &m_indices[toSizeTIndex(*currentIndex)]);
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
                *currentElements = 6;
            } else {
                // else add the element
                *currentElements += 6;
            }
        }

        // render
        glDrawElements(
            GL_TRIANGLES, toGLsizei(*currentElements), GL_UNSIGNED_INT, &m_indices[toSizeTIndex(*currentIndex)]);

        // reset all states
        disableLighting();
        disableTextures(0);
        setAlphaTest(m_alphaValue);
        disableAlphaTest();
        disableDepthTest();

        m_renderTextureColorDatasZ.clear();
        m_renderTextureColorObjectsZ.clear();
    }

    void RenderBackendOpenGL::renderWithMultitextureAndZ()
    {
        // bools to indicate changes
        bool texture = false;
        bool render  = false;
        bool mt      = false;

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
        uint32_t texture_id  = 0;
        uint32_t texture_id2 = 0;
        // overlay type
        OverlayType overlay_type = OVERLAY_TYPE_NONE;
        // overlay color
        uint8_t color[4] = {0};

        int32_t* currentIndex     = &index;
        uint32_t* currentElements = &elements;

        enableDepthTest();
        enableAlphaTest();
        enableTextures(0);
        enableLighting();

        for (auto& ro : m_renderMultitextureObjectsZ) {
            // first we look for changes
            if (ro.texture_id != texture_id) {
                texture = true;
                render  = true;
            }
            if (ro.overlay_type != overlay_type ||
                (ro.overlay_type != OVERLAY_TYPE_NONE &&
                 ((memcmp(color, ro.rgba, sizeof(uint8_t) * 4) != 0) || ro.overlay_id != texture_id2))) {
                mt     = true;
                render = true;
            }

            // if changes then we render all previously elements
            if (render) {
                if (*currentElements > 0) {
                    // render
                    glDrawElements(
                        GL_TRIANGLES,
                        toGLsizei(*currentElements),
                        GL_UNSIGNED_INT,
                        &m_indices[toSizeTIndex(*currentIndex)]);
                    *currentIndex += *currentElements;
                }
                // multitexturing
                if (mt) {
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
                        setEnvironmentalColor(1, &ro.rgba[0]);
                        enableTextures(0);

                        texture_id2 = m_maskOverlay;
                        break;
                    case OVERLAY_TYPE_COLOR_AND_TEXTURE:
                        disableTextures(3);
                        disableTextures(1);
                        bindTexture(2, ro.overlay_id);
                        setEnvironmentalColor(2, &ro.rgba[0]);
                        enableTextures(0);

                        texture_id2 = ro.overlay_id;
                        break;
                    case OVERLAY_TYPE_TEXTURES_AND_FACTOR:
                        disableTextures(2);
                        disableTextures(1);
                        bindTexture(3, ro.overlay_id);
                        setEnvironmentalColor(3, &ro.rgba[0]);
                        enableTextures(0);

                        texture_id2 = ro.overlay_id;
                        break;
                    }
                    memcpy(color, ro.rgba, sizeof(uint8_t) * 4);
                    overlay_type = ro.overlay_type;
                    mt           = false;
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
                render           = false;
            } else {
                // else add the element
                *currentElements += ro.size;
            }
        }
        // render
        glDrawElements(
            GL_TRIANGLES, toGLsizei(*currentElements), GL_UNSIGNED_INT, &m_indices[toSizeTIndex(*currentIndex)]);

        // reset all states
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

    void RenderBackendOpenGL::renderVertexArrays()
    {
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

    bool RenderBackendOpenGL::putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if ((x < 0) || (x >= static_cast<int32_t>(m_target->w)) || (y < 0) ||
            (y >= static_cast<int32_t>(m_target->h))) {
            return false;
        }
        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(x) + 0.375F;
        rd.vertex[1] = static_cast<float>(y) + 0.375F;
        rd.color[0]  = r;
        rd.color[1]  = g;
        rd.color[2]  = b;
        rd.color[3]  = a;
        m_renderPrimitiveDatas.push_back(rd);

        m_pIndices.push_back(m_pIndices.empty() ? 0 : m_pIndices.back() + 1);

        RenderObject const ro(GL_POINTS, 1);
        m_renderObjects.push_back(ro);

        return true;
    }

    void RenderBackendOpenGL::drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(p1.x) + 0.375F;
        rd.vertex[1] = static_cast<float>(p1.y) + 0.375F;
        rd.color[0]  = r;
        rd.color[1]  = g;
        rd.color[2]  = b;
        rd.color[3]  = a;
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[0] = static_cast<float>(p2.x) + 0.375F;
        rd.vertex[1] = static_cast<float>(p2.y) + 0.375F;
        m_renderPrimitiveDatas.push_back(rd);

        m_pIndices.push_back(m_pIndices.empty() ? 0 : m_pIndices.back() + 1);
        m_pIndices.push_back(m_pIndices.back() + 1);

        RenderObject const ro(GL_LINES, 2);
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::drawThickLine(
        const Point& p1, const Point& p2, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        const float xDiff = static_cast<float>(p2.x - p1.x);
        const float yDiff = static_cast<float>(p2.y - p1.y);
        const float halfW = static_cast<float>(width) / 2.0F;
        float angle       = (Mathf::ATan2(yDiff, xDiff) * (180.0F / Mathf::pi())) + 90.0F;
        if (angle < 0.0F) {
            angle += 360.0F;
        } else if (angle > 360.0F) {
            angle -= 360.0F;
        }
        angle *= Mathf::pi() / 180.0F;
        const float cornerX = halfW * Mathf::Cos(angle);
        const float cornerY = halfW * Mathf::Sin(angle);

        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(p1.x) + cornerX;
        rd.vertex[1] = static_cast<float>(p1.y) + cornerY;
        rd.color[0]  = r;
        rd.color[1]  = g;
        rd.color[2]  = b;
        rd.color[3]  = a;
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

        uint32_t const index = m_pIndices.empty() ? 0 : m_pIndices.back() + 1;
        uint32_t indices[]   = {index, index + 1, index + 2, index, index + 2, index + 3};
        m_pIndices.insert(m_pIndices.end(), indices, indices + 6);

        RenderObject const ro(GL_TRIANGLES, 6);
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::drawPolyLine(
        const std::vector<Point>& points, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (points.size() < 2) {
            return;
        }
        auto it = points.begin();
        if (width > 1) {
            const uint32_t capRadius = static_cast<uint32_t>(width / 2U);
            Point old                = *it;
            ++it;
            for (; it != points.end(); ++it) {
                drawThickLine(old, *it, width, r, g, b, a);
                drawFillCircle(old, capRadius, r, g, b, a);
                old = *it;
            }
            drawFillCircle(old, capRadius, r, g, b, a);
        } else {
            renderDataP rd{};
            rd.color[0] = r;
            rd.color[1] = g;
            rd.color[2] = b;
            rd.color[3] = a;
            for (; it != points.end(); ++it) {
                rd.vertex[0] = static_cast<float>((*it).x);
                rd.vertex[1] = static_cast<float>((*it).y);
                m_renderPrimitiveDatas.push_back(rd);
                m_pIndices.push_back(m_pIndices.empty() ? 0 : m_pIndices.back() + 1);
            }
            RenderObject const ro(GL_LINE_STRIP, toRenderObjectSize(points.size()));
            m_renderObjects.push_back(ro);
        }
    }

    void RenderBackendOpenGL::drawBezier(
        const std::vector<Point>& points, int32_t steps, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (points.size() < 2) {
            return;
        }
        const int32_t elements = toInt32Dimension(static_cast<uint32_t>(points.size()));
        if (elements < 3 || steps < 2) {
            return;
        }

        bool const thick = width > 1;
        const float step = 1.0F / static_cast<float>(steps - 1);
        float t          = 0.0F;
        Point old        = getBezierPoint(points, elements + 1, t);
        if (thick) {
            const uint32_t capRadius = static_cast<uint32_t>(width / 2U);
            for (int32_t i = 0; i <= (elements * steps); ++i) {
                t += step;
                Point const next = getBezierPoint(points, elements, t);
                drawThickLine(old, next, width, r, g, b, a);
                drawFillCircle(old, capRadius, r, g, b, a);
                old = next;
            }
            drawFillCircle(old, capRadius, r, g, b, a);
        } else {
            renderDataP rd{};
            rd.color[0] = r;
            rd.color[1] = g;
            rd.color[2] = b;
            rd.color[3] = a;
            for (int32_t i = 0; i <= (elements * steps); ++i) {
                t += step;
                Point const next = getBezierPoint(points, elements, t);
                rd.vertex[0]     = static_cast<float>(old.x);
                rd.vertex[1]     = static_cast<float>(old.y);
                m_renderPrimitiveDatas.push_back(rd);
                old = next;
                m_pIndices.push_back(m_pIndices.empty() ? 0 : m_pIndices.back() + 1);
            }
            RenderObject const ro(GL_LINE_STRIP, toRenderObjectSize((elements * steps) + 1));
            m_renderObjects.push_back(ro);
        }
    }

    void RenderBackendOpenGL::drawTriangle(
        const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(p1.x);
        rd.vertex[1] = static_cast<float>(p1.y);
        rd.color[0]  = r;
        rd.color[1]  = g;
        rd.color[2]  = b;
        rd.color[3]  = a;
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[0] = static_cast<float>(p2.x);
        rd.vertex[1] = static_cast<float>(p2.y);
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[0] = static_cast<float>(p3.x);
        rd.vertex[1] = static_cast<float>(p3.y);
        m_renderPrimitiveDatas.push_back(rd);

        uint32_t const index = m_pIndices.empty() ? 0 : m_pIndices.back() + 1;
        uint32_t indices[]   = {index, index + 1, index + 2};
        m_pIndices.insert(m_pIndices.end(), indices, indices + 3);

        RenderObject const ro(GL_TRIANGLES, 3);
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::drawRectangle(
        const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(p.x);
        rd.vertex[1] = static_cast<float>(p.y);
        rd.color[0]  = r;
        rd.color[1]  = g;
        rd.color[2]  = b;
        rd.color[3]  = a;
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[0] = static_cast<float>(p.x + w);
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[1] = static_cast<float>(p.y + h);
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[0] = static_cast<float>(p.x);
        m_renderPrimitiveDatas.push_back(rd);

        uint32_t const index = m_pIndices.empty() ? 0 : m_pIndices.back() + 1;
        uint32_t indices[]   = {index, index + 1, index + 2, index + 3};
        m_pIndices.insert(m_pIndices.end(), indices, indices + 4);

        RenderObject const ro(GL_LINE_LOOP, 4);
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::fillRectangle(
        const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(p.x);
        rd.vertex[1] = static_cast<float>(p.y);
        rd.color[0]  = r;
        rd.color[1]  = g;
        rd.color[2]  = b;
        rd.color[3]  = a;
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[1] = static_cast<float>(p.y + h);
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[0] = static_cast<float>(p.x + w);
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[1] = static_cast<float>(p.y);
        m_renderPrimitiveDatas.push_back(rd);

        uint32_t const index = m_pIndices.empty() ? 0 : m_pIndices.back() + 1;
        uint32_t indices[]   = {index, index + 1, index + 2, index, index + 2, index + 3};
        m_pIndices.insert(m_pIndices.end(), indices, indices + 6);

        RenderObject const ro(GL_TRIANGLES, 6);
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::drawQuad(
        const Point& p1, const Point& p2, const Point& p3, const Point& p4, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(p1.x);
        rd.vertex[1] = static_cast<float>(p1.y);
        rd.color[0]  = r;
        rd.color[1]  = g;
        rd.color[2]  = b;
        rd.color[3]  = a;
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

        uint32_t const index = m_pIndices.empty() ? 0 : m_pIndices.back() + 1;
        uint32_t indices[]   = {index, index + 1, index + 2, index, index + 2, index + 3};
        m_pIndices.insert(m_pIndices.end(), indices, indices + 6);

        RenderObject const ro(GL_TRIANGLES, 6);
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(p.x - size);
        rd.vertex[1] = static_cast<float>(p.y + size);
        rd.color[0]  = r;
        rd.color[1]  = g;
        rd.color[2]  = b;
        rd.color[3]  = a;
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[0] = static_cast<float>(p.x + size);
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[1] = static_cast<float>(p.y - size);
        m_renderPrimitiveDatas.push_back(rd);

        rd.vertex[0] = static_cast<float>(p.x - size);
        m_renderPrimitiveDatas.push_back(rd);

        uint32_t const index = m_pIndices.empty() ? 0 : m_pIndices.back() + 1;
        uint32_t indices[]   = {index, index + 1, index + 2, index + 3};
        m_pIndices.insert(m_pIndices.end(), indices, indices + 4);

        RenderObject const ro(GL_LINE_LOOP, 4);
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::drawCircle(const Point& p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        // set side length to 5 and calculate needed divisions
        int32_t subdivisions = static_cast<int32_t>(round(Mathf::pi() / (5.0F / (2.0F * static_cast<float>(radius)))));
        subdivisions         = std::max(subdivisions, 12);
        const float radiusF  = static_cast<float>(radius);
        const float step     = Mathf::twoPi() / static_cast<float>(subdivisions);
        float angle          = 0.0F;

        renderDataP rd{};
        rd.color[0] = r;
        rd.color[1] = g;
        rd.color[2] = b;
        rd.color[3] = a;
        for (int32_t i = 0; i < subdivisions - 1; ++i) {
            rd.vertex[0] = radiusF * Mathf::Cos(angle) + static_cast<float>(p.x);
            rd.vertex[1] = radiusF * Mathf::Sin(angle) + static_cast<float>(p.y);
            m_renderPrimitiveDatas.push_back(rd);
            angle += step;
            m_pIndices.push_back(m_pIndices.empty() ? 0 : m_pIndices.back() + 1);
        }

        RenderObject const ro(GL_LINE_LOOP, toRenderObjectSize(subdivisions - 1));
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::drawFillCircle(
        const Point& p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        // set side length to 5 and calculate needed divisions
        int32_t subdivisions = static_cast<int32_t>(round(Mathf::pi() / (5.0F / (2.0F * static_cast<float>(radius)))));
        subdivisions         = std::max(subdivisions, 12);
        const float radiusF  = static_cast<float>(radius);
        const float step     = Mathf::twoPi() / static_cast<float>(subdivisions);
        float angle          = Mathf::twoPi();
        uint32_t const index = m_pIndices.empty() ? 0 : m_pIndices.back() + 1;
        uint32_t lastIndex   = index;

        // center vertex
        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(p.x);
        rd.vertex[1] = static_cast<float>(p.y);
        rd.color[0]  = r;
        rd.color[1]  = g;
        rd.color[2]  = b;
        rd.color[3]  = a;
        m_renderPrimitiveDatas.push_back(rd);
        // reversed because of culling faces
        for (int32_t i = 0; i <= subdivisions; ++i) {
            rd.vertex[0] = radiusF * Mathf::Cos(angle) + static_cast<float>(p.x);
            rd.vertex[1] = radiusF * Mathf::Sin(angle) + static_cast<float>(p.y);
            m_renderPrimitiveDatas.push_back(rd);
            angle -= step;
            // forms triangle with start index, the last and a new one
            uint32_t indices[] = {index, lastIndex, ++lastIndex};
            m_pIndices.insert(m_pIndices.end(), indices, indices + 3);
        }
        RenderObject const ro(GL_TRIANGLES, toRenderObjectSize((subdivisions + 1) * 3));
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::drawCircleSegment(
        const Point& p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        const float radiusF = static_cast<float>(radius);
        const float step    = Mathf::twoPi() / 360.0F;
        int32_t elements    = 0;
        int32_t s           = (sangle + 360) % 360;
        int32_t e           = (eangle + 360) % 360;
        if (e == 0) {
            e = 360;
        }
        if (s == e) {
            return;
        }

        renderDataP rd{};
        rd.color[0] = r;
        rd.color[1] = g;
        rd.color[2] = b;
        rd.color[3] = a;
        float angle = static_cast<float>(s) * step;
        for (; s <= e; ++s, angle += step, ++elements) {
            rd.vertex[0] = radiusF * Mathf::Cos(angle) + static_cast<float>(p.x);
            rd.vertex[1] = radiusF * Mathf::Sin(angle) + static_cast<float>(p.y);
            m_renderPrimitiveDatas.push_back(rd);
            m_pIndices.push_back(m_pIndices.empty() ? 0 : m_pIndices.back() + 1);
        }

        RenderObject const ro(GL_LINE_STRIP, toRenderObjectSize(elements));
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::drawFillCircleSegment(
        const Point& p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        const float radiusF = static_cast<float>(radius);
        const float step    = Mathf::twoPi() / 360.0F;
        int32_t s           = (sangle + 360) % 360;
        int32_t e           = (eangle + 360) % 360;
        if (e == 0) {
            e = 360;
        }
        if (s == e) {
            return;
        }

        uint32_t const index = m_pIndices.empty() ? 0 : m_pIndices.back() + 1;
        uint32_t lastIndex   = index;
        // center vertex
        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(p.x);
        rd.vertex[1] = static_cast<float>(p.y);
        rd.color[0]  = r;
        rd.color[1]  = g;
        rd.color[2]  = b;
        rd.color[3]  = a;
        m_renderPrimitiveDatas.push_back(rd);
        int32_t elements = 0;
        // reversed because of culling faces
        float angle = static_cast<float>(e) * step;
        for (; s <= e; ++s, angle -= step, ++elements) {
            rd.vertex[0] = radiusF * Mathf::Cos(angle) + static_cast<float>(p.x);
            rd.vertex[1] = radiusF * Mathf::Sin(angle) + static_cast<float>(p.y);

            m_renderPrimitiveDatas.push_back(rd);
            // forms triangle with start index, the last and a new one
            uint32_t indices[] = {index, lastIndex, ++lastIndex};
            m_pIndices.insert(m_pIndices.end(), indices, indices + 3);
        }

        RenderObject const ro(GL_TRIANGLES, toRenderObjectSize(elements * 3));
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::drawLightPrimitive(
        const Point& p,
        uint8_t intensity,
        float radius,
        int32_t subdivisions,
        float xstretch,
        float ystretch,
        uint8_t red,
        uint8_t green,
        uint8_t blue)
    {
        const float step     = Mathf::twoPi() / static_cast<float>(subdivisions);
        uint32_t elements    = 0;
        uint32_t const index = m_pIndices.empty() ? 0 : m_pIndices.back() + 1;
        uint32_t lastIndex   = index;
        // center vertex
        renderDataP rd{};
        rd.vertex[0] = static_cast<float>(p.x);
        rd.vertex[1] = static_cast<float>(p.y);
        rd.color[0]  = red;
        rd.color[1]  = green;
        rd.color[2]  = blue;
        rd.color[3]  = intensity;
        m_renderPrimitiveDatas.push_back(rd);
        for (float angle = 0; angle <= Mathf::twoPi(); angle += step, elements += 3) {
            rd.vertex[0] = radius * Mathf::Cos(angle + step) * xstretch + static_cast<float>(p.x);
            rd.vertex[1] = radius * Mathf::Sin(angle + step) * ystretch + static_cast<float>(p.y);
            rd.color[0]  = 0;
            rd.color[1]  = 0;
            rd.color[2]  = 0;
            rd.color[3]  = 255;
            m_renderPrimitiveDatas.push_back(rd);

            rd.vertex[0] = radius * Mathf::Cos(angle) * xstretch + static_cast<float>(p.x);
            rd.vertex[1] = radius * Mathf::Sin(angle) * ystretch + static_cast<float>(p.y);
            m_renderPrimitiveDatas.push_back(rd);
            // forms triangle with start index and two new ones
            uint32_t indices[] = {index, ++lastIndex, ++lastIndex};
            m_pIndices.insert(m_pIndices.end(), indices, indices + 3);
        }
        RenderObject const ro(GL_TRIANGLES, toRenderObjectSize(elements));
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::addImageToArray(
        uint32_t id, const Rect& rect, float const * st, uint8_t alpha, uint8_t const * rgba)
    {
        RenderObject ro(GL_TRIANGLES, 6, id);

        // texture quad without alpha
        if (alpha == 255 && (rgba == nullptr)) {
            renderDataT rd{};
            rd.vertex[0] = static_cast<float>(rect.x);
            rd.vertex[1] = static_cast<float>(rect.y);
            rd.texel[0]  = st[0];
            rd.texel[1]  = st[1];
            m_renderTextureDatas.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x);
            rd.vertex[1] = static_cast<float>(rect.y + rect.h);
            rd.texel[1]  = st[3];
            m_renderTextureDatas.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x + rect.w);
            rd.vertex[1] = static_cast<float>(rect.y + rect.h);
            rd.texel[0]  = st[2];
            m_renderTextureDatas.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x + rect.w);
            rd.vertex[1] = static_cast<float>(rect.y);
            rd.texel[1]  = st[1];
            m_renderTextureDatas.push_back(rd);

            ro.color = false;

            uint32_t const index = m_tIndices.empty() ? 0 : m_tIndices.back() + 1;
            uint32_t indices[]   = {index, index + 1, index + 2, index, index + 2, index + 3};
            m_tIndices.insert(m_tIndices.end(), indices, indices + 6);
        } else {
            if (rgba != nullptr) {
                renderData2TC rd{};
                rd.vertex[0] = static_cast<float>(rect.x);
                rd.vertex[1] = static_cast<float>(rect.y);
                rd.texel[0]  = st[0];
                rd.texel[1]  = st[1];
                rd.texel2[0] = 0.0;
                rd.texel2[1] = 0.0;
                rd.color[0]  = 255;
                rd.color[1]  = 255;
                rd.color[2]  = 255;
                rd.color[3]  = alpha;
                m_renderMultitextureDatas.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x);
                rd.vertex[1] = static_cast<float>(rect.y + rect.h);
                rd.texel[1]  = st[3];
                rd.texel2[1] = 1.0;
                m_renderMultitextureDatas.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x + rect.w);
                rd.vertex[1] = static_cast<float>(rect.y + rect.h);
                rd.texel[0]  = st[2];
                rd.texel2[0] = 1.0;
                m_renderMultitextureDatas.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x + rect.w);
                rd.vertex[1] = static_cast<float>(rect.y);
                rd.texel[1]  = st[1];
                rd.texel2[1] = 0.0;
                m_renderMultitextureDatas.push_back(rd);

                ro.color        = true;
                ro.overlay_type = OVERLAY_TYPE_COLOR;
                ro.rgba[0]      = rgba[0];
                ro.rgba[1]      = rgba[1];
                ro.rgba[2]      = rgba[2];
                ro.rgba[3]      = rgba[3];

                uint32_t const index = m_tc2Indices.empty() ? 0 : m_tc2Indices.back() + 1;
                uint32_t indices[]   = {index, index + 1, index + 2, index, index + 2, index + 3};
                m_tc2Indices.insert(m_tc2Indices.end(), indices, indices + 6);
                // texture quad with alpha
            } else {
                renderDataTC rd{};
                rd.vertex[0] = static_cast<float>(rect.x);
                rd.vertex[1] = static_cast<float>(rect.y);
                rd.texel[0]  = st[0];
                rd.texel[1]  = st[1];
                rd.color[0]  = 255;
                rd.color[1]  = 255;
                rd.color[2]  = 255;
                rd.color[3]  = alpha;
                m_renderTextureColorDatas.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x);
                rd.vertex[1] = static_cast<float>(rect.y + rect.h);
                rd.texel[1]  = st[3];
                m_renderTextureColorDatas.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x + rect.w);
                rd.vertex[1] = static_cast<float>(rect.y + rect.h);
                rd.texel[0]  = st[2];
                m_renderTextureColorDatas.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x + rect.w);
                rd.vertex[1] = static_cast<float>(rect.y);
                rd.texel[1]  = st[1];
                m_renderTextureColorDatas.push_back(rd);

                ro.color = true;

                uint32_t const index = m_tcIndices.empty() ? 0 : m_tcIndices.back() + 1;
                uint32_t indices[]   = {index, index + 1, index + 2, index, index + 2, index + 3};
                m_tcIndices.insert(m_tcIndices.end(), indices, indices + 6);
            }
        }
        m_renderObjects.push_back(ro);
    }

    void RenderBackendOpenGL::addImageToArray(
        const Rect& rect,
        uint32_t id1,
        float const * st1,
        uint32_t id2,
        float const * st2,
        uint8_t alpha,
        uint8_t const * rgba)
    {
        if (rgba != nullptr) {
            renderData2TC rd{};
            rd.vertex[0] = static_cast<float>(rect.x);
            rd.vertex[1] = static_cast<float>(rect.y);
            rd.texel[0]  = st1[0];
            rd.texel[1]  = st1[1];
            rd.texel2[0] = st2[0];
            rd.texel2[1] = st2[1];
            rd.color[0]  = 255;
            rd.color[1]  = 255;
            rd.color[2]  = 255;
            rd.color[3]  = alpha;
            m_renderMultitextureDatas.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x);
            rd.vertex[1] = static_cast<float>(rect.y + rect.h);
            rd.texel[1]  = st1[3];
            rd.texel2[1] = st2[3];
            m_renderMultitextureDatas.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x + rect.w);
            rd.vertex[1] = static_cast<float>(rect.y + rect.h);
            rd.texel[0]  = st1[2];
            rd.texel2[0] = st2[2];
            m_renderMultitextureDatas.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x + rect.w);
            rd.vertex[1] = static_cast<float>(rect.y);
            rd.texel[1]  = st1[1];
            rd.texel2[1] = st2[1];
            m_renderMultitextureDatas.push_back(rd);

            uint32_t const index = m_tc2Indices.empty() ? 0 : m_tc2Indices.back() + 1;
            uint32_t indices[]   = {index, index + 1, index + 2, index, index + 2, index + 3};
            m_tc2Indices.insert(m_tc2Indices.end(), indices, indices + 6);

            RenderObject ro(GL_TRIANGLES, 6, id1, id2);
            ro.overlay_type = OVERLAY_TYPE_TEXTURES_AND_FACTOR;
            ro.rgba[0]      = rgba[0];
            ro.rgba[1]      = rgba[1];
            ro.rgba[2]      = rgba[2];
            ro.rgba[3]      = rgba[3];
            m_renderObjects.push_back(ro);
        }
    }

    RenderBackendOpenGL::RenderZObjectTest* RenderBackendOpenGL::getRenderBufferObject(GLuint texture_id)
    {
        for (auto& m_renderZ_object : m_renderZ_objects) {
            if (m_renderZ_object.texture_id == texture_id) {
                if (m_renderZ_object.elements < m_renderZ_object.max_size - 4) {
                    return &m_renderZ_object;
                }
            }
        }
        int32_t const max_quads_per_texbatch = 1000;
        // nothing was found (or we were forced to make new batch), we need to create new one
        RenderZObjectTest obj{};
        if (!m_renderZ_objects.empty()) {
            obj.index = m_renderZ_objects.back().index + m_renderZ_objects.back().max_size;
        } else {
            obj.index = 0;
        }
        obj.texture_id = texture_id;
        obj.elements   = 0;
        obj.max_size   = static_cast<uint32_t>(max_quads_per_texbatch * 4);

        m_renderZ_objects.push_back(obj);
        return &m_renderZ_objects.back();
    }

    void RenderBackendOpenGL::addImageToArrayZ(
        uint32_t id, const Rect& rect, float vertexZ, float const * st, uint8_t alpha, uint8_t const * rgba)
    {
        // texture quad without alpha and coloring
        if (alpha == 255 && (rgba == nullptr)) {
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

            renderDataZ rd{};
            rd.vertex[0] = static_cast<float>(rect.x);
            rd.vertex[1] = static_cast<float>(rect.y);
            rd.vertex[2] = vertexZ;
            rd.texel[0]  = st[0];
            rd.texel[1]  = st[1];
            m_renderTextureDatasZ.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x);
            rd.vertex[1] = static_cast<float>(rect.y + rect.h);
            rd.texel[1]  = st[3];
            m_renderTextureDatasZ.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x + rect.w);
            rd.vertex[1] = static_cast<float>(rect.y + rect.h);
            rd.texel[0]  = st[2];
            m_renderTextureDatasZ.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x + rect.w);
            rd.vertex[1] = static_cast<float>(rect.y);
            rd.texel[1]  = st[1];
            m_renderTextureDatasZ.push_back(rd);

            RenderZObject ro{};
            ro.texture_id = id;
            m_renderTextureObjectsZ.push_back(ro);
        } else {
            // multitexture with color, second texel is used for m_maskOverlay
            if (rgba != nullptr) {
                renderData2TCZ rd{};
                rd.vertex[0] = static_cast<float>(rect.x);
                rd.vertex[1] = static_cast<float>(rect.y);
                rd.vertex[2] = vertexZ;
                rd.texel[0]  = st[0];
                rd.texel[1]  = st[1];
                rd.texel2[0] = 0.0;
                rd.texel2[1] = 0.0;
                rd.color[0]  = 255;
                rd.color[1]  = 255;
                rd.color[2]  = 255;
                rd.color[3]  = alpha;
                m_renderMultitextureDatasZ.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x);
                rd.vertex[1] = static_cast<float>(rect.y + rect.h);
                rd.texel[1]  = st[3];
                rd.texel2[1] = 1.0;
                m_renderMultitextureDatasZ.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x + rect.w);
                rd.vertex[1] = static_cast<float>(rect.y + rect.h);
                rd.texel[0]  = st[2];
                rd.texel2[0] = 1.0;
                m_renderMultitextureDatasZ.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x + rect.w);
                rd.vertex[1] = static_cast<float>(rect.y);
                rd.texel[1]  = st[1];
                rd.texel2[1] = 0.0;
                m_renderMultitextureDatasZ.push_back(rd);

                RenderObject ro(GL_TRIANGLES, 6, id);
                ro.color        = true;
                ro.overlay_type = OVERLAY_TYPE_COLOR;
                ro.rgba[0]      = rgba[0];
                ro.rgba[1]      = rgba[1];
                ro.rgba[2]      = rgba[2];
                ro.rgba[3]      = rgba[3];
                m_renderMultitextureObjectsZ.push_back(ro);
                // texture with alpha
            } else {
                renderDataColorZ rd{};
                rd.vertex[0] = static_cast<float>(rect.x);
                rd.vertex[1] = static_cast<float>(rect.y);
                rd.vertex[2] = vertexZ;
                rd.texel[0]  = st[0];
                rd.texel[1]  = st[1];
                rd.color[0]  = 255;
                rd.color[1]  = 255;
                rd.color[2]  = 255;
                rd.color[3]  = alpha;
                m_renderTextureColorDatasZ.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x);
                rd.vertex[1] = static_cast<float>(rect.y + rect.h);
                rd.texel[1]  = st[3];
                m_renderTextureColorDatasZ.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x + rect.w);
                rd.vertex[1] = static_cast<float>(rect.y + rect.h);
                rd.texel[0]  = st[2];
                m_renderTextureColorDatasZ.push_back(rd);

                rd.vertex[0] = static_cast<float>(rect.x + rect.w);
                rd.vertex[1] = static_cast<float>(rect.y);
                rd.texel[1]  = st[1];
                m_renderTextureColorDatasZ.push_back(rd);

                RenderZObject ro{};
                ro.texture_id = id;
                m_renderTextureColorObjectsZ.push_back(ro);
            }
        }
    }

    void RenderBackendOpenGL::addImageToArrayZ(
        const Rect& rect,
        float vertexZ,
        uint32_t id1,
        float const * st1,
        uint32_t id2,
        float const * st2,
        uint8_t alpha,
        uint8_t const * rgba)
    {
        if (rgba != nullptr) {
            renderData2TCZ rd{};
            rd.vertex[0] = static_cast<float>(rect.x);
            rd.vertex[1] = static_cast<float>(rect.y);
            rd.vertex[2] = vertexZ;
            rd.texel[0]  = st1[0];
            rd.texel[1]  = st1[1];
            rd.texel2[0] = st2[0];
            rd.texel2[1] = st2[1];
            rd.color[0]  = 255;
            rd.color[1]  = 255;
            rd.color[2]  = 255;
            rd.color[3]  = alpha;
            m_renderMultitextureDatasZ.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x);
            rd.vertex[1] = static_cast<float>(rect.y + rect.h);
            rd.texel[1]  = st1[3];
            rd.texel2[1] = st2[3];
            m_renderMultitextureDatasZ.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x + rect.w);
            rd.vertex[1] = static_cast<float>(rect.y + rect.h);
            rd.texel[0]  = st1[2];
            rd.texel2[0] = st2[2];
            m_renderMultitextureDatasZ.push_back(rd);

            rd.vertex[0] = static_cast<float>(rect.x + rect.w);
            rd.vertex[1] = static_cast<float>(rect.y);
            rd.texel[1]  = st1[1];
            rd.texel2[1] = st2[1];
            m_renderMultitextureDatasZ.push_back(rd);

            RenderObject ro(GL_TRIANGLES, 6, id1, id2);
            ro.overlay_type = OVERLAY_TYPE_TEXTURES_AND_FACTOR;
            ro.rgba[0]      = rgba[0];
            ro.rgba[1]      = rgba[1];
            ro.rgba[2]      = rgba[2];
            ro.rgba[3]      = rgba[3];
            m_renderMultitextureObjectsZ.push_back(ro);
        }
    }

    void RenderBackendOpenGL::prepareForOverlays()
    {
        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);

        if (m_maskOverlay == 0) {
            // Constant texture - can be constant across every tilesets
            glGenTextures(1, &m_maskOverlay);

            uint8_t dummydata[3] = {127, 127, 127};
            glBindTexture(GL_TEXTURE_2D, m_maskOverlay);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, dummydata);
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
        // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    void RenderBackendOpenGL::captureScreen(const std::string& filename)
    {
        const uint32_t swidth  = getWidth();
        const uint32_t sheight = getHeight();

        uint8_t* pixels      = nullptr;
        SDL_Surface* surface = SDL_CreateRGBSurface(
            0, toInt32Dimension(swidth), toInt32Dimension(sheight), 24, RMASK, GMASK, BMASK, NULLMASK);

        if (surface == nullptr) {
            return;
        }

        SDL_LockSurface(surface);
        pixels = new uint8_t[swidth * sheight * 3];
        glReadPixels(
            0, 0, toGLsizei(swidth), toGLsizei(sheight), GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(pixels));
        auto* imagepixels = reinterpret_cast<uint8_t*>(surface->pixels);
        // Copy the "reversed_image" memory to the "image" memory
        for (int32_t y = toInt32Dimension(sheight) - 1; y >= 0; --y) {
            uint8_t* rowbegin = pixels + (y * swidth * 3);
            uint8_t* rowend   = rowbegin + (swidth * 3);

            std::copy(rowbegin, rowend, imagepixels);

            // Advance a row in the output surface.
            imagepixels += surface->pitch;
        }

        SDL_UnlockSurface(surface);
        Image::saveAsPng(filename, *surface);

        SDL_FreeSurface(surface);
        delete[] pixels;
    }

    void RenderBackendOpenGL::captureScreen(const std::string& filename, uint32_t width, uint32_t height)
    {
        const uint32_t swidth  = getWidth();
        const uint32_t sheight = getHeight();
        const bool same_size   = (width == swidth && height == sheight);

        if (width < 1 || height < 1) {
            return;
        }

        if (same_size) {
            captureScreen(filename);
            return;
        }

        uint8_t* pixels = nullptr;
        // create source surface
        SDL_Surface* src = SDL_CreateRGBSurface(
            0, toInt32Dimension(swidth), toInt32Dimension(sheight), 32, RMASK, GMASK, BMASK, AMASK);

        if (src == nullptr) {
            return;
        }

        if (SDL_MUSTLOCK(src)) {
            SDL_LockSurface(src);
        }
        pixels = new uint8_t[swidth * sheight * 4];
        glReadPixels(
            0, 0, toGLsizei(swidth), toGLsizei(sheight), GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(pixels));

        auto* imagepixels = reinterpret_cast<uint8_t*>(src->pixels);
        // Copy the "reversed_image" memory to the "image" memory
        for (int32_t y = toInt32Dimension(sheight) - 1; y >= 0; --y) {
            uint8_t* rowbegin = pixels + (y * swidth * 4);
            uint8_t* rowend   = rowbegin + (swidth * 4);

            std::copy(rowbegin, rowend, imagepixels);

            // Advance a row in the output surface.
            imagepixels += src->pitch;
        }

        // create destination surface
        SDL_Surface* dst =
            SDL_CreateRGBSurface(0, toInt32Dimension(width), toInt32Dimension(height), 32, RMASK, GMASK, BMASK, AMASK);

        auto* src_pointer          = static_cast<uint32_t*>(src->pixels);
        uint32_t* src_help_pointer = src_pointer;
        auto* dst_pointer          = static_cast<uint32_t*>(dst->pixels);

        int32_t x      = 0;
        int32_t y      = 0;
        int32_t* sx_ca = nullptr;
        int32_t* sy_ca = nullptr;
        auto sx        = static_cast<int32_t>(0xffff * src->w / dst->w);
        auto sy        = static_cast<int32_t>(0xffff * src->h / dst->h);
        int32_t sx_c   = 0;
        int32_t sy_c   = 0;

        // Allocates memory and calculates row wide&height
        auto* sx_a = new int32_t[static_cast<std::size_t>(dst->w) + 1U];
        sx_ca      = sx_a;
        for (x = 0; x <= dst->w; x++) {
            *sx_ca = sx_c;
            sx_ca++;
            sx_c &= 0xffff;
            sx_c += sx;
        }

        auto* sy_a = new int32_t[static_cast<std::size_t>(dst->h) + 1U];
        sy_ca      = sy_a;
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
            sx_ca       = sx_a;
            for (x = 0; x < dst->w; x++) {
                *dst_pointer = *src_pointer;
                sx_ca++;
                src_pointer += (*sx_ca >> 16);
                dst_pointer++;
            }
            sy_ca++;
            src_help_pointer = reinterpret_cast<uint32_t*>(
                reinterpret_cast<uint8_t*>(src_help_pointer) + ((*sy_ca >> 16) * src->pitch));
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

    void RenderBackendOpenGL::setClipArea(const Rect& cliparea, bool clear)
    {
        glScissor(
            cliparea.x,
            toGLint(getHeight()) - cliparea.y - cliparea.h,
            toGLsizei(static_cast<uint32_t>(cliparea.w)),
            toGLsizei(static_cast<uint32_t>(cliparea.h)));
        if (clear) {
            if (m_isbackgroundcolor) {
                auto red   = static_cast<float>(m_backgroundcolor.r / 255.0);
                auto green = static_cast<float>(m_backgroundcolor.g / 255.0);
                auto blue  = static_cast<float>(m_backgroundcolor.b / 255.0);
                glClearColor(red, green, blue, 0.0);
                m_isbackgroundcolor = false;
            }
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }

    void RenderBackendOpenGL::attachRenderTarget(ImagePtr& img, bool discard)
    {
        // flush down what we batched for the old target
        renderVertexArrays();

        m_img_target     = img;
        m_target_discard = discard;

        // to render on something, we need to make sure its loaded already in gpu memory
        m_img_target->forceLoadInternal();
        m_target = m_img_target->getSurface();

        auto* glimage = dynamic_cast<GLImage*>(m_img_target.get());

        GLuint const targetid = glimage->getTexId();
        uint32_t const w      = m_img_target->getWidth();
        uint32_t const h      = m_img_target->getHeight();

        // quick & dirty hack for attaching compressed texture
        if (glimage->isCompressed()) {
            bindTexture(targetid);
            auto* pixels = new GLubyte[w * h * 4];
            // here we get decompressed pixels
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, toGLsizei(w), toGLsizei(h), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            delete[] pixels;
            glimage->setCompressed(false);
        }

        // can we use fbo?
        if (GLEW_EXT_framebuffer_object && m_useframebuffer) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo_id);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, targetid, 0);

            // currenty unused, is needed in case the static layers should be rendered with depth buffer (instead of
            // sorting) see LayerCache
            // glBindRenderbuffer(GL_RENDERBUFFER, m_depthbuffer_id);
            // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
            // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbuffer_id);
        }

        glViewport(0, 0, toGLsizei(w), toGLsizei(h));
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // invert top with bottom
        glOrtho(0, w, 0, h, -100, 100);
        glMatrixMode(GL_MODELVIEW);
        // because of inversion 2 lines above we need to also invert culling faces
        glCullFace(GL_FRONT);

        if (m_target_discard) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } else if (!m_target_discard && (!GLEW_EXT_framebuffer_object || !m_useframebuffer)) {
            // if we wanna just add something to render target, we need to first render previous contents
            addImageToArray(
                targetid,
                m_img_target->getArea(),
                dynamic_cast<GLImage*>(m_img_target.get())->getTexCoords(),
                255,
                nullptr);
        }
    }

    void RenderBackendOpenGL::detachRenderTarget()
    {
        assert(m_target != m_screen);

        // flush down what we batched
        renderVertexArrays();

        if (GLEW_EXT_framebuffer_object && m_useframebuffer) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        } else {
            bindTexture(0, dynamic_cast<GLImage*>(m_img_target.get())->getTexId());
            glCopyTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA8,
                0,
                0,
                toGLsizei(m_img_target->getWidth()),
                toGLsizei(m_img_target->getHeight()),
                0);
        }

        m_target = m_screen;
        glViewport(0, 0, toGLsizei(static_cast<uint32_t>(m_screen->w)), toGLsizei(static_cast<uint32_t>(m_screen->h)));
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, m_screen->w, m_screen->h, 0, -100, 100);
        glMatrixMode(GL_MODELVIEW);
        glCullFace(GL_BACK);
    }

    void RenderBackendOpenGL::renderGuiGeometry(
        const std::vector<GuiVertex>& vertices,
        const std::vector<int>& indices,
        const DoublePoint& translation,
        ImagePtr texture)
    {

        glPushMatrix();
        glTranslatef(static_cast<GLfloat>(translation.x), static_cast<GLfloat>(translation.y), 0.0F);

        glVertexPointer(2, GL_DOUBLE, sizeof(GuiVertex), &vertices[0].position);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(GuiVertex), &vertices[0].color);

        GLuint texId = 0;

        auto* glImage = dynamic_cast<GLImage*>(texture.get());
        if (glImage != nullptr) {
            glImage->forceLoadInternal();
            texId = glImage->getTexId();
        }

        if (texId == 0) {
            glDisable(GL_TEXTURE_2D);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        } else {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texId);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_DOUBLE, sizeof(GuiVertex), &vertices[0].texCoords);
        }

        glDrawElements(GL_TRIANGLES, toGLsizei(indices.size()), GL_UNSIGNED_INT, indices.data());

        glPopMatrix();
    }
} // namespace FIFE
