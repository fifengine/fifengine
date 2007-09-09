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

#include "mffalloutrawdatadat2.h"
#include <zlib.h>
#include <algorithm>

#include "exception.h"
#include "rawdata.h"
#include "vfs.h"

namespace FIFE {

	MFFalloutRawDataDAT2::MFFalloutRawDataDAT2(const std::string& datfile, const s_info& info) : m_data(new uint8_t[info.unpackedLength]), m_datalen(info.unpackedLength) {

		RawDataPtr input = VFS::instance()->open(datfile);
		input->setIndex(info.offset);

		if (info.type == 1) { // compressed
			boost::scoped_array<uint8_t> compressed(new uint8_t[info.packedLength]);
			input->readInto(compressed.get(), info.packedLength);

			uLongf dstlen = info.unpackedLength;
			if (uncompress(m_data.get(), &dstlen, compressed.get(), info.packedLength) != Z_OK || dstlen != info.unpackedLength)
				throw InvalidFormat("failed to decompress " + info.name + " (inside: " + datfile + ")");
		} else
			input->readInto(m_data.get(), info.unpackedLength);
	}


	MFFalloutRawDataDAT2::~MFFalloutRawDataDAT2() {

	}

	MFFalloutRawDataDAT2::MFFalloutRawDataDAT2(const MFFalloutRawDataDAT2&) : m_data(0), m_datalen(0) {}

	MFFalloutRawDataDAT2& MFFalloutRawDataDAT2::operator=(const MFFalloutRawDataDAT2&) {
		return *this;
	}

	unsigned int MFFalloutRawDataDAT2::getSize() const {
		return m_datalen;
	}

	void MFFalloutRawDataDAT2::readInto(uint8_t* buffer, unsigned int start, unsigned int length) {
		std::copy(m_data.get() + start, m_data.get() + start + length, buffer);
	}

}
