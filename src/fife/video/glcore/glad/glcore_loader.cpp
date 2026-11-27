// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "glcore_loader.h"

#include <SDL3/SDL.h>

#include "video/glcore/glad/glcore_glad.h"
#include "video/glcore/glcontext.h"

namespace FIFE
{
    bool glcoreLoadProcs(GLProfile profile)
    {
        bool const requestedCore = profile == GLProfile::Core33 || profile == GLProfile::Core43;
#if defined(HAVE_OPENGL_CORE) || defined(HAVE_OPENGL_43)
        if (!requestedCore) {
            return false;
        }
#else
        if (requestedCore) {
            return false;
        }
#endif
        // glcore_glad.h selects the highest API generated in this build; its
        // loader covers every lower profile of the same family.
#if defined(HAVE_OPENGL_CORE) || defined(HAVE_OPENGL_43)
        return gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) != 0;
#elif defined(HAVE_OPENGL_ES2) || defined(HAVE_OPENGLES_31)
        return gladLoadGLES2Loader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) != 0;
#else
        return false;
#endif
    }
} // namespace FIFE
