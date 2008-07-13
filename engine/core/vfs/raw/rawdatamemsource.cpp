/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "rawdatamemsource.h"

namespace FIFE {

	RawDataMemSource::RawDataMemSource(unsigned int len) : m_data(new uint8_t[len]), m_datalen(len) {
	}

	RawDataMemSource::~RawDataMemSource() {
		delete[] m_data;
	}

	unsigned int RawDataMemSource::getSize() const {
		return m_datalen;
	}

	void RawDataMemSource::readInto(uint8_t* buffer, unsigned int start, unsigned int length) {
		std::copy(m_data + start, m_data + start + length, buffer);
	}

	uint8_t* RawDataMemSource::getRawData() const {
		return m_data;
	}

}
