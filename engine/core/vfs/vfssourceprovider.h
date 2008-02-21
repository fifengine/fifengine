/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_VFS_VFSSOURCEPROVIDER_H
#define FIFE_VFS_VFSSOURCEPROVIDER_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	class VFSSource;

	/** VFSSourceProvider abstract baseclass
	 *
	 * If you add support for a new archive-type (like the fallout1&dat files) you should
	 * also create a new VFSSourceProvider for your VFSSource - once this is done
	 * its very easy to load new archives even without knowing their real type.
	 * @see VFS
	 * @see VFSSource
	 * @see VFSSourceFactory
	 */
	class VFSSourceProvider{
		public:
			VFSSourceProvider(const std::string& name);
			virtual ~VFSSourceProvider();

			/** Returns the name of this VFSSourceProvider
			 *
			 * @return name
			 */
			const std::string& getName() const;

			/** Get the VFS this provider is using. Providers will not be able to locate
			 * sources before a VFS is set. VFS::addProvider sets this automatically.
			 */
			void setVFS(VFS* vfs);

			VFS* getVFS() const { return m_vfs; }

			/** Check if a given file is readable for this VFSSource
			 *
			 *
			 * @param file the filename to check
			 * @return true if readable, false otherwise
			 */
			virtual bool isReadable(const std::string& file) const = 0;

			/** Create a new instance of a VFSSource initialized with the given file
			 *
			 * @param file the filename to open (the archive-file)
			 * @return the new VFSSource
			 */
			virtual VFSSource* createSource(const std::string& file) const = 0;

		private:

			VFS* m_vfs;
			std::string m_name;
	};
}

#endif
