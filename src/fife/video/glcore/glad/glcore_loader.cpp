// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "glcore_loader.h"

#include <SDL3/SDL.h>

#include "video/glcore/glcontext.h"

#ifdef HAVE_OPENGL_CORE
    #include "video/glcore/glad/core/glad.h"
#elif defined(HAVE_OPENGL_ES2)
    #include "video/glcore/glad/es2/glad.h"
#endif

namespace FIFE
{
    bool glcoreLoadProcs(GLProfile profile)
    {
#if defined(HAVE_OPENGL_CORE)
        if (profile == GLProfile::Core33) {
            return gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) != 0;
        }
#endif
#if defined(HAVE_OPENGL_ES2)
        if (profile == GLProfile::ES2) {
            return gladLoadGLES2Loader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) != 0;
        }
#endif
        return false;
    }
} // namespace FIFE
