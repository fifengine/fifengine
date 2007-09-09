/***************************************************************************
 *   Copyright (C) 2005 by the FIFE Team                                   *
 *   fife@developer1.cvsdude.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef FIFEMFFALLOUTDAT1_H
#define FIFEMFFALLOUTDAT1_H

#include "vfs.h"
#include "vfssource.h"
#include "mffalloutrawdatadat1.h"

#include <map>

namespace FIFE {

	class MFFalloutDAT1 : public VFSSource {
		public:
			MFFalloutDAT1(const std::string& file);
			virtual ~MFFalloutDAT1();

			virtual bool fileExists(const std::string& name) const;
			virtual RawData* open(const std::string& file) const;

			const MFFalloutRawDataDAT1::s_info& getInfo(const std::string& name) const;

		private:
			std::string m_datpath;
			RawDataPtr m_data;
			typedef std::map<std::string, MFFalloutRawDataDAT1::s_info> type_filelist;
			type_filelist m_filelist;

			void readIndex();
			void loadFileList(const std::string& dirname);
			std::string readString();

			MFFalloutDAT1(const MFFalloutDAT1&);
			MFFalloutDAT1& operator=(const MFFalloutDAT1&);
	};

}

#endif
