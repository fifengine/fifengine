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

#ifndef FIFE_VFS_VFSSOURCE_H
#define FIFE_VFS_VFSSOURCE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs.h"

namespace FIFE {

	class RawData;
	/** VFSSource abstract baseclass
	 *
	 * VFSSources are used to provide transparent access to diffrent archives/filesystems etc.
	 * Examples are VFSHostSystem
	 * @see VFS
	 * @see VFSSourceProvider
	 */
	class VFSSource {
		public:
			VFSSource(VFS* vfs);
			virtual ~VFSSource();

			/** check if the given file exists
			 *
			 * @param file filename to check
			 * @return true if it exists, false otherwise
			 */
			virtual bool fileExists(const std::string& file) const = 0;

			/** get the VFS this source is associated with.
			 */
			VFS* getVFS() const { return m_vfs; }

			/** open a file inside this source
			 *
			 * @param file the file to open
			 * @return a new RawData*
			 * @throws CannotOpenFile if the file can't be found
			 */
			virtual RawData* open(const std::string& file) const = 0;

			/** list all files in a directory of this source
			 * 
			 * @param path path to list files in
			 * Will return an empty list on \b any error
			 */
			virtual std::set<std::string> listFiles(const std::string& path) const = 0;

			/** list all directories in a directory of this source
			 * 
			 * @param path path to list directories in
			 * Will return an empty list on \b any error
			 */
			virtual std::set<std::string> listDirectories(const std::string& path) const = 0;

		protected:
			std::string fixPath(std::string path) const;

		private:
			VFS* m_vfs;
	};

}

#endif
