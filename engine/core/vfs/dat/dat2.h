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

#ifndef FIFE_MAP_LOADERS_FALLOUT_DAT2_H
#define FIFE_MAP_LOADERS_FALLOUT_DAT2_H

// Standard C++ library includes
#include <map>
#include <string>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes
#include <boost/scoped_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/time/timer.h"
#include "vfs/vfs.h"
#include "vfs/vfssource.h"

#include "rawdatadat2.h"

namespace FIFE {
	class RawData;

	/** VFSource for the Fallout2 DAT file format
	 *
	 *  Implements a kind of lazy initializing, by reading the file list
	 *  in chunks. Behaviour is the same as if it wouldn't do this,
	 *  but startup is very fast. But a open/fileExists call with a
	 *  filename that doesn't exist, does trigger completely loading
	 *  the file entries.
	 *
	 * @see MFFalloutDAT1
	 * @todo @b maybe merge common DAT1/DAT2 code in a common base class
	 */
	class DAT2 : public VFSSource {

		public:
			/** Constructor
			 * Create a VFSSource for a Fallout2 DAT file.
			 * @param vfs A pointer to the VFS.
			 * @param path A Fallout2 DAT file - e.g. master.DAT
			 */
			DAT2(VFS* vfs, const std::string& path);

			bool fileExists(const std::string& name) const;
			RawData* open(const std::string& file) const;

			/** Get Information needed to unpack and extract data
			 *
			 * @see MFFalloutDAT1::getInfo
			 */
			const RawDataDAT2::s_info& getInfo(const std::string& name) const;

			std::set<std::string> listFiles(const std::string& pathstr) const;
			std::set<std::string> listDirectories(const std::string& pathstr) const;

		private:
			std::string m_datpath;
			mutable boost::scoped_ptr<RawData> m_data;
			typedef std::map<std::string, RawDataDAT2::s_info> type_filelist;
			mutable type_filelist m_filelist;

			/// number of file entries to read
			mutable uint32_t m_filecount;
			/// current index in file
			mutable uint32_t m_currentIndex;
			/// lazy loading timer
			mutable Timer m_timer;

			/// read a bunch of file entries
			void readFileEntry() const;

			/// find a file entry
			type_filelist::const_iterator findFileEntry(const std::string& name) const;

			std::set<std::string> list(const std::string& pathstr, bool dirs) const;

			// Not copyable
			DAT2(const DAT2&);
			DAT2& operator=(const DAT2&);
	};

} // FIFE

#endif
