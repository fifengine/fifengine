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

#ifndef FIFE_VFS_VFSUTILITY_H
#define FIFE_VFS_VFSUTILITY_H

// Standard C++ library includes
#include <vector>
#include <string>

// 3rd party library includes

// FIFE includes

namespace FIFE {
	class VFS;
	class VFSUtility {
		public:
			VFSUtility();
			virtual ~VFSUtility();
			std::vector<uint8_t> readBytes(const std::string& fname);
			std::vector<std::string> readLines(const std::string& fname);
			std::vector<std::string> listFiles(const std::string& path);
			std::vector<std::string> listDirectories(const std::string& path);
			bool addSource(const std::string& sname);
		private:
			VFS* vfs;
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
