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

#ifndef FIFE_MAP_LOADERS_FALLOUT_DAT1_H
#define FIFE_MAP_LOADERS_FALLOUT_DAT1_H

// Standard C++ library includes
#include <map>
#include <boost/scoped_ptr.hpp>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfs.h"
#include "vfs/vfssource.h"

#include "rawdatadat1.h"

namespace FIFE {

	/** VFSource for the Fallout1 DAT file format.
	 */
	class DAT1 : public VFSSource {
		public:
			/** Constructor
			 * Create a VFSSource for a Fallout1 DAT file.
			 * @param vfs A pointer to the VFS.
			 * @param file A Fallout1 DAT file - e.g. master.DAT
			 */
			DAT1(VFS* vfs, const std::string& file);

			bool fileExists(const std::string& name) const;
			RawData* open(const std::string& file) const;

			/** Get the needed information to unpack and extract a file from the
			 * DAT file.
			 * Retrieves the Information needed to extract a specific file, this
			 * is passed as argument to a mffalloutrawdatadat1 RawMemSource,
			 * which itself fills its memory content with the unpacked file.
			 *
			 *  @throw NotFound
			 *  @see MFFalloutRawDataDAT1, RawMemSource
			 */
			const RawDataDAT1::s_info& getInfo(const std::string& name) const;

			std::set<std::string> listFiles(const std::string& pathstr) const;
			std::set<std::string> listDirectories(const std::string& pathstr) const;

		private:
			std::string m_datpath;
			boost::scoped_ptr<RawData> m_data;
			typedef std::map<std::string, RawDataDAT1::s_info> type_filelist;
			type_filelist m_filelist;

			std::set<std::string> list(const std::string& pathstr, bool dirs) const;
			std::string readString();

			void loadFileList(const std::string& dirname);

			// Not copyable
			DAT1(const DAT1&);
			DAT1& operator=(const DAT1&);
	};

} // FIFE

#endif
