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
        ES2
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

    GLContextConfig makeCore33Config();
    GLContextConfig makeES2Config();

    class GLContext
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

        private:
            GLContextConfig m_config;
            SDL_GLContext m_context{nullptr};
            bool m_procsLoaded{false};
    };

    void glcoreGetDrawableSize(SDL_Window* window, uint32_t& w, uint32_t& h);
} // namespace FIFE

#endif
