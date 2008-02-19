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

#ifndef FIFE_ZIP_DATA_H
#define FIFE_ZIP_DATA_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include "unzip.h"

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdatasource.h"

namespace FIFE {
	class ZipData : public RawDataSource {
	public:
		virtual ~ZipData();

		unsigned int getSize() const;
		void readInto(uint8_t* buffer, unsigned int start, unsigned int length);

	private:
		/** Creates a new zip data source.
		 * Note that this constructor will not do any error checking.
		 * It will assume that the zip file exists and that the file inside it
		 * also exists. That is why this constructor is private: only the 
		 * ZipSource is really supposed to use it, and hence why it is our
		 * friend.
		 *
		 * @param zip_file the name of the zip file to read the data from.
		 * @param file the name of the file from the zip to read.
		 */
		ZipData(const std::string& zip_file, const std::string& file);
		friend class ZipSource;

		unzFile m_file;
		unsigned int m_size;
	};

} //FIFE

#endif
