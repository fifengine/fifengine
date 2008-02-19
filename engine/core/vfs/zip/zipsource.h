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

#ifndef FIFE_ZIP_SOURCE_H
#define FIFE_ZIP_SOURCE_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfssource.h"

namespace FIFE {
	/**  Implements a Zip archive file source.
	 *
	 * @see FIFE::VFSSource
	 */
	class ZipSource : public VFSSource {
	public:
		ZipSource(const std::string& zip_file);
		~ZipSource();

		/// WARNING: fileExists, listFiles and listDirectories are not
		// thread-safe, and will probably break if called from multiple
		// threads at the same time.
		bool fileExists(const std::string& file) const;
		RawData* open(const std::string& file) const;
		std::set<std::string> listFiles(const std::string& path) const;
		std::set<std::string> listDirectories(const std::string& path) const;

	private:
		// PImpl pattern, so the users of this header don't need to know
		// about minizip.
		struct zip_data_t;
		struct zip_data_t* m_data;
	};

} //FIFE

#endif
