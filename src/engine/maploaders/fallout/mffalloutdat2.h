/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef FIFEMFFALLOUTDAT_H
#define FIFEMFFALLOUTDAT_H

#include <string>
#include <map>
#include <stdint.h>

#include "vfs.h"
#include "vfssource.h"
#include "mffalloutrawdatadat2.h"

namespace FIFE {

	class RawData;
	/** VFSource for the Fallout2 DAT file format
	 *
	 * @see MFFalloutDAT1
	 */
	class MFFalloutDAT2 : public VFSSource {

		public:
			/** Constructor
			 * Create a VFSSource for a Fallout2 DAT file.
			 * @param file A Fallout2 DAT file - e.g. master.DAT
			 */
			MFFalloutDAT2(const std::string& path);

			/** Destructor
			 */
			virtual ~MFFalloutDAT2();

			/** Get Information needed to unpack and extract data
			 *
			 * @see MFFalloutDAT1::getInfo
			 */
			const MFFalloutRawDataDAT2::s_info& getInfo(const std::string& name) const;

			virtual bool fileExists(const std::string& name) const;
			virtual RawData* open(const std::string& file) const;

			VFS::type_stringlist listFiles(const std::string & pathstr) const;
			VFS::type_stringlist listDirectories(const std::string & pathstr) const;

		private:
			std::string m_datpath;
			RawDataPtr m_data;
			typedef std::map<std::string, MFFalloutRawDataDAT2::s_info> type_filelist;
			type_filelist m_filelist;

			VFS::type_stringlist list(const std::string& pathstr, bool dirs) const;
			void readFileList();
			MFFalloutDAT2(const MFFalloutDAT2&);
			MFFalloutDAT2& operator=(const MFFalloutDAT2&);
	};

}

#endif
