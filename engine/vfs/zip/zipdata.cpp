/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"

#include "zipdata.h"

namespace FIFE { namespace zip {
	ZipData::ZipData(const std::string& zip_file, const std::string& file) {
		m_file = unzOpen(zip_file.c_str());

		int error = 0;
		error = unzLocateFile(m_file, file.c_str(), 2);
		if (error != 0)
			throw Exception("minizip error " + error);

		unz_file_info info;
		error = unzGetCurrentFileInfo(m_file, &info,
			NULL, 0, NULL, 0, NULL, 0);
		if (error != 0)
			throw Exception("minizip error " + error);
		m_size = info.uncompressed_size;

		error = unzOpenCurrentFile(m_file);
		if (error != 0)
			throw Exception("minizip error " + error);
	}

	ZipData::~ZipData() {
		unzCloseCurrentFile(m_file);
		unzClose(m_file);
	}

	unsigned int ZipData::getSize() const {
		return m_size;
	}

	void ZipData::readInto(uint8_t* buffer, unsigned int start, unsigned int length) {
		int error =
			unzReadCurrentFile(m_file, (voidp)buffer, length * sizeof(uint8_t));
		if (error < 0)
			throw Exception("minizip error " + error);
	}
}}
