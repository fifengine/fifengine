// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_GLAD_H
#define FIFE_GLCORE_GLAD_H

// The generated loader with the highest API level available in this build
// covers all lower profiles in the same family.
#if defined(HAVE_OPENGL_43)
    #include "video/glcore/glad/core43/glad.h"
#elif defined(HAVE_OPENGLES_31)
    #include "video/glcore/glad/es31/glad.h"
#elif defined(HAVE_OPENGL_CORE)
    #include "video/glcore/glad/core/glad.h"
#else
    #include "video/glcore/glad/es2/glad.h"
#endif

#endif
