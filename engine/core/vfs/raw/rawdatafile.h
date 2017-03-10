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

#ifndef FIFE_VFS_RAW_RAWDATAFILE_H
#define FIFE_VFS_RAW_RAWDATAFILE_H

// Standard C++ library includes
#include <fstream>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "rawdatasource.h"

namespace FIFE {

	/** A RawDataSource for a file on the host system
	 * @see VFSHostSystem
	 * @see RawDataSource
	 */
	class RawDataFile : public RawDataSource {

		public:
			/** Constructor
			 * Constructs a RawDataSource for file.
			 * @param file The path to the file to load.
			 * @throw CannotOpenFile
			 */
			RawDataFile(const std::string& file);
			virtual ~RawDataFile();

			virtual uint32_t getSize() const;
			virtual void readInto(uint8_t* buffer, uint32_t start, uint32_t length);

		private:
			std::string m_file;
			std::ifstream m_stream;

			uint32_t m_filesize;

	};

}

#endif
