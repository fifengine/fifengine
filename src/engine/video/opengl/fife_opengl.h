// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_UTIL_FIFEGL_H
#define FIFE_UTIL_FIFEGL_H

// Standard C++ library includes

// 3rd party library includes

// GLEW
// On Windows:
//   when building a static library or executable: GLEW_STATIC
//   when building a dll: GLEW_BUILD
#ifdef BUILD_SHARED_LIBS
    #define GLEW_BUILD
#else
    #define GLEW_STATIC
#endif

#include "GL/glew.h"

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE
{

   // TODO

} // FIFE

#endif
