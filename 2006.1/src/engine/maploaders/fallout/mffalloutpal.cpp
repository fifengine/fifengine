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

#include "mffalloutpal.h"
#include "exception.h"
#include "debugutils.h"
#include "vfs.h"
#include "rawdata.h"

static const size_t PALSIZE = 256*3;

namespace FIFE {

	MFFalloutPAL::MFFalloutPAL(const std::string& file) : m_palette(new uint8_t[PALSIZE]) {
		RawDataPtr input = VFS::instance()->open(file);

		if (input->getDataLength() < PALSIZE) {
			Log("mlfallout") << "filesize: " << input->getDataLength() << " wanted: " << PALSIZE;
			throw InvalidFormat("size mismatch in " + file);
		}
		input->readInto(m_palette, PALSIZE);
	}

	MFFalloutPAL::~MFFalloutPAL() {
		delete[] m_palette;
	}

	MFFalloutPAL::MFFalloutPAL(const MFFalloutPAL&) : m_palette(0) {
	}

	MFFalloutPAL& MFFalloutPAL::operator=(const MFFalloutPAL&) {
		return *this;
	}

	uint8_t MFFalloutPAL::get(size_t index) const {
			if (index >= PALSIZE) throw IndexOverflow("invalid index");

			return m_palette[index];
		}

	uint32_t MFFalloutPAL::operator[](uint8_t index) const {
		return getRGB(index);
	}

	uint32_t MFFalloutPAL::getRGB(uint8_t index) const {
		if (static_cast<unsigned int>(index*3 + 2) >= PALSIZE) {
			throw IndexOverflow("invalid index");
		}

		size_t sindex = index * 3;
		uint32_t rgba = (m_palette[sindex] << 24) | (m_palette[sindex + 1] << 16) | (m_palette[sindex + 2] << 8) | 0xff;
		return rgba;
	}

	uint8_t MFFalloutPAL::getRed(uint8_t index) const {
		return get(index * 3);
	}

	uint8_t MFFalloutPAL::getGreen(uint8_t index) const {
		return get(index * 3 + 1);
	}

	uint8_t MFFalloutPAL::getBlue(uint8_t index) const {
		return get(index * 3 + 2);
	}

}//FIFE
