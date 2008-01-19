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
#include <fstream>

// 3rd party library includes
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "vfs/raw/rawdatafile.h"

#include "vfshostsystem.h"

namespace bfs = boost::filesystem;
namespace FIFE {

	VFSHostSystem::VFSHostSystem() {
	}


	VFSHostSystem::~VFSHostSystem() {
	}


	bool VFSHostSystem::fileExists(const std::string& name) const {
		std::ifstream file(name.c_str());
		if (file)
			return true;

		return false;
	}

	RawData* VFSHostSystem::open(const std::string& file) const {
		return new RawData(new RawDataFile(file));
	}

	std::vector<std::string> VFSHostSystem::listFiles(const std::string& pathstr) const {
		return list(pathstr, false);
	}

	std::vector<std::string> VFSHostSystem::listDirectories(const std::string& path) const {
		return list(path, true);
	}

	std::vector<std::string> VFSHostSystem::list(const std::string& pathstr, bool directorys) const {
		std::vector<std::string> list;
		bfs::path path(pathstr);
		if (!bfs::exists(path) || !bfs::is_directory(path))
			return list;

		bfs::directory_iterator end;
		for (bfs::directory_iterator i(path); i != end; ++i) {
			if (bfs::is_directory(*i) != directorys)
				continue;

			list.push_back(i->leaf());
		}

		return list;
	}
}
