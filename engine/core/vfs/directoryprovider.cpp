/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"
#include "vfs/vfs.h"

#include "directoryprovider.h"
#include "vfsdirectory.h"

namespace FIFE {
	bool DirectoryProvider::isReadable(const std::string& path) const {
		// TODO: Checking for a trailing slash isn't a very good test for whether
		// this is a directory. Better idea: split the path on the '/' token and
		// recursively check for membership in VFS::ListDirectories. C++ doesn't
		// seem to have built in string splitting though. --jwt
		if(path.empty() || *(path.end() - 1) != '/')
			return false;

		return true;
	}

	FIFE::VFSSource* DirectoryProvider::createSource(const std::string& path) const {
		if (isReadable(path))
			return new VFSDirectory(path);
		else
			throw Exception("Path " + path + " is not readable.");
	}
}
