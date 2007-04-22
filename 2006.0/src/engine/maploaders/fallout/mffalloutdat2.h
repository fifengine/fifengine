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
	class MFFalloutDAT2 : public VFSSource {

		public:
			MFFalloutDAT2(const std::string& path);
			virtual ~MFFalloutDAT2();

			const MFFalloutRawDataDAT2::s_info& getInfo(const std::string& name) const;

			virtual bool fileExists(const std::string& name) const;
			virtual RawData* open(const std::string& file) const;

		private:
			std::string m_datpath;
			RawDataPtr m_data;
			typedef std::map<std::string, MFFalloutRawDataDAT2::s_info> type_filelist;
			type_filelist m_filelist;

			void readFileList();
			MFFalloutDAT2(const MFFalloutDAT2&);
			MFFalloutDAT2& operator=(const MFFalloutDAT2&);
	};

}

#endif
