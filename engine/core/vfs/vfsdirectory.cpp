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
#include "util/log/logger.h"
#include "util/base/exception.h"
#include "vfsdirectory.h"

namespace bfs = boost::filesystem;
namespace FIFE {
	static Logger _log(LM_VFS);

	VFSDirectory::VFSDirectory(VFS* vfs, const std::string& root) : VFSSource(vfs), m_root(root) {
		FL_DBG(_log, LMsg("VFSDirectory created with root path ") << m_root);
		if(!m_root.empty() && *(m_root.end() - 1) != '/')
			m_root.append(1,'/');
	}


	VFSDirectory::~VFSDirectory() {
	}


	bool VFSDirectory::fileExists(const std::string& name) const {
		std::string fullpath = m_root + name;
		std::ifstream file(fullpath.c_str());
		if (file)
			return true;

		return false;
	}

	RawData* VFSDirectory::open(const std::string& file) const {
		return new RawData(new RawDataFile(m_root + file));
	}

	std::set<std::string> VFSDirectory::listFiles(const std::string& path) const {
		return list(path, false);
	}

	std::set<std::string> VFSDirectory::listDirectories(const std::string& path) const {
		return list(path, true);
	}

	std::set<std::string> VFSDirectory::list(const std::string& path, bool directorys) const {
		std::set<std::string> list;
		std::string dir = m_root;

		// Avoid double slashes
		if(path[0] == '/' && m_root[m_root.size()-1] == '/') {
			dir.append(path.substr(1));
		}
		else {
			dir.append(path);
		}

		try {
			bfs::path boost_path(dir);
			if (!bfs::exists(boost_path) || !bfs::is_directory(boost_path))
				return list;

			bfs::directory_iterator end;
			for	(bfs::directory_iterator i(boost_path); i != end; ++i) {
				if (bfs::is_directory(*i) != directorys)
					continue;

				// This only works with boost 1.34 and up
				// list.insert(i->path().leaf());
				// This one should be ok with both 1.33 and above
				list.insert(i->leaf());
			}
		}
		catch (const bfs::filesystem_error& ex) {
			throw Exception(ex.what());
		}

		return list;
	}
}
