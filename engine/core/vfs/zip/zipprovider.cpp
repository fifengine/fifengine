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
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "vfs/vfs.h"

#include "zipprovider.h"
#include "zipsource.h"

namespace FIFE {
	bool ZipProvider::isReadable(const std::string& file) const {
		// File name must have a .zip extension:
		// TODO: Case sensitive?
		if (file.find(".zip") == std::string::npos)
			return false;

		// File should exist:
		if (!getVFS()->exists(file))
			return false;

		// File should start with the bytes "PK":
		// TODO: ...

		return true;
	}

	FIFE::VFSSource* ZipProvider::createSource(const std::string& file) {
		if (isReadable(file)) {
			VFSSource* source = NULL;
			if ( hasSource(file)) {
				source = m_sources[file];
			} else {
				source = new ZipSource(getVFS(), file);
				m_sources[file] = source;
			}
			return source;
		}
		else
			throw Exception("File " + file + " is not readable.");
	}

	VFSSource* ZipProvider::getSource(const std::string& path) const {
		VFSSource* source = NULL;
		if (hasSource(path)) {
			source = m_sources.find(path)->second;
		}
		return source;
	}

	bool ZipProvider::hasSource(const std::string & path) const {
		return m_sources.count(path) > 0;
	}
}
