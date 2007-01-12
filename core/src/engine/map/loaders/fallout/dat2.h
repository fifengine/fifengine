/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
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

#ifndef FIFE_MAP_LOADERS_FALLOUT_DAT2_H
#define FIFE_MAP_LOADERS_FALLOUT_DAT2_H

// Standard C++ library includes
#include <map>
#include <string>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfs.h"
#include "vfs/vfssource.h"

#include "rawdatadat2.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	/** VFSource for the Fallout2 DAT file format
	 *
	 * @see MFFalloutDAT1
	 */

	// XXX Merge common DAT1/DAT2 code in a common base class
	class DAT2 : public VFSSource {

		public:
			/** Constructor
			 * Create a VFSSource for a Fallout2 DAT file.
			 * @param file A Fallout2 DAT file - e.g. master.DAT
			 */
			DAT2(const std::string& path);

			bool fileExists(const std::string& name) const;
			FIFE::RawData* open(const std::string& file) const;
			// XXX Why did I need to fully qualify that?

			/** Get Information needed to unpack and extract data
			 *
			 * @see MFFalloutDAT1::getInfo
			 */
			const RawDataDAT2::s_info& getInfo(const std::string& name) const;

			VFS::type_stringlist listFiles(const std::string& pathstr) const;
			VFS::type_stringlist listDirectories(const std::string& pathstr) const;

		private:
			std::string m_datpath;
			RawDataPtr m_data;
			typedef std::map<std::string, RawDataDAT2::s_info> type_filelist;
			type_filelist m_filelist;

			VFS::type_stringlist list(const std::string& pathstr, bool dirs) const;

			// Not copyable
			DAT2(const DAT2&);
			DAT2& operator=(const DAT2&);
	};

} } } } // FIFE::map::loaders::fallout

#endif
