/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "vfshostsystem.h"
#include "rawdata.h"
#include "rawdatafile.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <fstream>

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

	VFS::type_stringlist VFSHostSystem::listFiles(const std::string& pathstr) const {
		return list(pathstr, false);
	}

	VFS::type_stringlist VFSHostSystem::listDirectories(const std::string& path) const {
		return list(path, true);
	}

	VFS::type_stringlist VFSHostSystem::list(const std::string& pathstr, bool directorys) const {
		VFS::type_stringlist list;
		bfs::path path(pathstr);
		if (!bfs::exists(path) || !bfs::is_directory(path))
			return list;

		bfs::directory_iterator end;
		for (bfs::directory_iterator i(path); i != end; ++i) {
			if (bfs::is_directory(*i) != directorys)
				continue;

			list.insert(i->leaf());
		}

		return list;
	}
}
