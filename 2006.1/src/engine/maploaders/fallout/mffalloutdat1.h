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

#ifndef FIFEMFFALLOUTDAT1_H
#define FIFEMFFALLOUTDAT1_H

#include "vfs.h"
#include "vfssource.h"
#include "mffalloutrawdatadat1.h"

#include <map>

namespace FIFE {

	/** VFSource for the Fallout1 DAT file format.
	 */
	class MFFalloutDAT1 : public VFSSource {
		public:
			/** Constructor
			 * Create a VFSSource for a Fallout1 DAT file.
			 * @param file A Fallout1 DAT file - e.g. master.DAT
			 */
			MFFalloutDAT1(const std::string& file);

			/** Destructor
			 */
			virtual ~MFFalloutDAT1();

			virtual bool fileExists(const std::string& name) const;
			virtual RawData* open(const std::string& file) const;

			/** Get the needed information to unpack and extract a file from the DAT file.
			 * Retrieves the Information needed to extract a specific file, this
			 * is passed as argument to a mffalloutrawdatadat1 RawMemSource,
			 * which itself fills its memory content with the unpacked file.
			 *
			 *  @throw NotFound
			 *  @see MFFalloutRawDataDAT1, RawMemSource
			 */
			const MFFalloutRawDataDAT1::s_info& getInfo(const std::string& name) const;

			VFS::type_stringlist listFiles(const std::string & pathstr) const;
			VFS::type_stringlist listDirectories(const std::string & pathstr) const;

		private:
			std::string m_datpath;
			RawDataPtr m_data;
			typedef std::map<std::string, MFFalloutRawDataDAT1::s_info> type_filelist;
			type_filelist m_filelist;

			VFS::type_stringlist list(const std::string& pathstr, bool dirs) const;
			void readIndex();
			void loadFileList(const std::string& dirname);
			std::string readString();

			MFFalloutDAT1(const MFFalloutDAT1&);
			MFFalloutDAT1& operator=(const MFFalloutDAT1&);
	};

}

#endif
