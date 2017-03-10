/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

#include "fife_boost_filesystem.h"
#include "vfs.h"
#include "directoryprovider.h"
#include "vfsdirectory.h"

namespace FIFE {
	bool DirectoryProvider::isReadable(const std::string& path) const {
        return bfs::is_directory(bfs::path(path));
	}

	FIFE::VFSSource* DirectoryProvider::createSource(const std::string& path) {
		if (isReadable(path)) {
			VFSSource* source = NULL;
			if ( hasSource(path)) {
				source = m_sources[path];
			} else {
				source = new VFSDirectory(getVFS(), path);
				m_sources[path] = source;
			}
			return source;
		}
		else
			throw Exception("Path " + path + " is not readable.");
	}

	VFSSource* DirectoryProvider::getSource(const std::string& path) const {
		VFSSource* source = NULL;
		if (hasSource(path)) {
			source = m_sources.find(path)->second;
		}
		return source;
	}

	bool DirectoryProvider::hasSource(const std::string & path) const {
		return m_sources.count(path) > 0;
	}
}
