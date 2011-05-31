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
#include <boost/version.hpp>

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

namespace 
{
    // grab the major and minor version of boost, 
    // calculations taken from boost/version.hpp
    #define BOOST_MAJOR_VERSION BOOST_VERSION / 100000
    #define BOOST_MINOR_VERSION BOOST_VERSION / 100 % 1000

#if (BOOST_MAJOR_VERSION >= 1 && BOOST_MINOR_VERSION >= 46)
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
	static Logger _log(LM_VFS);

	VFSDirectory::VFSDirectory(VFS* vfs, const std::string& root) : VFSSource(vfs), m_root(root) {
		FL_DBG(_log, LMsg("VFSDirectory created with root path ") << m_root);
		if(!m_root.empty() && *(m_root.end() - 1) != '/')
			m_root.append(1,'/');
	}


	VFSDirectory::~VFSDirectory() {
	}


	bool VFSDirectory::fileExists(const std::string& name) const {
		std::string fullFilename = m_root + name;

        bfs::path fullPath(fullFilename);
		std::ifstream file(fullPath.file_string().c_str());

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

#if defined(USE_BOOST_FILESYSTEM_V3)
                // boost version 1.46 and above uses
                // boost filesystem version 3 as the default
                // which has yet a different way of getting
                // a filename string
                bfs::path filenamePath = i->path().filename();
                list.insert(filenamePath.string());
#elif defined(USE_NON_DEPRECATED_BOOST_FILESYSTEM_V2)
                // the new way in boost filesystem version 2
                // to get a filename string
                //(this is for boost version 1.36 and above)
                list.insert(i->path().filename());
#else
                // the old way in boost filesystem version 2
                // to get a filename string 
                //(this is for boost version 1.35 and below)
                list.insert(i->leaf());
#endif
			}
		}
		catch (const bfs::filesystem_error& ex) {
			throw Exception(ex.what());
		}

		return list;
	}
}
