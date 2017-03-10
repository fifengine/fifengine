/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

#include "rawdatafile.h"

namespace FIFE {

	RawDataFile::RawDataFile(const std::string& file) : m_file(file), m_stream(m_file.c_str(), std::ios::binary), m_filesize(0) {
		if (!m_stream)
			throw CannotOpenFile(m_file);

		m_stream.seekg(0, std::ios::end);
		m_filesize = m_stream.tellg();
		m_stream.seekg(0, std::ios::beg);
	}


	RawDataFile::~RawDataFile() {
	}


	uint32_t RawDataFile::getSize( ) const {
		return m_filesize;
	}

	void RawDataFile::readInto(uint8_t* buffer, uint32_t start, uint32_t length) {
		m_stream.seekg(start);
		m_stream.read(reinterpret_cast<char*>(buffer), length);
	}

}

