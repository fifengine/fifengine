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

#ifndef FIFE_ZIP_SOURCE_H
#define FIFE_ZIP_SOURCE_H

// Standard C++ library includes
//
#include <map>

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"
#include "vfs/vfssource.h"

namespace FIFE {
	/**  Implements a Zip archive file source.
	 *
	 * @see FIFE::VFSSource
	 */
	class ZipSource : public VFSSource {
		public:
			ZipSource(VFS* vfs, const std::string& zip_file);
			~ZipSource();

			/// WARNING: fileExists, listFiles and listDirectories are not
			// thread-safe, and will probably break if called from multiple
			// threads at the same time.
			bool fileExists(const std::string& file) const;
			std::set<std::string> listFiles(const std::string& path) const;
			std::set<std::string> listDirectories(const std::string& path) const;

			virtual RawData* open(const std::string& path) const;

		private:
			struct s_data {
					uint16_t comp;
					uint32_t crc32;
					std::string path;
					unsigned int size_comp;
					unsigned int size_real;
					unsigned int offset;
				};

			typedef std::map<std::string, s_data> type_files;

			RawData* m_zipfile;
			type_files m_files;

			void readIndex();
			bool readFileToIndex();
	};

} //FIFE

#endif
