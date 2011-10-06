/***************************************************************************
*   Copyright (C) 2005-2008 by the FIFE team                              *
*   http://www.fifengine.de                                               *
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
#include <string>

// 3rd party library includes
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/version.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "fife_boost_filesystem.h"

namespace 
{
    // grab the major and minor version of boost, 
    // calculations taken from boost/version.hpp
#define BOOST_MAJOR_VERSION BOOST_VERSION / 100000
#define BOOST_MINOR_VERSION BOOST_VERSION / 100 % 1000

#if (BOOST_MAJOR_VERSION >= 1 && BOOST_MINOR_VERSION >= 44 && defined(BOOST_FILESYSTEM_VERSION))
    #if (BOOST_FILESYSTEM_VERSION == 2)
        // if this macro is defined to 2 the the user wants to
        // force the use of boost filesystem version 2 so we
        // will set our internal macros correctly to do that
        #define USE_NON_DEPRECATED_BOOST_FILESYSTEM_V2
    #elif (BOOST_FILESYSTEM_VERSION == 3)
        // if this macro is set to 3 then the user wants to force
        // the use of boost filesystem version 3 so we will set
        // our internal macros correctly to do that
        #define USE_BOOST_FILESYSTEM_V3
    #endif
#elif (BOOST_MAJOR_VERSION >= 1 && BOOST_MINOR_VERSION >= 46)
    // this define will tell us to use boost filesystem
    // version 3 since this is the default version of the library
    // starting in boost version 1.46 and above
    #define USE_BOOST_FILESYSTEM_V3
#elif (BOOST_MAJOR_VERSION >= 1 && BOOST_MINOR_VERSION >= 36)
    // this define will tell us not to use the deprecated functions
    // in boost filesystem version 2 library which were introduced
    // in boost version 1.36 and above
    #define USE_NON_DEPRECATED_BOOST_FILESYSTEM_V2
#endif
}

namespace FIFE {

    bool HasParentPath(const bfs::path& path) {
        #if defined(USE_BOOST_FILESYSTEM_V3) || defined(USE_NON_DEPRECATED_BOOST_FILESYSTEM_V2)
            return path.has_parent_path();
        #else
            return path.has_branch_path();
        #endif
    }

    bfs::path GetParentPath(const bfs::path& path) {
        #if defined(USE_BOOST_FILESYSTEM_V3) || defined(USE_NON_DEPRECATED_BOOST_FILESYSTEM_V2)
            return path.parent_path();
        #else
            return path.branch_path();
        #endif
    }

    std::string GetFilenameFromDirectoryIterator(const bfs::directory_iterator& iter) {
        bfs::directory_iterator badIter;

        // early exit for bad directory_iterator parameter
        if (iter == badIter) {
            return "";
        }

        #if defined(USE_BOOST_FILESYSTEM_V3)
            // boost version 1.46 and above uses
            // boost filesystem version 3 as the default
            // which has yet a different way of getting
            // a filename string
            return iter->path().filename().string();
        #elif defined(USE_NON_DEPRECATED_BOOST_FILESYSTEM_V2)
            // the new way in boost filesystem version 2
            // to get a filename string
            //(this is for boost version 1.36 and above)
            return iter->path().filename();
        #else
            // the old way in boost filesystem version 2
            // to get a filename string 
            //(this is for boost version 1.35 and below)
            return iter->leaf();
        #endif
    }
}