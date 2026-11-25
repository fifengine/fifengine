// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "glcontext.h"

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "video/glcore/glad/glcore_loader.h"

namespace FIFE
{
    GLAttributeValues glAttributesFor(GLContextConfig const & config)
    {
        bool const isCore = config.profile == GLProfile::Core33 || config.profile == GLProfile::Core43;
        GLAttributeValues values;
        switch (config.profile) {
        case GLProfile::Core33:
            values.major = 3;
            values.minor = 3;
            break;
        case GLProfile::ES2:
            values.major = 2;
            values.minor = 0;
            break;
        case GLProfile::Core43:
            values.major = 4;
            values.minor = 3;
            break;
        case GLProfile::ES31:
            values.major = 3;
            values.minor = 1;
            break;
        }
        values.profileMask = isCore ? SDL_GL_CONTEXT_PROFILE_CORE : SDL_GL_CONTEXT_PROFILE_ES;
        return values;
    }

    GLCapabilities capabilitiesFor(GLProfile profile)
    {
        GLCapabilities caps;
        caps.computeShaders       = (profile == GLProfile::Core43 || profile == GLProfile::ES31);
        caps.shaderStorageBuffers = caps.computeShaders;
        return caps;
    }

    GLContextConfig makeCore33Config()
    {
        GLContextConfig config;
        config.profile     = GLProfile::Core33;
        config.major       = 3;
        config.minor       = 3;
        config.stencilBits = 8;
        config.depthBits   = 24;
        return config;
    }

    GLContextConfig makeES2Config()
    {
        GLContextConfig config;
        config.profile     = GLProfile::ES2;
        config.major       = 2;
        config.minor       = 0;
        config.stencilBits = 8;
        config.depthBits   = 24;
        return config;
    }

    GLContextConfig makeCore43Config()
    {
        GLContextConfig config;
        config.profile     = GLProfile::Core43;
        config.major       = 4;
        config.minor       = 3;
        config.stencilBits = 8;
        config.depthBits   = 24;
        return config;
    }

    GLContextConfig makeES31Config()
    {
        GLContextConfig config;
        config.profile     = GLProfile::ES31;
        config.major       = 3;
        config.minor       = 1;
        config.stencilBits = 8;
        config.depthBits   = 24;
        return config;
    }

    GLContext::GLContext(GLContextConfig const & config) : m_config(config)
    {
    }

    GLContext::~GLContext()
    {
        destroy();
    }

    void GLContext::applyAttributes() const
    {
        GLAttributeValues const attrs = glAttributesFor(m_config);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, attrs.major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, attrs.minor);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, static_cast<int>(attrs.profileMask));
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_config.stencilBits);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_config.depthBits);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        if (m_config.debug) {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        }
    }

    bool GLContext::create(SDL_Window* window)
    {
        if (window == nullptr) {
            return false;
        }
        if (!SDL_GL_LoadLibrary(nullptr)) {
            return false;
        }
        m_context = SDL_GL_CreateContext(window);
        if (m_context == nullptr) {
            return false;
        }
        m_procsLoaded = glcoreLoadProcs(m_config.profile);
        if (!m_procsLoaded) {
            SDL_GL_DestroyContext(m_context);
            m_context = nullptr;
            return false;
        }
        SDL_GL_SetSwapInterval(m_config.vsync ? 1 : 0);
        return true;
    }

    bool GLContext::makeCurrent(SDL_Window* window)
    {
        if (m_context == nullptr || window == nullptr) {
            return false;
        }
        return SDL_GL_MakeCurrent(window, m_context);
    }

    void GLContext::swap(SDL_Window* window) const
    {
        if (window != nullptr) {
            SDL_GL_SwapWindow(window);
        }
    }

    void GLContext::destroy()
    {
        if (m_context != nullptr) {
            SDL_GL_DestroyContext(m_context);
            m_context = nullptr;
        }
        m_procsLoaded = false;
    }

    void glcoreGetDrawableSize(SDL_Window* window, uint32_t& w, uint32_t& h)
    {
        int width  = 0;
        int height = 0;
        if (window != nullptr && SDL_GetWindowSizeInPixels(window, &width, &height)) {
            w = static_cast<uint32_t>(std::max(1, width));
            h = static_cast<uint32_t>(std::max(1, height));
            return;
        }
        w = 1;
        h = 1;
    }
} // namespace FIFE
