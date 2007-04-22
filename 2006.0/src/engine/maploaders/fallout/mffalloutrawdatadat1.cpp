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
#include "mffalloutrawdatadat1.h"
#include "rawdata.h"
#include "exception.h"
#include "lzssdecoder.h"

namespace FIFE {

	MFFalloutRawDataDAT1::MFFalloutRawDataDAT1(const std::string& datfile, const s_info& info) : m_data(new uint8_t[info.unpackedLength]), m_datalen(info.unpackedLength) {

		RawDataPtr input = VFS::instance()->open(datfile);
		input->setIndex(info.offset);

		if (info.type == 0x40) { // compressed
			LZSSDecoder decoder;
			decoder.decode(input, &m_data[0], m_datalen);
		} else
			input->readInto(m_data.get(), info.unpackedLength);
	}


	MFFalloutRawDataDAT1::~MFFalloutRawDataDAT1() {}

	MFFalloutRawDataDAT1::MFFalloutRawDataDAT1(const MFFalloutRawDataDAT1&) : m_data(0), m_datalen(0) {}

	MFFalloutRawDataDAT1& MFFalloutRawDataDAT1::operator=(const MFFalloutRawDataDAT1&) {
		return *this;
	}

	void MFFalloutRawDataDAT1::readInto(uint8_t* buffer, unsigned int start, unsigned int length) {
		std::copy(m_data.get() + start, m_data.get() + start + length, buffer);
	}

	unsigned int MFFalloutRawDataDAT1::getSize( ) const {
		return m_datalen;
	}

}
