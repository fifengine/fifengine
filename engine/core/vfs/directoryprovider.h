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

#ifndef FIFE_VFS_DIRECTORYPROVIDER_H
#define FIFE_VFS_DIRECTORYPROVIDER_H

// Standard C++ library includes
#include <string>
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfssourceprovider.h"

namespace FIFE {

	class VFSSource;

	/** Provider for OS directories.
	 */
	class DirectoryProvider : public VFSSourceProvider {
		public:
			DirectoryProvider() : VFSSourceProvider("OS Directory") { }

			/** Check if a given directory is readable.
 			 * The path must either be an absolute path or relative
             * to the current working directory where the client was launched
			 *
			 * @param path the filename to check
			 * @return true if readable, false otherwise
			 */
			virtual bool isReadable(const std::string& path) const;

			/** Create a new instance of a VFSSource initialized with the given directory
			 * This will only pass if the path is readable and recognized
             * as a directory as per the description of isReadable
             *
			 * @param path the directory to open
			 * @return the new VFSSource
			 */
			virtual VFSSource* createSource(const std::string& path);

			/** Get the source instance of the path
			 *
			 * @param path The source path
			 * @return A VFSSource or NULL of none is present
			 */
			virtual VFSSource* getSource(const std::string& path) const;

			/** Check whether the provider already has created a source with that path
			*
			* @param path The path to the source
                        * @return true if the provider has already created a source with that path, false if not
                        */
			virtual bool hasSource(const std::string & path) const;
		private:
			std::string m_name;
			std::map<std::string, VFSSource* > m_sources;

	};
}

#endif
