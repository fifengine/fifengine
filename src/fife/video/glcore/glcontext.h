// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCONTEXT_H
#define FIFE_GLCONTEXT_H

#include <SDL3/SDL_video.h>

#include <cstdint>
#include <utility>

#include "platform.h"

namespace FIFE
{
    enum class GLProfile : uint8_t
    {
        Core33,
        ES2,
        Core43,
        ES31
    };

    struct GLContextConfig
    {
            GLProfile profile{GLProfile::Core33};
            int32_t major{3};
            int32_t minor{3};
            bool debug{false};
            int32_t stencilBits{8};
            int32_t depthBits{24};
            bool vsync{false};
            int32_t msaaSamples{0};
    };

    struct GLAttributeValues
    {
            int major{3};
            int minor{3};
            SDL_GLProfile profileMask{SDL_GL_CONTEXT_PROFILE_CORE};
    };

    FIFE_API GLAttributeValues glAttributesFor(GLContextConfig const & config);

    struct GLCapabilities
    {
            bool computeShaders{false};
            bool shaderStorageBuffers{false};
    };
    FIFE_API GLCapabilities capabilitiesFor(GLProfile profile);

    FIFE_API GLContextConfig makeCore33Config();
    FIFE_API GLContextConfig makeES2Config();
    FIFE_API GLContextConfig makeCore43Config();
    FIFE_API GLContextConfig makeES31Config();

    class FIFE_API GLContext
    {
        public:
            explicit GLContext(GLContextConfig const & config);
            ~GLContext();

            GLContext(GLContext const &)            = delete;
            GLContext& operator=(GLContext const &) = delete;

            void applyAttributes() const;
            bool create(SDL_Window* window);
            bool makeCurrent(SDL_Window* window);
            void swap(SDL_Window* window) const;
            void destroy();

            bool isLoaded() const
            {
                return m_context != nullptr && m_procsLoaded;
            }

            SDL_GLContext native() const
            {
                return m_context;
            }

            GLContextConfig const & config() const
            {
                return m_config;
            }

            bool hasComputeShaders() const
            {
                return capabilitiesFor(m_config.profile).computeShaders;
            }

            bool hasShaderStorageBuffers() const
            {
                return capabilitiesFor(m_config.profile).shaderStorageBuffers;
            }

        private:
            GLContextConfig m_config;
            SDL_GLContext m_context{nullptr};
            bool m_procsLoaded{false};
    };

    FIFE_API void glcoreGetDrawableSize(SDL_Window* window, uint32_t& w, uint32_t& h);
} // namespace FIFE

#endif
