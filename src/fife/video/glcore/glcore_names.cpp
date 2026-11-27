// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "glcore_names.h"

#include <format>
#include <string>

#include "util/log/logger.h"

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_VIDEO);
            return log;
        }

        bool isGlBackend(std::string const & name)
        {
            return name == "OpenGL" || name == "OpenGL_CORE" || name == "GLES2" || name == "OpenGL_43" ||
                   name == "GLES_31";
        }
    } // namespace

    bool isGlDepthBackend(std::string const & backendName)
    {
        return isGlBackend(backendName);
    }

    bool usesOpenGLRenderTargetClip(std::string const & backendName)
    {
        return backendName != "SDL";
    }

    bool usesGLRgba32UploadFormat(std::string const & backendName)
    {
        return backendName != "SDL";
    }

    bool bindsColorLikeSDL(std::string const & backendName)
    {
        return backendName == "SDL";
    }

    bool isCompiledRenderBackend(std::string const & backendName)
    {
        if (backendName == "SDL" || backendName == "DUMMY_NULL" || backendName == "DUMMY_SURFACE") {
            return true;
        }
#ifdef HAVE_OPENGL
        if (backendName == "OpenGL") {
            return true;
        }
#endif
#ifdef HAVE_OPENGL_CORE
        if (backendName == "OpenGL_CORE") {
            return true;
        }
#endif
#ifdef HAVE_OPENGL_ES2
        if (backendName == "GLES2") {
            return true;
        }
#endif
#ifdef HAVE_OPENGL_43
        if (backendName == "OpenGL_43") {
            return true;
        }
#endif
#ifdef HAVE_OPENGLES_31
        if (backendName == "GLES_31") {
            return true;
        }
#endif
        return false;
    }

    std::string resolveCompiledBackendName(std::string const & requested)
    {
        if (isCompiledRenderBackend(requested)) {
            return requested;
        }
        FL_WARN(_log(), std::format("Render backend '{}' is not compiled in; falling back to SDL", requested));
        return "SDL";
    }
} // namespace FIFE
