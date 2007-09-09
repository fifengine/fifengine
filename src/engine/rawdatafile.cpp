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

#include "rawdatafile.h"
#include "exception.h"

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


	unsigned int RawDataFile::getSize( ) const {
		return m_filesize;
	}

	void RawDataFile::readInto(uint8_t* buffer, unsigned int start, unsigned int length) {
		m_stream.seekg(start);
		m_stream.read(reinterpret_cast<char*>(buffer), length);
	}

}

