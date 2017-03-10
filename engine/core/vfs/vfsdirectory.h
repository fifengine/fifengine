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

#ifndef FIFE_VFS_VFSHOSTSYSTEM_H
#define FIFE_VFS_VFSHOSTSYSTEM_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfssource.h"

namespace FIFE {

	/** The most basic VFSSource for "normal" filesystems.
	 * For example, '/' or './tests/data'.
	 *
	 * Uses boost_filesystem to achieve Plattform independancy.
	 * This also means you have to use slashes as directory
	 * separators.
	 */
	class VFSDirectory : public VFSSource {
		public:
			/** Constructor
			 * Creates the given file system's VFS Source, Uses boost_filesystem to
			 * achieve Plattform independancy.
			 */
			VFSDirectory(VFS* vfs, const std::string& root = "./");
			/** Destructor
			 */
			virtual ~VFSDirectory();

			/** Tests whether a file can be opened.
			 * @param filename The file to test.
			 * @return True, if the file filename can be opened.
			 */
			virtual bool fileExists(const std::string& filename) const;
			/** Opens a file.
			 * @param filename The file to open.
			 */
			virtual RawData* open(const std::string& filename) const;

			/** List files in a directory
			 * @param path The directory to list the files in
			 * @return A string list of filenames
			 * Will return an empty list on \b any error
			 */
			std::set<std::string> listFiles(const std::string& path) const;

			/** List directories in a directory
			 * @param path The directory to list the directories in
			 * @return A string list of directories
			 * Will return an empty list on \b any error
			 */
			std::set<std::string> listDirectories(const std::string& path) const;

		private:
			std::string m_root;

			std::set<std::string> list(const std::string& path, bool directorys) const;

	};

}

#endif
