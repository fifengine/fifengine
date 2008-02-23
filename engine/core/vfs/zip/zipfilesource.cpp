/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "zipfilesource.h"
#include <cassert>
#include <string>

namespace FIFE {

	ZipFileSource::ZipFileSource(uint8_t* data, unsigned int datalen) : m_data(data), m_datalen(datalen) {
	}

	ZipFileSource::~ZipFileSource() {
		delete[] m_data;
	}

	unsigned int ZipFileSource::getSize() const {
		return m_datalen;
	}

	void ZipFileSource::readInto(uint8_t* target, unsigned int start, unsigned int len) {
		assert(start + len <= m_datalen);
		memcpy(target, m_data + start, len);
	}
}
