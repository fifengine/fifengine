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

#ifndef FIFEMFFALLOUTRAWDATADAT_H
#define FIFEMFFALLOUTRAWDATADAT_H

#include "rawdatasource.h"
#include <string>
#include <stdint.h>
#include <boost/scoped_array.hpp>

namespace FIFE {

	class MFFalloutRawDataDAT2 : public RawDataSource {
		public:
			struct s_info {
				std::string name;
				uint32_t packedLength;
				uint32_t unpackedLength;
				uint32_t offset;
				uint32_t type;
			};

			MFFalloutRawDataDAT2(const std::string& datfile, const s_info& info);
			virtual ~MFFalloutRawDataDAT2();

			virtual unsigned int getSize() const;
			virtual void readInto(uint8_t* buffer, unsigned int start, unsigned int length);

		private:
			boost::scoped_array<uint8_t> m_data;
			uint32_t m_datalen;

			MFFalloutRawDataDAT2(const MFFalloutRawDataDAT2&);
			MFFalloutRawDataDAT2& operator=(const MFFalloutRawDataDAT2&);
	};

}

#endif
